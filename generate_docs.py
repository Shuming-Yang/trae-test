#!/usr/bin/env python3
import os
import re
import subprocess
import base64
import uuid
from datetime import datetime
from pathlib import Path

# ==========================================
# Mermaid 轉換核心引擎 (PNG 轉 Base64 終極版)
# ==========================================
def create_puppeteer_config():
    """建立 Puppeteer 設定檔，解決 Docker root 權限問題"""
    config_path = "puppeteer-config.json"
    if not os.path.exists(config_path):
        with open(config_path, "w", encoding="utf-8") as f:
            f.write('{"args": ["--no-sandbox", "--disable-setuid-sandbox"]}')
    return config_path

def render_mermaid_to_png_base64(mermaid_code, alt_text):
    """將 Mermaid 轉為實體 PNG，再編碼為 Base64 字串回傳，徹底解決路徑與排版問題"""
    config_path = create_puppeteer_config()
    unique_id = uuid.uuid4().hex
    temp_mmd = f"temp_{unique_id}.mmd"
    temp_png = f"temp_{unique_id}.png"
    
    with open(temp_mmd, "w", encoding="utf-8") as f:
        f.write(mermaid_code)

    try:
        # 強制轉為 PNG，設定白底，並將解析度放大 2 倍 (-s 2)
        subprocess.run([
            "mmdc",
            "-i", temp_mmd,
            "-o", temp_png,
            "-p", config_path,
            "-b", "white",
            "-s", "2"
        ], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        # 讀取 PNG 並轉為 Base64
        with open(temp_png, "rb") as f:
            encoded = base64.b64encode(f.read()).decode('utf-8')
        
        return f"![{alt_text}](data:image/png;base64,{encoded})"
    except subprocess.CalledProcessError as e:
        print(f"      [警告] Mermaid 轉換失敗: {e.stderr.decode('utf-8', errors='ignore')}")
        return f"```mermaid\n{mermaid_code}\n
```"
    finally:
        # 刪除暫存檔案，不留痕跡
        if os.path.exists(temp_mmd): os.remove(temp_mmd)
        if os.path.exists(temp_png): os.remove(temp_png)

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
    return render_mermaid_to_png_base64(code, "模組架構圖")

def generate_function_flow_diagram(func_name):
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
    return render_mermaid_to_png_base64(code, f"{func_name} 執行流程圖")

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
    os.makedirs(os.path.dirname(md_full_path), exist_ok=True)
    
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
        
        flow_diag = generate_function_flow_diagram(f['name'])
        if flow_diag:
            content += "**執行流程圖**:\n\n"
            content += flow_diag + "\n\n"
            
        content += "---\n\n"
        
    return content, md_full_path, md_rel_path

def main():
    print("=" * 60)
    print("TRAE 專案自動文件產生器 (Base64 PNG + with-pdf 排版完美版)")
    print("=" * 60)
    
    print("\n[1/4] 正在搜尋所有 .c 和 .h 檔案...")
    source_files = find_all_source_files()
    print(f"      找到 {len(source_files)} 個原始碼檔案")
    
    print("\n[2/4] 正在解析原始碼並呼叫 mmdc 渲染 Base64 實體圖表...")
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
        print(f"      ✓ 處理完成: {src_file} ({len(funcs)} 個函式，圖表已嵌入 Base64)")

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

markdown_extensions:
  - admonition
  - toc:
      permalink: true

plugins:
  - search
  - with-pdf:
      output_path: trae_test_documentation.pdf
      cover_title: "TRAE Test Project"
      cover_subtitle: "System Architecture & API Reference"
      author: "Steven.Yang"
      toc_title: "目錄"

nav:
  - 首頁: index.md
  - 原始碼結構:
{nav_yaml_block}
'''
    with open("mkdocs.yml", "w", encoding="utf-8") as f:
        f.write(mkdocs_content)
    print("      ✓ 已產生 mkdocs.yml (改用 with-pdf，告別空白頁！)")
    
    print("\n" + "=" * 60)
    print("文件產生完成！所有圖片皆為 Base64 PNG，搭配 WeasyPrint 保證排版完美。")
    print("=" * 60)

if __name__ == "__main__":
    main()
