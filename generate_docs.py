#!/usr/bin/env python3
import os
import re
import subprocess
from datetime import datetime
from pathlib import Path

# ==========================================
# Mermaid 轉換核心引擎 (PNG 實體點陣圖版)
# ==========================================
def create_puppeteer_config():
    """建立 Puppeteer 設定檔，解決 Docker root 權限問題"""
    config_path = "puppeteer-config.json"
    if not os.path.exists(config_path):
        with open(config_path, "w", encoding="utf-8") as f:
            f.write('{"args": ["--no-sandbox", "--disable-setuid-sandbox"]}')
    return config_path

def render_mermaid_to_png(mermaid_code, out_png_path):
    """呼叫 mermaid-cli 將原始碼轉為實體 PNG 圖片"""
    config_path = create_puppeteer_config()
    mmd_filename = out_png_path.replace(".png", ".mmd")
    
    with open(mmd_filename, "w", encoding="utf-8") as f:
        f.write(mermaid_code)

    try:
        # 強制轉為 PNG，設定白底，並將解析度放大 2 倍 (-s 2) 以適應 PDF 列印需求
        subprocess.run([
            "mmdc",
            "-i", mmd_filename,
            "-o", out_png_path,
            "-p", config_path,
            "-b", "white",
            "-s", "2"
        ], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        return True
    except subprocess.CalledProcessError as e:
        print(f"      [警告] Mermaid 轉換失敗 ({out_png_path}): {e.stderr.decode('utf-8', errors='ignore')}")
        return False
    finally:
        if os.path.exists(mmd_filename):
            os.remove(mmd_filename)

# ==========================================
# 專案原始碼解析
# ==========================================
def find_all_source_files():
    source_files = []
    for filepath in Path(".").rglob("*.[ch]"):
        path_str = str(filepath)
        if "build" not in path_str and "docs" not in path_str:
            source_files.append(path_str)
    return sorted(source_files)

def parse_file(filepath):
    if not os.path.exists(filepath):
        return []
    
    funcs = []
    in_comment = False
    current_comment = []
    
    with open(filepath, "r", encoding="utf-8", errors="ignore") as f:
        lines = f.readlines()
        
    for i, line in enumerate(lines):
        if "/**" in line:
            in_comment = True
            current_comment = []
            continue
            
        if in_comment:
            if "*/" in line:
                in_comment = False
                continue
            current_comment.append(line.strip())
            continue
            
        func_pattern = r"^(?:(?:void|int|char|unsigned|short|long|static|inline)\s+)+(\w+)\s*\((.*?)\)"
        func_match = re.match(func_pattern, line.strip())
        
        if func_match and current_comment:
            func_name = func_match.group(1)
            func_params = func_match.group(2)
            
            brief = ""
            remark = ""
            params = []
            retval = ""
            
            for cline in current_comment:
                if "@brief" in cline:
                    brief = cline.split("@brief", 1)[1].strip(" :")
                if "@remark" in cline:
                    remark = cline.split("@remark", 1)[1].strip()
                if "@param" in cline:
                    params.append(cline.strip())
                if "@retval" in cline:
                    retval = cline.strip()
            
            funcs.append({
                "name": func_name,
                "brief": brief,
                "remark": remark,
                "doc_params": params,
                "doc_retval": retval
            })
            current_comment = []
    
    return funcs

# ==========================================
# Markdown 與圖表生成
# ==========================================
def generate_mermaid_arch():
    """產生系統架構圖"""
    code = """graph TD
    A[start.s<br/>重置入口] --> B[_except_reset]
    B --> C[main.c<br/>main 函式]
    C --> D[註冊中斷處理器]
    D --> E[主迴圈]
    
    F[_tick_isr<br/>Tick中斷] --> G{_rom_except_tick?}
    G -->|是| H[tick_irq_handler]
    
    I[_int_isr<br/>外部中斷] --> J{_rom_except_int?}
    J -->|是| K[exception_irq_handler]
    
    C -.->|設定| G
    C -.->|設定| J
"""
    os.makedirs("docs/assets", exist_ok=True)
    png_path = "docs/assets/arch.png"
    
    # 產生物理 PNG 檔案
    if render_mermaid_to_png(code, png_path):
        # 回傳標準 Markdown 圖片語法 (使用相對路徑)
        return "![模組架構圖](assets/arch.png)"
    else:
        return f"""```mermaid\n{code}\n
```"""

def generate_function_flow_diagram(func_name, save_dir):
    """產生函式流程圖"""
    diagrams = {
        "main": """flowchart TD
    Start([開始]) --> Init[初始化中斷處理器指標]
    Init --> RegTick[設定 _rom_except_tick]
    RegTick --> RegInt[設定 _rom_except_int]
    RegInt --> Print1[列印歡迎訊息]
    Print1 --> Loop[進入主迴圈]
    Loop --> Loop
""",
        "tick_irq_handler": """flowchart TD
    Start([Tick IRQ 觸發]) --> Inc[增加 tick_count]
    Inc --> End([返回])
""",
        "exception_irq_handler": """flowchart TD
    Start([Exception IRQ 觸發]) --> Inc[增加 exception_count]
    Inc --> End([返回])
"""
    }
    
    if func_name not in diagrams:
        return ""

    code = diagrams[func_name]
    png_name = f"flow_{func_name}.png"
    png_full_path = os.path.join(save_dir, png_name)
    
    # 產生物理 PNG 檔案，放在跟 Markdown 檔案同一個資料夾
    if render_mermaid_to_png(code, png_full_path):
        return f"![{func_name} 執行流程圖]({png_name})"
    else:
        return f"""```mermaid\n{code}\n```"""

def generate_index_md(total_files, total_funcs):
    content = f"""# TRAE 測試專案文件

**文件產生時間**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

---

## 專案概述

這是一個使用 TRAE IDE 開發的 C 語言嵌入式專案，包含中斷處理功能和 GitLab CI/CD 整合。
總共掃描到 {total_files} 個原始碼檔案，擷取出 {total_funcs} 個函式定義。

---

## 模組架構圖

{generate_mermaid_arch()}

---

## 檔案下載

### 📄 PDF 文件 (包含完整架構與原始碼說明)

[📥 下載完整說明文件 (PDF)](trae_test_documentation.pdf)

---
*本文件由 GitLab CI 自動掃描原始碼並產生*
"""
    return content

def generate_file_md(filepath, funcs):
    md_rel_path = f"api/{filepath}.md".replace("\\", "/")
    md_full_path = os.path.join("docs", md_rel_path)
    save_dir = os.path.dirname(md_full_path)
    os.makedirs(save_dir, exist_ok=True)
    
    filename = os.path.basename(filepath)
    content = f"# {filename}\n\n**檔案路徑**: `{filepath}`\n\n---\n\n"
    
    if not funcs:
        content += "> *此檔案目前沒有解析到符合格式的函式註解。*\n"
        return content, md_full_path, md_rel_path

    content += "## 函式詳細說明\n\n"
    for f in funcs:
        content += f"### {f['name']}\n\n"
        content += f"**功能說明**: {f['brief']}\n\n"
        
        if f['doc_params']:
            content += "**參數說明**:\n"
            for p in f['doc_params']:
                content += f"- {p}\n"
            content += "\n"
        
        if f['doc_retval']:
            content += f"**回傳值**: {f['doc_retval']}\n\n"
        
        flow_diag = generate_function_flow_diagram(f['name'], save_dir)
        if flow_diag:
            content += "**執行流程圖**:\n\n"
            content += flow_diag + "\n\n"
            
        content += "---\n\n"
        
    return content, md_full_path, md_rel_path

def main():
    print("=" * 60)
    print("TRAE 專案自動文件產生器 (實體高畫質 PNG 終極版)")
    print("=" * 60)
    
    print("\n[1/4] 正在搜尋所有 .c 和 .h 檔案...")
    source_files = find_all_source_files()
    print(f"      找到 {len(source_files)} 個原始碼檔案")
    
    print("\n[2/4] 正在解析原始碼並呼叫 mmdc 渲染實體圖表...")
    all_funcs_count = 0
    nav_entries = []
    
    os.makedirs("docs", exist_ok=True)
    
    for src_file in source_files:
        funcs = parse_file(src_file)
        all_funcs_count += len(funcs)
        
        md_content, md_full_path, md_rel_path = generate_file_md(src_file, funcs)
        
        with open(md_full_path, "w", encoding="utf-8") as f:
            f.write(md_content)
            
        nav_entries.append(f"      - '{src_file}': {md_rel_path}")
        print(f"      ✓ 處理完成: {src_file} ({len(funcs)} 個函式，PNG 已產出)")

    print("\n[3/4] 正在產生首頁...")
    index_md = generate_index_md(len(source_files), all_funcs_count)
    with open("docs/index.md", "w", encoding="utf-8") as f:
        f.write(index_md)
        
    print("\n[4/4] 正在產生 mkdocs.yml...")
    nav_yaml_block = "\n".join(nav_entries)
    
    mkdocs_content = f'''site_name: TRAE Test Project Documentation
site_description: Documentation for the TRAE test project
site_author: Steven.Yang

theme:
  name: material
  features:
    - navigation.tabs
    - navigation.sections
    - toc.integrate
  palette:
    - scheme: default
      toggle:
        icon: material/toggle-switch-off-outline
        name: Switch to dark mode
    - scheme: slate
      toggle:
        icon: material/toggle-switch
        name: Switch to light mode

markdown_extensions:
  - admonition
  - toc:
      permalink: true

plugins:
  - search
  - pdf-export:
      combined: true
      combined_output_path: trae_test_documentation.pdf

nav:
  - 首頁: index.md
  - 原始碼結構:
{nav_yaml_block}
'''
    with open("mkdocs.yml", "w", encoding="utf-8") as f:
        f.write(mkdocs_content)
    print("      ✓ 已產生 mkdocs.yml")
    
    print("\n" + "=" * 60)
    print("文件產生完成！所有圖表均已轉為實體高畫質 PNG 檔案。")
    print("=" * 60)

if __name__ == "__main__":
    main()
