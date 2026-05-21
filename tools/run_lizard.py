import lizard
import sys
import os

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUTPUT_PATH = os.path.join(PROJECT_ROOT, "docs", "05_software_integration_test", "lizard_report.md")

THRESHOLD_CCN = 10
THRESHOLD_NLOC = 50
THRESHOLD_PARAM = 7

# 1. Analyze only src/ and inc/ directories
file_infos = lizard.analyze(["src", "inc"])

# 2. Collect all functions from all analyzed files
all_functions = []
for file_info in file_infos:
    for func in file_info.function_list:
        all_functions.append((file_info.filename, func))

# 3. Find maximum values and their locations
max_ccn = 0
max_nloc = 0
max_param = 0
max_ccn_loc = "—"
max_nloc_loc = "—"
max_param_loc = "—"

for filename, func in all_functions:
    param_count = len(func.parameters)
    location = f"{filename}:{func.start_line} @ `{func.name}`"

    if func.cyclomatic_complexity > max_ccn:
        max_ccn = func.cyclomatic_complexity
        max_ccn_loc = location
    if func.nloc > max_nloc:
        max_nloc = func.nloc
        max_nloc_loc = location
    if param_count > max_param:
        max_param = param_count
        max_param_loc = location

# 4. Filter functions that exceed the configured thresholds
violations = []
for filename, func in all_functions:
    param_count = len(func.parameters)
    if func.cyclomatic_complexity > THRESHOLD_CCN or func.nloc > THRESHOLD_NLOC or param_count > THRESHOLD_PARAM:
        violations.append((filename, func, param_count))

# 5. Generate the Markdown report
with open(OUTPUT_PATH, "w", encoding="utf-8") as mdf:
    mdf.write("# Code Matrix Summary Report\n\n")
    mdf.write("> Thresholds: CCN > 10 | NLOC > 50 | Param > 7\n\n")

    mdf.write("## Summary\n\n")
    mdf.write("| Metric | Max Value | Threshold | Location |\n")
    mdf.write("|---|---|---|---|\n")
    mdf.write(f"| CCN | {max_ccn} | > {THRESHOLD_CCN} | {max_ccn_loc} |\n")
    mdf.write(f"| NLOC | {max_nloc} | > {THRESHOLD_NLOC} | {max_nloc_loc} |\n")
    mdf.write(f"| Param | {max_param} | > {THRESHOLD_PARAM} | {max_param_loc} |\n")

    mdf.write(f"\n---\n\n")
    mdf.write("## Violations\n\n")
    mdf.write(f"**Total Violations Found:** {len(violations)}\n\n")

    mdf.write("| NLOC | CCN | Param | Location |\n")
    mdf.write("|--- |--- |--- |--- |\n")

    for filename, func, param_count in violations:
        mdf.write(f"| {func.nloc} | {func.cyclomatic_complexity} | {param_count} | {filename}:{func.start_line} @ `{func.name}` |\n")

    mdf.write("\n---\n\n")
    mdf.write("## Abbreviation Reference\n\n")
    mdf.write("| Abbreviation | Full Name | Description |\n")
    mdf.write("|---|---|---|\n")
    mdf.write("| **NLOC** | Net Lines of Code | Number of executable lines (excludes blanks and comments) |\n")
    mdf.write("| **CCN** | Cyclomatic Complexity Number | McCabe complexity metric; higher values indicate more branching paths |\n")
    mdf.write("| **Param** | Parameters Count | Number of formal parameters in the function signature |\n")

print(f"Success! Report written to lizard_report.md (max CCN={max_ccn}, max NLOC={max_nloc}, max Param={max_param}, {len(violations)} violations).")