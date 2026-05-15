#!/usr/bin/env python3
"""
gen_func_docs.py - Auto-generate function documentation with Mermaid flowcharts

Scans src/*.c files, extracts Doxygen-style comments and function signatures,
parses function bodies to generate Mermaid flowcharts, and outputs Markdown
documentation for the SWE.3 Software Detailed Design section.

Usage:
    python3 tools/gen_func_docs.py
"""

import os
import re
import glob
from typing import Optional

# ==========================================
# Path Configuration
# ==========================================
BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC_DIR = os.path.join(BASE_DIR, "src")
OUTPUT_DIR = os.path.join(BASE_DIR, "docs", "03_software_detailed_design", "auto_generated")

# ==========================================
# Language Labels
# ==========================================
LANG_LABELS = {
    "en": {
        "title": "Auto-Generated Function Documentation",
        "subtitle": "Dynamically generated from source code during CI pipeline",
        "file_header": "Source File",
        "func_list": "Function List",
        "declaration": "Declaration",
        "description": "Description",
        "parameters": "Parameters",
        "return_value": "Return Value",
        "test_criteria": "Test Criteria",
        "flowchart": "Flowchart",
        "name": "Name",
        "type": "Type",
        "direction": "Direction",
        "no_params": "None",
        "no_return": "None",
        "generated_on": "Generated on",
        "toc": "Table of Contents",
    },
    "cn": {
        "title": "自动生成的函数文档",
        "subtitle": "在 CI 流水线中从源代码动态生成",
        "file_header": "源文件",
        "func_list": "函数列表",
        "declaration": "声明",
        "description": "描述",
        "parameters": "参数",
        "return_value": "返回值",
        "test_criteria": "测试标准",
        "flowchart": "流程图",
        "name": "名称",
        "type": "类型",
        "direction": "方向",
        "no_params": "无",
        "no_return": "无",
        "generated_on": "生成时间",
        "toc": "目录",
    },
    "tw": {
        "title": "自動生成的函式文件",
        "subtitle": "在 CI 管線中從原始碼動態生成",
        "file_header": "原始檔",
        "func_list": "函式列表",
        "declaration": "宣告",
        "description": "描述",
        "parameters": "參數",
        "return_value": "回傳值",
        "test_criteria": "測試標準",
        "flowchart": "流程圖",
        "name": "名稱",
        "type": "型別",
        "direction": "方向",
        "no_params": "無",
        "no_return": "無",
        "generated_on": "生成時間",
        "toc": "目錄",
    },
}


# ==========================================
# Doxygen Comment Parser
# ==========================================
def parse_doxygen_comment(comment_block: str) -> dict:
    """Parse a Doxygen-style comment block and extract fields."""
    result = {
        "brief": "",
        "remark": "",
        "params": [],
        "retval_type": "",
        "retval_desc": "",
    }

    # Extract @brief
    brief_match = re.search(r"@brief[:\s]+(.+?)(?:\n|$)", comment_block)
    if brief_match:
        result["brief"] = brief_match.group(1).strip()

    # Extract @remark
    remark_match = re.search(r"@remark\s+(.+?)(?:\n|$)", comment_block)
    if remark_match:
        result["remark"] = remark_match.group(1).strip()

    # Extract @param lines
    param_pattern = re.compile(
        r"@param\s+\[(\w+)\]\s+(\w+)\s+"
        r"type=\[([^\]]*)\]\s+"
        r"R=\[([^\]]*)\]\s+"
        r"P=\[([^\]]*)\]\s+"
        r"N=\[([^\]]*)\]\s+"
        r"D=\[([^\]]*)\]"
    )
    for match in param_pattern.finditer(comment_block):
        result["params"].append({
            "direction": match.group(1),
            "name": match.group(2),
            "type": match.group(3),
            "R": match.group(4),
            "P": match.group(5),
            "N": match.group(6),
            "D": match.group(7),
        })

    # Extract @retval
    retval_match = re.search(
        r"@retval\s+type=\[([^\]]*)\]\s*(?:R=\[([^\]]*)\]\s*)?"
        r"(?:P=\[([^\]]*)\]\s*)?(?:N=\[([^\]]*)\]\s*)?"
        r"D=\[([^\]]*)\]",
        comment_block
    )
    if retval_match:
        result["retval_type"] = retval_match.group(1)
        result["retval_desc"] = retval_match.group(5) if retval_match.lastindex >= 5 else ""

    return result


# ==========================================
# Function Signature Parser
# ==========================================
def parse_function_signature(sig_line: str) -> Optional[dict]:
    """Parse a C function signature line and extract return type, name, and parameters."""
    # Match: return_type function_name(params)
    pattern = re.compile(
        r"^\s*"                           # leading whitespace
        r"(?:(?:static|inline|extern)\s+)*"  # optional storage class
        r"((?:unsigned\s+|signed\s+)?\w+\s*\*?)"  # return type
        r"\s+"
        r"(\w+)"                           # function name
        r"\s*\(([^)]*)\)"                  # parameters
        r"\s*\{?\s*$"                      # optional opening brace
    )
    match = pattern.match(sig_line)
    if not match:
        return None

    return_type = match.group(1).strip()
    func_name = match.group(2)
    params_str = match.group(3).strip()

    # Parse individual parameters
    params = []
    if params_str and params_str != "void":
        for param in params_str.split(","):
            param = param.strip()
            if param:
                # Try to extract type and name
                parts = param.split()
                if len(parts) >= 2:
                    ptype = " ".join(parts[:-1])
                    pname = parts[-1].rstrip("[]")
                    # Remove leading * from name if it's a pointer
                    if pname.startswith("*"):
                        ptype += " *"
                        pname = pname[1:]
                    params.append({"type": ptype, "name": pname})
                else:
                    params.append({"type": param, "name": ""})

    return {
        "return_type": return_type,
        "name": func_name,
        "params": params,
        "full_sig": sig_line.strip(),
    }


# ==========================================
# Function Body Extractor
# ==========================================
def extract_function_body(lines: list, start_idx: int) -> tuple:
    """Extract the function body by tracking brace nesting from start_idx.
    Returns (body_lines, end_idx)."""
    brace_count = 0
    body_started = False
    body_lines = []
    end_idx = start_idx

    for i in range(start_idx, len(lines)):
        line = lines[i]
        for ch in line:
            if ch == "{":
                brace_count += 1
                body_started = True
            elif ch == "}":
                brace_count -= 1
                if brace_count == 0 and body_started:
                    end_idx = i
                    return body_lines, end_idx
        if body_started:
            body_lines.append(line)
        if brace_count == 0 and body_started:
            break

    return body_lines, end_idx


# ==========================================
# Mermaid Flowchart Generator
# ==========================================
class FlowchartGenerator:
    """Generate Mermaid flowchart from C function body."""

    def __init__(self):
        self.nodes = []
        self.edges = []
        self.node_counter = 0
        self.subgraph_counter = 0

    def _new_id(self) -> str:
        nid = f"N{self.node_counter}"
        self.node_counter += 1
        return nid

    def _add_node(self, label: str, shape: str = "process") -> str:
        nid = self._new_id()
        # Use a single space for empty labels to keep Mermaid happy
        if not label or not label.strip():
            label = " "
        escaped = label.replace('"', "'").replace("\n", " ")
        if shape == "decision":
            self.nodes.append(f'    {nid}{{{{"{escaped}"}}}}')
        elif shape == "start":
            self.nodes.append(f'    {nid}(["{escaped}"])')
        elif shape == "end":
            self.nodes.append(f'    {nid}(["{escaped}"])')
        elif shape == "loop":
            self.nodes.append(f'    {nid}[("{escaped}")]')
        else:
            self.nodes.append(f'    {nid}["{escaped}"]')
        return nid

    def _add_edge(self, from_id: str, to_id: str, label: str = ""):
        if label:
            self.edges.append(f'    {from_id} -->|"{label}"| {to_id}')
        else:
            self.edges.append(f"    {from_id} --> {to_id}")

    def generate(self, func_body_lines: list) -> str:
        """Generate Mermaid flowchart string from function body lines."""
        self.nodes = []
        self.edges = []
        self.node_counter = 0

        if not func_body_lines:
            return ""

        # Clean up lines: strip leading/trailing whitespace, remove comments
        clean_lines = []
        for line in func_body_lines:
            # Remove // comments
            line = re.sub(r"//.*$", "", line)
            # Remove leading whitespace and opening brace
            line = line.strip()
            if line == "{":
                continue
            if line:
                clean_lines.append(line)

        if not clean_lines:
            return ""

        # Start node
        start_id = self._add_node("Start", "start")
        prev_id = start_id

        # Parse the body recursively
        end_id = self._parse_block(clean_lines, 0, len(clean_lines), prev_id)

        # If the last node doesn't have an outgoing edge to an end node, add one
        if end_id:
            return_id = self._add_node("Return", "end")
            self._add_edge(end_id, return_id)

        # Build Mermaid output
        mermaid = "flowchart TD\n"
        mermaid += "\n".join(self.nodes)
        mermaid += "\n"
        mermaid += "\n".join(self.edges)
        return mermaid

    def _parse_block(self, lines: list, start: int, end: int, entry_id: str) -> str:
        """Parse a block of code and return the exit node ID."""
        current_id = entry_id
        i = start

        while i < end:
            line = lines[i]

            # Skip empty lines and closing braces
            if not line or line == "}":
                i += 1
                continue

            # Handle if statement (with or without trailing {)
            if_match = re.match(r"if\s*\((.+?)\)\s*\{?\s*$", line)
            if if_match:
                condition = self._clean_condition(if_match.group(1))
                decision_id = self._add_node(condition, "decision")
                self._add_edge(current_id, decision_id)

                # Determine if brace is on same line or next line
                has_brace = "{" in line
                body_start = i if has_brace else i + 1

                # Find the if body
                if_body, if_end = self._extract_braced_block(lines, body_start, end)
                else_body = []
                else_end = if_end

                # Check for else (on same line as closing } or next line)
                if if_end < end:
                    # Check if closing brace line has else: } else { or } else
                    close_line = lines[if_end]
                    if re.search(r"\}\s*else\s*\{?\s*$", close_line):
                        # else is on the same line as closing }
                        else_body, else_end = self._extract_braced_block(lines, if_end, end)
                    elif if_end + 1 < end:
                        next_line = lines[if_end + 1]
                        if re.match(r"else\s*\{?\s*$", next_line):
                            else_body, else_end = self._extract_braced_block(lines, if_end + 1, end)

                # Parse if body
                if if_body:
                    if_start_id = self._add_node("Yes", "process")
                    self._add_edge(decision_id, if_start_id, "Yes")
                    if_exit = self._parse_simple_block(if_body, if_start_id)
                else:
                    if_exit = decision_id

                # Parse else body
                if else_body:
                    else_start_id = self._add_node("No", "process")
                    self._add_edge(decision_id, else_start_id, "No")
                    else_exit = self._parse_simple_block(else_body, else_start_id)
                else:
                    else_exit = decision_id

                # Merge point
                merge_id = self._add_node("Continue", "process")
                if if_exit and if_exit != decision_id:
                    self._add_edge(if_exit, merge_id)
                if else_exit and else_exit != decision_id:
                    self._add_edge(else_exit, merge_id)

                current_id = merge_id
                i = max(if_end, else_end) + 1
                continue

            # Handle for loop (with or without trailing {)
            for_match = re.match(r"for\s*\((.+?)\)\s*\{?\s*$", line)
            if for_match:
                condition = self._clean_condition(for_match.group(1))
                loop_id = self._add_node(f"for ({condition})", "loop")
                self._add_edge(current_id, loop_id)

                has_brace = "{" in line
                body_start = i if has_brace else i + 1
                for_body, for_end = self._extract_braced_block(lines, body_start, end)
                if for_body:
                    body_start_id = self._add_node("Loop Body", "process")
                    self._add_edge(loop_id, body_start_id)
                    body_exit = self._parse_simple_block(for_body, body_start_id)
                    if body_exit:
                        self._add_edge(body_exit, loop_id)

                # Exit path after loop
                exit_id = self._add_node("Loop End", "process")
                self._add_edge(loop_id, exit_id)
                current_id = exit_id
                i = for_end + 1
                continue

            # Handle while loop (with or without trailing {)
            while_match = re.match(r"while\s*\((.+?)\)\s*\{?\s*$", line)
            if while_match:
                condition = self._clean_condition(while_match.group(1))
                loop_id = self._add_node(f"while ({condition})", "loop")
                self._add_edge(current_id, loop_id)

                has_brace = "{" in line
                body_start = i if has_brace else i + 1
                while_body, while_end = self._extract_braced_block(lines, body_start, end)
                if while_body:
                    body_start_id = self._add_node("Loop Body", "process")
                    self._add_edge(loop_id, body_start_id)
                    body_exit = self._parse_simple_block(while_body, body_start_id)
                    if body_exit:
                        self._add_edge(body_exit, loop_id)

                # Exit path after loop
                exit_id = self._add_node("Loop End", "process")
                self._add_edge(loop_id, exit_id)
                current_id = exit_id
                i = while_end + 1
                continue

            # Handle switch statement (with or without trailing {)
            switch_match = re.match(r"switch\s*\((.+?)\)\s*\{?\s*$", line)
            if switch_match:
                switch_var = switch_match.group(1).strip()
                switch_id = self._add_node(f"switch ({switch_var})", "decision")
                self._add_edge(current_id, switch_id)

                has_brace = "{" in line
                body_start = i if has_brace else i + 1
                switch_body, switch_end = self._extract_braced_block(lines, body_start, end)

                # Count cases for collapsing
                case_count = sum(1 for l in switch_body if re.match(r"case\s+", l.strip()))

                if case_count > 8:
                    case_id = self._add_node(f"{case_count} cases handled", "process")
                    self._add_edge(switch_id, case_id)
                    current_id = case_id
                else:
                    for case_line in switch_body:
                        case_match = re.match(r"case\s+(\d+)\s*:", case_line.strip())
                        if case_match:
                            case_val = case_match.group(1)
                            case_label = f"case {case_val}"
                            case_node_id = self._add_node(case_label, "process")
                            self._add_edge(switch_id, case_node_id, case_val)
                    current_id = switch_id

                i = switch_end + 1
                continue

            # Handle return statement
            return_match = re.match(r"return\s*(.*?)\s*;", line)
            if return_match:
                ret_val = return_match.group(1).strip()
                if ret_val:
                    ret_id = self._add_node(f"return {ret_val}", "end")
                else:
                    ret_id = self._add_node("return", "end")
                self._add_edge(current_id, ret_id)
                current_id = ret_id
                i += 1
                continue

            # Handle break statement
            if re.match(r"break\s*;", line):
                break_id = self._add_node("break", "process")
                self._add_edge(current_id, break_id)
                current_id = break_id
                i += 1
                continue

            # Handle simple statements (assignments, function calls, etc.)
            stmt_match = re.match(r"(.+?);", line)
            if stmt_match:
                stmt = self._clean_stmt(stmt_match.group(1))
                if stmt and stmt not in ("{", "}"):
                    stmt_id = self._add_node(stmt, "process")
                    self._add_edge(current_id, stmt_id)
                    current_id = stmt_id
                i += 1
                continue

            i += 1

        return current_id

    def _parse_simple_block(self, lines: list, entry_id: str) -> str:
        """Parse a simple block of statements and return exit node ID."""
        current_id = entry_id
        for line in lines:
            line = line.strip()
            if not line or line == "{" or line == "}":
                continue

            # Handle if inside block
            if_match = re.match(r"if\s*\((.+?)\)\s*$", line)
            if if_match:
                condition = self._clean_condition(if_match.group(1))
                decision_id = self._add_node(condition, "decision")
                self._add_edge(current_id, decision_id)
                current_id = decision_id
                continue

            # Handle return
            return_match = re.match(r"return\s*(.*?)\s*;", line)
            if return_match:
                ret_val = return_match.group(1).strip()
                label = f"return {ret_val}" if ret_val else "return"
                ret_id = self._add_node(label, "end")
                self._add_edge(current_id, ret_id)
                current_id = ret_id
                continue

            # Handle break
            if re.match(r"break\s*;", line):
                break_id = self._add_node("break", "process")
                self._add_edge(current_id, break_id)
                current_id = break_id
                continue

            # Handle simple statement
            stmt_match = re.match(r"(.+?);", line)
            if stmt_match:
                stmt = self._clean_stmt(stmt_match.group(1))
                if stmt:
                    stmt_id = self._add_node(stmt, "process")
                    self._add_edge(current_id, stmt_id)
                    current_id = stmt_id
                continue

            # Handle function calls or other lines
            if line and not line.startswith("//"):
                stmt_id = self._add_node(self._clean_stmt(line), "process")
                self._add_edge(current_id, stmt_id)
                current_id = stmt_id

        return current_id

    def _extract_braced_block(self, lines: list, start: int, end: int) -> tuple:
        """Extract a braced block starting from start index.
        Returns (body_lines, end_index)."""
        brace_count = 0
        body_lines = []
        block_end = start - 1

        for i in range(start, end):
            line = lines[i]
            # Check if this line starts a brace
            if "{" in line and brace_count == 0:
                brace_count += line.count("{")
                # Add content after { if any
                after_brace = line.split("{", 1)[-1].strip()
                if after_brace and after_brace != "}":
                    body_lines.append(after_brace)
                continue

            for ch in line:
                if ch == "{":
                    brace_count += 1
                elif ch == "}":
                    brace_count -= 1
                    if brace_count == 0:
                        block_end = i
                        return body_lines, block_end

            if brace_count > 0:
                body_lines.append(line)

        return body_lines, block_end

    def _clean_condition(self, cond: str) -> str:
        """Clean up a condition expression for display."""
        cond = cond.strip()
        # Shorten long conditions
        if len(cond) > 40:
            cond = cond[:37] + "..."
        return cond

    def _clean_stmt(self, stmt: str) -> str:
        """Clean up a statement for display."""
        stmt = stmt.strip()
        # Remove trailing semicolons
        stmt = stmt.rstrip(";").strip()
        # Shorten long statements
        if len(stmt) > 50:
            stmt = stmt[:47] + "..."
        # Escape quotes for Mermaid
        stmt = stmt.replace('"', "'")
        return stmt


# ==========================================
# C Source File Parser
# ==========================================
def parse_c_file(filepath: str) -> list:
    """Parse a C source file and extract all function information."""
    with open(filepath, "r", encoding="utf-8") as f:
        content = f.read()

    lines = content.split("\n")
    functions = []

    i = 0
    while i < len(lines):
        line = lines[i]

        # Look for Doxygen comment block (/** ... */)
        if line.strip().startswith("/**"):
            comment_lines = []
            j = i
            while j < len(lines):
                comment_lines.append(lines[j])
                if "*/" in lines[j]:
                    break
                j += 1
            comment_block = "\n".join(comment_lines)
            doxy_info = parse_doxygen_comment(comment_block)

            # Look for function signature after the comment
            sig_idx = j + 1
            # Skip blank lines and preprocessor directives
            while sig_idx < len(lines):
                next_line = lines[sig_idx].strip()
                if next_line and not next_line.startswith("#"):
                    break
                sig_idx += 1

            if sig_idx < len(lines):
                sig_line = lines[sig_idx]
                sig_info = parse_function_signature(sig_line)

                if sig_info:
                    # Extract function body
                    body_lines, body_end = extract_function_body(lines, sig_idx)

                    # Generate flowchart
                    fg = FlowchartGenerator()
                    flowchart = fg.generate(body_lines)

                    functions.append({
                        "doxygen": doxy_info,
                        "signature": sig_info,
                        "body_lines": body_lines,
                        "flowchart": flowchart,
                        "line_start": i + 1,
                        "line_end": body_end + 1 if body_end > sig_idx else sig_idx + 1,
                        "file": os.path.basename(filepath),
                    })

                    i = body_end + 1 if body_end > sig_idx else sig_idx + 1
                    continue

            i = j + 1
            continue

        i += 1

    return functions


# ==========================================
# Markdown Generator
# ==========================================
def generate_markdown(all_functions: list, lang: str, output_dir: str):
    """Generate Markdown documentation for all functions."""
    labels = LANG_LABELS[lang]
    lang_suffix = f"_{lang}" if lang != "en" else ""

    os.makedirs(output_dir, exist_ok=True)

    # Group functions by file
    by_file = {}
    for func in all_functions:
        fname = func["file"]
        if fname not in by_file:
            by_file[fname] = []
        by_file[fname].append(func)

    # Generate per-file documentation
    for filename, funcs in by_file.items():
        file_base = filename.replace(".c", "")
        output_path = os.path.join(output_dir, f"func_{file_base}{lang_suffix}.md")

        with open(output_path, "w", encoding="utf-8") as f:
            f.write(f"# {labels['title']}: `{filename}`\n\n")
            f.write(f"> {labels['subtitle']}\n\n")
            f.write("---\n\n")

            # Table of Contents
            f.write(f"## {labels['toc']}\n\n")
            for idx, func in enumerate(funcs, 1):
                name = func["signature"]["name"]
                brief = func["doxygen"]["brief"]
                f.write(f"{idx}. [{name}](#{name.lower()}) — {brief}\n")
            f.write("\n---\n\n")

            # Detailed function documentation
            for func in funcs:
                sig = func["signature"]
                doxy = func["doxygen"]
                name = sig["name"]

                f.write(f"## {name}\n\n")

                # Summary table
                f.write("| Property | Value |\n")
                f.write("|----------|-------|\n")
                f.write(f"| **{labels['declaration']}** | `{sig['full_sig']}` |\n")
                f.write(f"| **{labels['description']}** | {doxy['brief']} |\n")
                f.write(f"| **{labels['file_header']}** | `{func['file']}` |\n")
                f.write(f"| **Line** | {func['line_start']}–{func['line_end']} |\n")
                f.write("\n")

                # Parameters
                f.write(f"### {labels['parameters']}\n\n")
                if sig["params"]:
                    f.write(f"| {labels['name']} | {labels['type']} | {labels['direction']} | {labels['description']} |\n")
                    f.write("|------|------|----------|-------------|\n")
                    for p in sig["params"]:
                        # Try to find matching doxygen param
                        desc = ""
                        for dp in doxy["params"]:
                            if dp["name"] == p["name"]:
                                desc = dp["D"]
                                break
                        f.write(f"| `{p['name']}` | `{p['type']}` | in | {desc} |\n")
                else:
                    f.write(f"{labels['no_params']}\n")
                f.write("\n")

                # Return value
                f.write(f"### {labels['return_value']}\n\n")
                if doxy["retval_type"] and doxy["retval_type"] != "N/A":
                    f.write(f"| {labels['type']} | {labels['description']} |\n")
                    f.write("|------|-------------|\n")
                    f.write(f"| `{doxy['retval_type']}` | {doxy['retval_desc']} |\n")
                else:
                    f.write(f"{labels['no_return']}\n")
                f.write("\n")

                # Test criteria
                if doxy["remark"]:
                    f.write(f"### {labels['test_criteria']}\n\n")
                    f.write(f"{doxy['remark']}\n\n")

                # Flowchart
                if func["flowchart"]:
                    f.write(f"### {labels['flowchart']}\n\n")
                    f.write("```mermaid\n")
                    f.write(func["flowchart"])
                    f.write("\n```\n\n")

                f.write("---\n\n")

        print(f"  Generated: {output_path}")

    # Generate index page
    index_path = os.path.join(output_dir, f"index{lang_suffix}.md")
    with open(index_path, "w", encoding="utf-8") as f:
        f.write(f"# {labels['title']}\n\n")
        f.write(f"> {labels['subtitle']}\n\n")
        f.write("---\n\n")

        f.write(f"## {labels['func_list']}\n\n")

        for filename, funcs in by_file.items():
            file_base = filename.replace(".c", "")
            f.write(f"### {labels['file_header']}: `{filename}`\n\n")
            f.write(f"| # | Function | {labels['declaration']} | {labels['description']} |\n")
            f.write("|---|----------|-------------|-------------|\n")
            for idx, func in enumerate(funcs, 1):
                name = func["signature"]["name"]
                sig = func["signature"]["full_sig"]
                brief = func["doxygen"]["brief"]
                detail_link = f"func_{file_base}{lang_suffix}.md#{name.lower()}"
                f.write(f"| {idx} | [{name}]({detail_link}) | `{sig}` | {brief} |\n")
            f.write("\n")

        f.write("---\n\n")
        from datetime import datetime
        f.write(f"*{labels['generated_on']}: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}*\n")

    print(f"  Generated: {index_path}")


# ==========================================
# Main Entry Point
# ==========================================
def main():
    """Main entry point: scan src/*.c and generate documentation."""
    print("=" * 60)
    print("  gen_func_docs.py - Function Documentation Generator")
    print("=" * 60)

    # Find all C source files
    c_files = glob.glob(os.path.join(SRC_DIR, "*.c"))
    if not c_files:
        print("  WARNING: No .c files found in src/ directory")
        return

    print(f"\n  Found {len(c_files)} source file(s):")
    for cf in c_files:
        print(f"    - {os.path.basename(cf)}")

    # Parse all C files
    all_functions = []
    for cf in c_files:
        print(f"\n  Parsing: {os.path.basename(cf)}")
        funcs = parse_c_file(cf)
        all_functions.extend(funcs)
        for func in funcs:
            name = func["signature"]["name"]
            has_flow = "Yes" if func["flowchart"] else "No"
            print(f"    -> {name}() [flowchart: {has_flow}]")

    print(f"\n  Total functions extracted: {len(all_functions)}")

    # Generate Markdown (English only)
    print(f"\n  Generating Markdown documentation (English only)...")
    generate_markdown(all_functions, "en", OUTPUT_DIR)

    print(f"\n  All done! Documentation generated in:")
    print(f"    {OUTPUT_DIR}")
    print("=" * 60)


if __name__ == "__main__":
    main()