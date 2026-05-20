import lizard
import sys
import os

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUTPUT_PATH = os.path.join(PROJECT_ROOT, "docs", "05_software_integration_test", "lizard_report.md")

# 1. Analyze the project directory
file_infos = lizard.analyze(["."])

# 2. Collect all functions from all analyzed files
all_functions = []
for file_info in file_infos:
    for func in file_info.function_list:
        all_functions.append((file_info.filename, func))

# 3. Filter functions that exceed the configured thresholds
warnings = []
for filename, func in all_functions:
    # Thresholds: Complexity > 10, NLOC > 50, Parameters > 7
    if func.cyclomatic_complexity > 10 or func.nloc > 50 or len(func.parameters) > 7:
        warnings.append((filename, func))

# 4. Generate the Markdown report
with open(OUTPUT_PATH, "w", encoding="utf-8") as mdf:
    mdf.write("# Lizard Code Quality Warning Report\n\n")
    mdf.write("> Thresholds: Cyclomatic Complexity > 10 | NLOC > 50 | Parameters > 7\n\n")
    mdf.write(f"**Total Violations Found:** {len(warnings)}\n\n")

    mdf.write("| NLOC | CCN | Param | Location |\n")
    mdf.write("|--- |--- |--- |--- |\n")

    for filename, func in warnings:
        mdf.write(f"| {func.nloc} | {func.cyclomatic_complexity} | {len(func.parameters)} | {filename}:{func.start_line} @ `{func.name}` |\n")

print(f"Success! Report written to lizard_report.md ({len(warnings)} violations found).")