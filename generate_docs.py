#!/usr/bin/env python3
import os
import re
from datetime import datetime
from pathlib import Path

def find_all_source_files():
    """自動搜尋專案內所有 .c 和 .h 檔案"""
    source_files = []
    for filepath in Path(".").rglob("*.c"):
        if "build" not in str(filepath):
            source_files.append(str(filepath))
    for filepath in Path(".").rglob("*.h"):
        if "build" not in str(filepath):
            source_files.append(str(filepath))
    return sorted(source_files)

def parse_file(filepath):
    """解析 C 檔案，提取函式註解與定義"""
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
            file_tag = ""
            
            for cline in current_comment:
                if "@brief" in cline:
                    brief = cline.split("@brief", 1)[1].strip(" :")
                if "@remark" in cline:
                    remark = cline.split("@remark", 1)[1].strip()
                if "@param" in cline:
                    params.append(cline.strip())
                if "@retval" in cline:
                    retval = cline.strip()
                if "@file" in cline:
                    file_tag = cline.split("@file", 1)[1].strip()
            
            display_file = file_tag if file_tag else filepath
            
            funcs.append({
                "name": func_name,
                "params": func_params,
                "file": display_file,
                "filepath": filepath,
                "brief": brief,
                "remark": remark,
                "doc_params": params,
                "doc_retval": retval,
                "line_number": i + 1
            })
            current_comment = []
    
    return funcs

def generate_mermaid_arch():
    return '''```mermaid
graph TD
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
```'''

def generate_function_table(funcs):
    table = "| 函式名稱 | 所在檔案 | 功能說明 |\n"
    table += "|---------|----------|---------|\n"
    for f in funcs:
        table += f"| {f['name']} | {f['file']} | {f['brief']} |\n"
    return table

def generate_function_flow_diagram(func_name):
    diagrams = {
        "main": '''```mermaid
flowchart TD
    Start([開始]) --> Init[初始化中斷處理器指標]
    Init --> RegTick[設定 _rom_except_tick]
    RegTick --> RegInt[設定 _rom_except_int]
    RegInt --> Print1[列印歡迎訊息]
    Print1 --> Loop[進入主迴圈]
    Loop --> Loop
```''',
        "tick_irq_handler": '''```mermaid
flowchart TD
    Start([Tick IRQ 觸發]) --> Inc[增加 tick_count]
    Inc --> End([返回])
```''',
        "exception_irq_handler": '''```mermaid
flowchart TD
    Start([Exception IRQ 觸發]) --> Inc[增加 exception_count]
    Inc --> End([返回])
```'''
    }
    return diagrams.get(func_name, "")

def generate_index_md(all_funcs, source_files):
    content = f"""# TRAE 測試專案文件

**文件產生時間**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

---

## 專案概述

這是一個使用 TRAE IDE 開發的 C 語言嵌入式專案，包含中斷處理功能和 GitLab CI/CD 整合。

### 掃描到的原始碼檔案

| 編號 | 檔案名稱 |
|------|----------|
"""
    for i, f in enumerate(source_files, 1):
        content += f"| {i} | `{f}` |\n"
    
    content += f"""

總共掃描到 {len(source_files)} 個原始碼檔案，擷取出 {len(all_funcs)} 個函式定義。

---

## 模組架構圖

{generate_mermaid_arch()}

---

## 函式定義總覽

{generate_function_table(all_funcs)}

---

## 函式詳細說明

"""
    for f in all_funcs:
        content += f"### {f['name']}\n\n"
        content += f"**所在檔案**: `{f['file']}`\n\n"
        content += f"**功能說明**: {f['brief']}\n\n"
        
        if f['doc_params']:
            content += "**參數說明**:\n"
            for p in f['doc_params']:
                content += f"- {p}\n"
            content += "\n"
        
        if f['doc_retval']:
            content += f"**回傳值**: {f['doc_retval']}\n\n"
        
        if f['remark']:
            content += f"**備註**: {f['remark']}\n\n"
        
        flow_diag = generate_function_flow_diagram(f['name'])
        if flow_diag:
            content += "**執行流程圖**:\n\n"
            content += flow_diag + "\n"
        
        content += "\n---\n\n"
    
    content += """## 檔案下載

### 📄 PDF 文件

[下載完整說明文件 (PDF)](trae_test_documentation.pdf)

---

*本文件由 GitLab CI 自動掃描原始碼並產生*
"""
    return content

def main():
    print("=" * 60)
    print("TRAE 專案自動文件產生器")
    print("=" * 60)
    
    print("\n[1/4] 正在搜尋所有 .c 和 .h 檔案...")
    source_files = find_all_source_files()
    print(f"      找到 {len(source_files)} 個原始碼檔案:")
    for f in source_files:
        print(f"        - {f}")
    
    print("\n[2/4] 正在解析原始碼檔案...")
    all_funcs = []
    for src_file in source_files:
        if os.path.exists(src_file):
            funcs = parse_file(src_file)
            all_funcs.extend(funcs)
            print(f"      {src_file}: 找到 {len(funcs)} 個函式")
    
    print("\n[3/4] 正在建立文件目錄...")
    os.makedirs("docs", exist_ok=True)
    
    print("\n[4/4] 正在產生文件...")
    
    index_md = generate_index_md(all_funcs, source_files)
    with open("docs/index.md", "w", encoding="utf-8") as f:
        f.write(index_md)
    print("      ✓ 已產生 docs/index.md")
    
    mkdocs_content = '''site_name: TRAE Test Project Documentation
site_description: Documentation for the TRAE test project
site_author: Steven.Yang
copyright: Copyright (c) 2026 OmniVision Co.,Ltd

theme:
  name: material
  features:
    - navigation.tabs
    - navigation.sections
    - toc.integrate
    - search.suggest
    - search.highlight
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
  - codehilite
  - toc:
      permalink: true
  - pymdownx.superfences:
      custom_fences:
        - name: mermaid
          class: mermaid
          format: !!python/name:pymdownx.superfences.fence_code_format
  - pymdownx.tabbed
  - pymdownx.details
  - pymdownx.emoji
  - attr_list

plugins:
  - search

nav:
  - 首頁: index.md

extra:
  social:
    - icon: fontawesome/brands/gitlab
      link: https://gitlab.com
'''
    with open("mkdocs.yml", "w", encoding="utf-8") as f:
        f.write(mkdocs_content)
    print("      ✓ 已產生 mkdocs.yml")
    
    print("\n" + "=" * 60)
    print(f"文件產生完成！共處理 {len(source_files)} 個檔案，{len(all_funcs)} 個函式")
    print("=" * 60)

if __name__ == "__main__":
    main()
