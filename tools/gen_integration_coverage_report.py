#!/usr/bin/env python3
"""
Integration Test Coverage Report Generator

This script:
  1. Configures CMake with coverage flags (ENABLE_COVERAGE=ON)
  2. Builds the integrated_test executable
  3. Runs integrated_test and captures the test results
  4. Generates HTML coverage report (gcovr)
  5. Generates XML coverage report (Cobertura format)
  6. Copies reports to the docs directory for MkDocs publishing
"""

import subprocess
import os
import sys
import shutil
import time
import re

# Paths
PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD_DIR = os.path.join(PROJECT_ROOT, "build")
COVERAGE_BUILD_DIR = os.path.join(BUILD_DIR, "coverage_build")
COVERAGE_OUTPUT_DIR = os.path.join(PROJECT_ROOT, "docs", "05_software_integration_test", "coverage")


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
    print("  Integration Test Coverage Report Generator")
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

    # Step 2: Build integrated_test
    print("\n[2/5] Building integrated_test with coverage instrumentation...")
    run_cmd(["ninja", "integrated_test"], cwd=COVERAGE_BUILD_DIR)

    # Step 3: Run integrated_test
    print("\n[3/5] Running integration tests...")
    result = run_cmd(["./integrated_test"], cwd=COVERAGE_BUILD_DIR, check=False)
    test_output = result.stdout
    print(f"  Test exit code: {result.returncode}")

    # Step 4: Generate coverage reports with gcovr
    print("\n[4/5] Generating coverage reports...")

    # Ensure gcovr is available
    if not shutil.which("gcovr"):
        print("  [INFO] gcovr not found, installing via pip...")
        run_cmd([sys.executable, "-m", "pip", "install", "gcovr"])

    # Verify .gcda files exist after test run
    gcda_count = 0
    for root, dirs, files in os.walk(COVERAGE_BUILD_DIR):
        for f in files:
            if f.endswith(".gcda"):
                gcda_count += 1
                print(f"  [DEBUG] Found .gcda: {os.path.join(root, f)}")
    print(f"  [DEBUG] Total .gcda files: {gcda_count}")

    # Generate HTML report
    print("  Generating HTML coverage report...")
    run_cmd([
        "gcovr",
        "--root", PROJECT_ROOT,
        "--filter", "src/",
        "--html", "--html-details",
        "-o", os.path.join(COVERAGE_BUILD_DIR, "coverage.html"),
        "--exclude", ".*start.s.*",
        COVERAGE_BUILD_DIR,
    ], cwd=PROJECT_ROOT)

    # Generate Cobertura XML report
    print("  Generating XML coverage report (Cobertura)...")
    run_cmd([
        "gcovr",
        "--root", PROJECT_ROOT,
        "--filter", "src/",
        "--xml",
        "-o", os.path.join(COVERAGE_BUILD_DIR, "coverage.xml"),
        "--exclude", ".*start.s.*",
        COVERAGE_BUILD_DIR,
    ], cwd=PROJECT_ROOT)

    # Step 5: Copy reports to docs directory
    print("\n[5/5] Copying reports to docs directory...")
    if os.path.exists(COVERAGE_OUTPUT_DIR):
        shutil.rmtree(COVERAGE_OUTPUT_DIR)
    os.makedirs(COVERAGE_OUTPUT_DIR, exist_ok=True)

    # Copy HTML report
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

    # Generate test summary
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

    # Inject test summary into coverage_report.md
    report_md_path = os.path.join(PROJECT_ROOT, "docs", "05_software_integration_test", "coverage_report.md")
    if os.path.exists(report_md_path):
        with open(report_md_path, "r", encoding="utf-8") as f:
            report_content = f.read()

        placeholder = "<!-- TEST_SUMMARY_PLACEHOLDER -->"
        if placeholder in report_content:
            pattern = re.compile(
                re.escape(placeholder) + r".*?(?=\n> Run `python)",
                re.DOTALL
            )
            replacement = placeholder + "\n" + summary_md + "\n"
            report_content = pattern.sub(replacement, report_content, count=1)

            with open(report_md_path, "w", encoding="utf-8") as f:
                f.write(report_content)
            print(f"  Updated: coverage_report.md with test results")

    print("\n" + "=" * 60)
    print(f"  Integration coverage report generated successfully!")
    print(f"  HTML: {os.path.join(COVERAGE_OUTPUT_DIR, 'coverage.html')}")
    print(f"  XML:  {os.path.join(COVERAGE_OUTPUT_DIR, 'coverage.xml')}")
    print("=" * 60)


if __name__ == "__main__":
    main()