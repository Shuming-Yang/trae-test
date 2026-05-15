#!/usr/bin/env python3
"""
Unit Test Coverage Report Generator

This script:
  1. Configures CMake with coverage flags (ENABLE_COVERAGE=ON)
  2. Builds the unit_test executable
  3. Runs unit_test and captures the test results
  4. Generates HTML coverage report (gcovr)
  5. Generates XML coverage report (Cobertura format)
  6. Copies reports to the docs directory for MkDocs publishing
"""

import subprocess
import os
import sys
import shutil
import time

# Paths
PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD_DIR = os.path.join(PROJECT_ROOT, "build")
COVERAGE_BUILD_DIR = os.path.join(BUILD_DIR, "coverage_build")
COVERAGE_OUTPUT_DIR = os.path.join(PROJECT_ROOT, "docs", "04_software_unit_verification", "coverage")


def run_cmd(cmd, cwd=None, check=True):
    """Run a shell command and print output."""
    print(f"  [CMD] {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    if result.stdout:
        for line in result.stdout.strip().split("\n"):
            print(f"        {line}")
    if check and result.returncode != 0:
        print(f"  [ERROR] Command failed with exit code {result.returncode}")
        if result.stderr:
            print(f"  [STDERR] {result.stderr.strip()}")
        sys.exit(1)
    return result


def main():
    print("=" * 60)
    print("  Unit Test Coverage Report Generator")
    print("=" * 60)

    # Step 1: Configure CMake with coverage enabled
    print("\n[1/5] Configuring CMake with coverage flags...")
    os.makedirs(COVERAGE_BUILD_DIR, exist_ok=True)
    run_cmd([
        "cmake", "-G", "Ninja",
        "-DENABLE_COVERAGE=ON",
        "-DCMAKE_BUILD_TYPE=Debug",
        "../.."
    ], cwd=COVERAGE_BUILD_DIR)

    # Step 2: Build unit_test
    print("\n[2/5] Building unit_test with coverage instrumentation...")
    run_cmd(["ninja", "unit_test"], cwd=COVERAGE_BUILD_DIR)

    # Step 3: Run unit_test
    print("\n[3/5] Running unit tests...")
    result = run_cmd(["./unit_test"], cwd=COVERAGE_BUILD_DIR, check=False)
    test_output = result.stdout
    print(f"  Test exit code: {result.returncode}")

    # Step 4: Generate coverage reports with gcovr
    print("\n[4/5] Generating coverage reports...")

    # Ensure gcovr is available
    if not shutil.which("gcovr"):
        print("  [INFO] gcovr not found, installing via pip...")
        run_cmd([sys.executable, "-m", "pip", "install", "gcovr"])

    # Generate HTML report
    print("  Generating HTML coverage report...")
    run_cmd([
        "gcovr",
        "-r", os.path.join(PROJECT_ROOT, "src"),
        "--html", "--html-details",
        "-o", os.path.join(COVERAGE_BUILD_DIR, "coverage.html"),
        "--exclude", ".*start.s.*",
        "--exclude", ".*unit_test.*",
    ], cwd=COVERAGE_BUILD_DIR)

    # Generate Cobertura XML report
    print("  Generating XML coverage report (Cobertura)...")
    run_cmd([
        "gcovr",
        "-r", os.path.join(PROJECT_ROOT, "src"),
        "--xml",
        "-o", os.path.join(COVERAGE_BUILD_DIR, "coverage.xml"),
        "--exclude", ".*start.s.*",
        "--exclude", ".*unit_test.*",
    ], cwd=COVERAGE_BUILD_DIR)

    # Step 5: Copy reports to docs directory
    print("\n[5/5] Copying reports to docs directory...")
    if os.path.exists(COVERAGE_OUTPUT_DIR):
        shutil.rmtree(COVERAGE_OUTPUT_DIR)
    os.makedirs(COVERAGE_OUTPUT_DIR, exist_ok=True)

    # Copy HTML report (gcovr generates coverage.html + coverage.*.html files)
    for item in os.listdir(COVERAGE_BUILD_DIR):
        if item.startswith("coverage"):
            src = os.path.join(COVERAGE_BUILD_DIR, item)
            dst = os.path.join(COVERAGE_OUTPUT_DIR, item)
            if os.path.isfile(src):
                shutil.copy2(src, dst)
                print(f"  Copied: {item}")

    # Copy XML report
    xml_src = os.path.join(COVERAGE_BUILD_DIR, "coverage.xml")
    xml_dst = os.path.join(COVERAGE_OUTPUT_DIR, "coverage.xml")
    if os.path.exists(xml_src):
        shutil.copy2(xml_src, xml_dst)
        print(f"  Copied: coverage.xml")

    # Generate test summary markdown snippet
    passed = test_output.count("[PASS]")
    failed = test_output.count("[FAIL]")
    total = passed + failed
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
    pass_rate = f"{passed/total*100:.1f}%" if total > 0 else "N/A"

    summary_md = (
        f"| Metric | Value |\n"
        f"|--------|-------|\n"
        f"| **Total Tests** | {total} |\n"
        f"| **Passed** | {passed} |\n"
        f"| **Failed** | {failed} |\n"
        f"| **Pass Rate** | {pass_rate} |\n"
        f"| **Generated** | {timestamp} |\n"
    )

    summary_path = os.path.join(COVERAGE_OUTPUT_DIR, "test_summary.md")
    with open(summary_path, "w", encoding="utf-8") as f:
        f.write(summary_md)
    print(f"  Generated: test_summary.md")

    # Inject test summary into coverage_report.md
    report_md_path = os.path.join(PROJECT_ROOT, "docs", "04_software_unit_verification", "coverage_report.md")
    if os.path.exists(report_md_path):
        with open(report_md_path, "r", encoding="utf-8") as f:
            report_content = f.read()

        # Replace placeholder section with actual results
        placeholder = "<!-- TEST_SUMMARY_PLACEHOLDER -->"
        if placeholder in report_content:
            # Find the placeholder and replace everything from it to the next "> Run" hint
            import re
            pattern = re.compile(
                re.escape(placeholder) + r".*?(?=\n> \*\*|$)",
                re.DOTALL
            )
            replacement = placeholder + "\n" + summary_md
            report_content = pattern.sub(replacement, report_content, count=1)

            with open(report_md_path, "w", encoding="utf-8") as f:
                f.write(report_content)
            print(f"  Updated: coverage_report.md with test results")

    print("\n" + "=" * 60)
    print(f"  Coverage report generated successfully!")
    print(f"  HTML: {os.path.join(COVERAGE_OUTPUT_DIR, 'coverage.html')}")
    print(f"  XML:  {os.path.join(COVERAGE_OUTPUT_DIR, 'coverage.xml')}")
    print("=" * 60)


if __name__ == "__main__":
    main()