import os
import subprocess
import glob

# ==========================================
# Path Configuration
# ==========================================
BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
TARGET_SRC_DIR = os.path.join(BASE_DIR, "application", "backend", "src")
DOCS_AUTO_DIR = os.path.join(BASE_DIR, "doc", "pages", "auto_generated")
PROJECT_NAME = "DevVerifyGUI"

# 排除清單：不需要產出類別圖的檔案
EXCLUDE_FILES = ["__init__.py", "fw_bin.py", "gens_data.py", "shared_utils.py"]

def run_pyreverse(target_path: str, output_prefix: str) -> bool:
    """執行 pyreverse 並返回是否成功"""
    env = os.environ.copy()
    backend_root = os.path.join(BASE_DIR, "application", "backend")
    env["PYTHONPATH"] = f"{TARGET_SRC_DIR}{os.pathsep}{backend_root}{os.pathsep}{env.get('PYTHONPATH', '')}"
    try:
        # 如果覺得圖表還是太擠，可以在參數加入 "-k" (只顯示類別名與繼承關係)
        subprocess.run([
            "pyreverse", "-o", "mmd", "-p", output_prefix, target_path
        ], check=True, capture_output=True, text=True, env=env)
        return True
    except subprocess.CalledProcessError:
        return False

def get_mmd_content(filename: str) -> str | None:
    """讀取 mmd 檔案內容並刪除暫存檔"""
    if os.path.exists(filename):
        with open(filename, "r", encoding="utf-8") as f:
            content = f.read()
        os.remove(filename)
        return content
    return None

def generate_docs():
    """主生成邏輯"""
    os.chdir(BASE_DIR)
    os.makedirs(DOCS_AUTO_DIR, exist_ok=True)
    # --- 生成全域模組架構圖 (維持一張，看檔案間的引用關係) ---
    print("📊 Generating Module Architecture (Global)...")
    if run_pyreverse(TARGET_SRC_DIR, "GLOBAL"):
        pkg_mmd = get_mmd_content(f"packages_GLOBAL.mmd")
        # 清理 GLOBAL 產出的類別圖，因為我們下面要手動拆分
        if os.path.exists(f"classes_GLOBAL.mmd"): 
            os.remove(f"classes_GLOBAL.mmd")
        if pkg_mmd:
            target_path = os.path.join(DOCS_AUTO_DIR, "module_architecture.md")
            with open(target_path, "w", encoding="utf-8") as f:
                f.write("# Module Architecture\n\n")
                f.write("This diagram shows the dependencies between Python files.\n\n")
                f.write(f"```mermaid\n{pkg_mmd}\n```\n")
            print(f"✅ Generated: {target_path}")
    # --- 按檔案拆分生成類別圖 (一個檔案一張圖，排除指定檔案) ---
    print("🔍 Generating Split Class Diagrams by File...")
    class_md_path = os.path.join(DOCS_AUTO_DIR, "class_inheritance.md")
    with open(class_md_path, "w", encoding="utf-8") as f:
        f.write("# Class Inheritance Analysis\n\n")
        f.write("Class relationships are separated by file for better clarity.\n\n")
        # 取得 src 下所有的 .py 檔案
        py_files = glob.glob(os.path.join(TARGET_SRC_DIR, "*.py"))
        for py_file in py_files:
            file_name = os.path.basename(py_file)
            # 檢查是否在排除清單中
            if file_name in EXCLUDE_FILES:
                print(f"  ⏭️ Skipping excluded file: {file_name}")
                continue
            # 建立一個唯一的辨識前綴
            prefix = file_name.replace(".py", "").replace(".", "_")
            print(f"  -> Processing: {file_name}")
            if run_pyreverse(py_file, prefix):
                class_mmd = get_mmd_content(f"classes_{prefix}.mmd")
                # 清理掉單獨跑檔案時產出的 packages 圖
                if os.path.exists(f"packages_{prefix}.mmd"): 
                    os.remove(f"packages_{prefix}.mmd")
                if class_mmd and "classDiagram" in class_mmd:
                    f.write(f"## File: `{file_name}`\n\n")
                    f.write(f"```mermaid\n{class_mmd}\n```\n\n")
                    f.write("---\n\n")
    print(f"✨ All done! Documentation generated in {DOCS_AUTO_DIR}")

if __name__ == "__main__":
    generate_docs()
