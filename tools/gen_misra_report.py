#!/usr/bin/env python3
"""
MISRA C 2012 Static Analysis Report Generator

This script:
  1. Collects all include paths from inc/ and src/
  2. Locates the cppcheck MISRA addon (misra.py) and rule texts
  3. Runs cppcheck with MISRA C 2012 checks on src/
  4. Generates HTML report via cppcheck-htmlreport
  5. Copies the report to the docs directory for MkDocs publishing
"""

import subprocess
import os
import sys
import shutil
import glob
import re
import time

# Paths
PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC_DIR = os.path.join(PROJECT_ROOT, "src")
INC_DIR = os.path.join(PROJECT_ROOT, "inc")
OUTPUT_DIR = os.path.join(PROJECT_ROOT, "docs", "05_software_integration_test", "misra_report")
BUILD_DIR = os.path.join(PROJECT_ROOT, "build")
MISRA_BUILD_DIR = os.path.join(BUILD_DIR, "misra_build")


def run_cmd(cmd, cwd=None, check=True, capture_stderr=True):
    """Run a shell command and print output."""
    print(f"  [CMD] {' '.join(cmd)}")
    stderr_target = subprocess.PIPE if capture_stderr else None
    result = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    if result.stdout:
        for line in result.stdout.strip().split("\n")[:20]:
            print(f"        {line}")
    if check and result.returncode != 0:
        print(f"  [ERROR] Command failed with exit code {result.returncode}")
        if result.stderr:
            print(f"  [STDERR] {result.stderr.strip()[:500]}")
        sys.exit(1)
    return result


def find_misra_addon():
    """Find the cppcheck MISRA addon Python script and rule texts."""
    misra_py = None
    misra_txt = None

    # Common locations
    search_paths = [
        "/usr/share/cppcheck/addons/",
        "/usr/local/share/cppcheck/addons/",
        "/usr/lib/cppcheck/addons/",
        os.path.expanduser("~/.local/share/cppcheck/addons/"),
    ]

    for base in search_paths:
        py_path = os.path.join(base, "misra.py")
        if os.path.exists(py_path):
            misra_py = py_path
            # Look for rule texts in the same directory
            for txt_name in ["misra_2012.txt", "misra.txt", "misra_2012_rules.txt"]:
                txt_path = os.path.join(base, txt_name)
                if os.path.exists(txt_path):
                    misra_txt = txt_path
                    break
            break

    # Also check environment variables (from Docker image)
    if not misra_py:
        env_py = os.environ.get("CPPCHECK_MISRA_PY", "")
        if env_py and os.path.exists(env_py):
            misra_py = env_py
    if not misra_txt:
        env_txt = os.environ.get("CPPCHECK_MISRA_TXT", "")
        if env_txt and os.path.exists(env_txt):
            misra_txt = env_txt

    return misra_py, misra_txt


def collect_include_paths():
    """Collect all -I include paths from inc/ and src/."""
    inc_paths = []
    for directory in [INC_DIR, SRC_DIR]:
        if os.path.isdir(directory):
            inc_paths.append(f"-I{directory}")
    return inc_paths


def main():
    print("=" * 60)
    print("  MISRA C 2012 Static Analysis Report Generator")
    print("=" * 60)

    # Step 1: Ensure cppcheck and cppcheck-htmlreport are available
    print("\n[1/5] Checking tools...")
    if not shutil.which("cppcheck"):
        print("  [INFO] cppcheck not found, installing...")
        run_cmd(["apt-get", "update", "-qq"])
        run_cmd(["apt-get", "install", "-y", "-qq", "cppcheck"])
    if not shutil.which("cppcheck-htmlreport"):
        print("  [INFO] cppcheck-htmlreport not found, installing...")
        run_cmd([sys.executable, "-m", "pip", "install", "cppcheck-htmlreport"])

    cppcheck_ver = subprocess.run(["cppcheck", "--version"], capture_output=True, text=True)
    print(f"  cppcheck: {cppcheck_ver.stdout.strip()}")

    # Step 2: Locate MISRA addon
    print("\n[2/5] Locating MISRA C 2012 addon...")
    misra_py, misra_txt = find_misra_addon()

    if not misra_py:
        print("  [WARNING] MISRA addon (misra.py) not found!")
        print("  [WARNING] Please install cppcheck with MISRA addon support.")
        print("  [WARNING] Skipping MISRA check - generating placeholder report.")
        _generate_placeholder()
        return

    print(f"  MISRA addon: {misra_py}")
    if misra_txt:
        print(f"  Rule texts:  {misra_txt}")
    else:
        print("  [WARNING] MISRA rule texts not found, using defaults.")

    # Step 3: Collect include paths and create config
    print("\n[3/5] Preparing MISRA configuration...")
    inc_paths = collect_include_paths()
    print(f"  Include paths: {' '.join(inc_paths)}")

    os.makedirs(MISRA_BUILD_DIR, exist_ok=True)

    # Create cppcheck MISRA addon config
    misra_config = {
        "script": misra_py,
    }
    if misra_txt:
        misra_config["args"] = [f"--rule-texts={misra_txt}"]

    import json
    config_path = os.path.join(MISRA_BUILD_DIR, "misra_config.json")
    with open(config_path, "w") as f:
        json.dump(misra_config, f, indent=2)
    print(f"  Config written: {config_path}")

    # Step 4: Run cppcheck with MISRA checks
    print("\n[4/5] Running cppcheck MISRA analysis...")
    xml_output = os.path.join(MISRA_BUILD_DIR, "cppcheck_result.xml")

    cmd = [
        "cppcheck",
    ] + inc_paths + [
        "--enable=all",
        "--inline-suppr",
        "--xml",
        "--xml-version=2",
        f"--addon={config_path}",
        SRC_DIR,
    ]

    # cppcheck outputs XML to stderr
    result = subprocess.run(cmd, cwd=PROJECT_ROOT, capture_output=True, text=True)
    cppcheck_xml = result.stderr if result.stderr else result.stdout

    with open(xml_output, "w", encoding="utf-8") as f:
        f.write(cppcheck_xml)
    print(f"  XML output: {xml_output} ({len(cppcheck_xml)} bytes)")

    # Count violations from XML
    error_count = cppcheck_xml.count('<error ')
    print(f"  MISRA violations found: {error_count}")

    # Step 5: Generate HTML report
    print("\n[5/5] Generating HTML report...")

    # Clean output directory
    if os.path.exists(OUTPUT_DIR):
        shutil.rmtree(OUTPUT_DIR)
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # Generate HTML report
    run_cmd([
        "cppcheck-htmlreport",
        "--title=MISRA C 2012 Static Analysis Report",
        f"--file={xml_output}",
        f"--report-dir={OUTPUT_DIR}",
        f"--source-dir={PROJECT_ROOT}",
    ])

    # Generate summary snippet
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
    summary_md = (
        f"| Metric | Value |\n"
        f"|--------|-------|\n"
        f"| **MISRA Violations** | {error_count} |\n"
        f"| **Source Directory** | `src/` |\n"
        f"| **Standard** | MISRA C 2012 |\n"
        f"| **Generated** | {timestamp} |\n"
    )

    # Inject summary into misra_report.md
    report_md_path = os.path.join(PROJECT_ROOT, "docs", "05_software_integration_test", "misra_report.md")
    if os.path.exists(report_md_path):
        with open(report_md_path, "r", encoding="utf-8") as f:
            report_content = f.read()

        placeholder = "<!-- MISRA_SUMMARY_PLACEHOLDER -->"
        if placeholder in report_content:
            pattern = re.compile(
                re.escape(placeholder) + r".*?(?=\n> Run `python)",
                re.DOTALL
            )
            replacement = placeholder + "\n" + summary_md + "\n"
            report_content = pattern.sub(replacement, report_content, count=1)

            with open(report_md_path, "w", encoding="utf-8") as f:
                f.write(report_content)
            print(f"  Updated: misra_report.md with analysis results")

    print("\n" + "=" * 60)
    print(f"  MISRA report generated successfully!")
    print(f"  Report: {os.path.join(OUTPUT_DIR, 'index.html')}")
    print(f"  Violations: {error_count}")
    print("=" * 60)


def _generate_placeholder():
    """Generate a placeholder report when MISRA tools are unavailable."""
    if os.path.exists(OUTPUT_DIR):
        shutil.rmtree(OUTPUT_DIR)
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    index_html = os.path.join(OUTPUT_DIR, "index.html")
    with open(index_html, "w", encoding="utf-8") as f:
        f.write("""<!DOCTYPE html>
<html><head><meta charset="utf-8"><title>MISRA Report</title>
<style>body{font-family:sans-serif;padding:40px;text-align:center}
.warn{color:#c60;font-size:1.2em}</style></head><body>
<h1>MISRA C 2012 Report</h1>
<p class="warn">MISRA addon (misra.py) not found in the CI environment.</p>
<p>Please ensure cppcheck is installed with MISRA addon support.</p>
</body></html>""")
    print("  Placeholder report generated.")


if __name__ == "__main__":
    main()