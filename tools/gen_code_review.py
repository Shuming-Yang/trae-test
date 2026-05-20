#!/usr/bin/env python3
"""
gen_code_review.py - Detect src/ changes and manage per-issue review records

Scans ``git diff`` for changes in the ``src/`` directory and maintains
``docs/code_review_record/code_review.md`` with per-issue granularity.
Each issue tracks its own status: ``pending``, ``fixed (YYYY-MM-DD)``, or ``ignored``.

When CI detects ``src/`` changes and a previous review still has pending issues,
it auto-marks issues whose referenced files appear in the diff as fixed.

Usage:
    python3 tools/gen_code_review.py                  # detect changes, create entry
    python3 tools/gen_code_review.py --dry-run        # preview without writing
    python3 tools/gen_code_review.py --fix-issue 3    # mark issue #3 as fixed (today)
    python3 tools/gen_code_review.py --fix-issue 3 --date=2026-05-22
    python3 tools/gen_code_review.py --ignore-issue 5 # mark issue #5 as ignored
    python3 tools/gen_code_review.py --scan-fixes     # scan diff against pending issues
"""

import os
import re
import subprocess
import sys
from datetime import date, datetime
from typing import Optional

# ==========================================
# Path Configuration
# ==========================================
BASE_DIR: str = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SRC_DIR: str = os.path.join(BASE_DIR, "src")
RECORD_FILE: str = os.path.join(BASE_DIR, "docs", "code_review_record", "code_review.md")

REVIEW_MARKER_START: str = "<!-- REVIEW_ENTRIES_START -->"
REVIEW_MARKER_END: str = "<!-- REVIEW_ENTRIES_END -->"


# ==========================================
# Git helpers
# ==========================================

def get_changed_files() -> list[str]:
    """Return list of src/ files changed since the previous HEAD~1 commit."""
    try:
        result = subprocess.run(
            ["git", "-C", BASE_DIR, "diff", "--name-only", "HEAD~1", "HEAD", "--", "src/"],
            capture_output=True,
            text=True,
            check=False,
        )
        files: list[str] = [
            f.strip() for f in result.stdout.strip().split("\n") if f.strip()
        ]
        return files
    except Exception:
        return []


def get_changed_files_all() -> list[str]:
    """Return list of all changed files in src/ (staged + unstaged)."""
    staged: list[str] = []
    try:
        result = subprocess.run(
            ["git", "-C", BASE_DIR, "diff", "--cached", "--name-only", "--", "src/"],
            capture_output=True,
            text=True,
            check=False,
        )
        staged = [f.strip() for f in result.stdout.strip().split("\n") if f.strip()]
    except Exception:
        pass

    try:
        result = subprocess.run(
            ["git", "-C", BASE_DIR, "diff", "--name-only", "--", "src/"],
            capture_output=True,
            text=True,
            check=False,
        )
        unstaged: list[str] = [
            f.strip() for f in result.stdout.strip().split("\n") if f.strip()
        ]
        return sorted(set(staged + unstaged))
    except Exception:
        return staged


# ==========================================
# File I/O
# ==========================================

def read_record_file() -> str:
    """Read the current review record file content."""
    if os.path.isfile(RECORD_FILE):
        with open(RECORD_FILE, "r", encoding="utf-8") as fh:
            return fh.read()
    return ""


def write_record_file(content: str) -> None:
    """Write updated content to the review record file."""
    os.makedirs(os.path.dirname(RECORD_FILE), exist_ok=True)
    with open(RECORD_FILE, "w", encoding="utf-8") as fh:
        fh.write(content)


# ==========================================
# Issue table parser
# ==========================================

def find_latest_review_start(content: str) -> int:
    """Return the character index where the first review entry begins
    (immediately after REVIEW_MARKER_START).  Returns -1 if not found."""
    idx: int = content.find(REVIEW_MARKER_START)
    if idx == -1:
        return -1
    return idx + len(REVIEW_MARKER_START)


def find_next_review_boundary(content: str, start: int) -> int:
    """Find the start of the next `## [` review section or REVIEW_MARKER_END."""
    next_review: int = re.search(r"^##\s+\[", content[start:], re.MULTILINE)
    next_marker: int = content.find(REVIEW_MARKER_END, start)

    candidates: list[int] = []
    if next_review is not None:
        candidates.append(start + next_review.start())
    if next_marker != -1:
        candidates.append(next_marker)

    return min(candidates) if candidates else len(content)


def parse_issue_table(latest_section: str) -> list[dict]:
    """Parse the `### Issues` markdown table in a review section.

    Returns a list of issue dicts with keys:
        issue_num, severity, file, lines, category, description, status, fix_date
    """
    issues: list[dict] = []

    # Locate the issues table
    table_start: int = latest_section.find("### Issues")
    if table_start == -1:
        return issues

    # Find table rows (skip header and separator)
    remaining: str = latest_section[table_start:]
    in_table: bool = False
    header_skipped: bool = False

    for raw_line in remaining.split("\n"):
        line: str = raw_line.strip()

        if line.startswith("### Issues"):
            in_table = True
            continue
        if not in_table:
            continue
        if line.startswith("|") and "---" in line and not header_skipped:
            header_skipped = True
            continue
        if not line.startswith("|"):
            # Table ended
            break

        # Parse a data row: | # | Severity | File | Line(s) | Category | Issue | Status | Fix Date |
        if not header_skipped:
            continue

        cells: list[str] = [c.strip() for c in line.split("|")]
        # cells[0] is empty (leading |), then cells[1..N]
        if len(cells) < 9:
            continue

        try:
            issue_num: int = int(cells[1])
        except (ValueError, IndexError):
            continue

        issues.append({
            "issue_num": issue_num,
            "severity": cells[2],
            "file": cells[3],
            "lines": cells[4],
            "category": cells[5],
            "description": cells[6],
            "status": cells[7],
            "fix_date": cells[8] if len(cells) > 8 else "—",
        })

    return issues


def get_pending_issues(content: str) -> list[dict]:
    """Return all pending issues from the latest review entry."""
    start: int = find_latest_review_start(content)
    if start == -1:
        return []

    end: int = find_next_review_boundary(content, start)
    latest_section: str = content[start:end]

    all_issues: list[dict] = parse_issue_table(latest_section)
    return [i for i in all_issues if i["status"] == "pending"]


# ==========================================
# Auto-fix detection
# ==========================================

def auto_fix_pending_issues(content: str, changed_files: list[str], fix_date: str = "") -> tuple[str, int]:
    """Scan pending issues against changed files; mark matches as fixed.

    Returns (updated_content, count_of_issues_fixed).
    """
    if not changed_files:
        return content, 0

    today_str: str = fix_date or date.today().isoformat()
    pending: list[dict] = get_pending_issues(content)
    if not pending:
        return content, 0

    lines: list[str] = content.split("\n")
    fixed_count: int = 0

    for issue in pending:
        issue_file: str = issue["file"]
        # Normalize path: strip leading ./ if present
        issue_file_normalized: str = issue_file.lstrip("./")

        matched: bool = False
        for cf in changed_files:
            # git diff returns paths like "src/main.c"
            # table stores paths like "src/main.c" or "main.c"
            if cf == issue_file_normalized or cf.endswith("/" + issue_file_normalized) or issue_file_normalized.endswith("/" + cf.rsplit("/", 1)[-1] if "/" in cf else cf):
                matched = True
                break

        if not matched:
            continue

        # Find the table row for this issue and update status + fix_date
        target_num: int = issue["issue_num"]
        for i, line in enumerate(lines):
            stripped: str = line.strip()
            if not stripped.startswith("|"):
                continue
            cells: list[str] = [c.strip() for c in stripped.split("|")]
            if len(cells) < 2:
                continue
            try:
                row_num: int = int(cells[1])
            except (ValueError, IndexError):
                continue
            if row_num != target_num:
                continue

            # Replace status cell (index 7) and fix_date cell (index 8)
            # Reconstruct the row (skip trailing empty element from split)
            cells_data = cells[1:]
            if cells_data and cells_data[-1] == "":
                cells_data = cells_data[:-1]
            if len(cells_data) >= 8:
                cells_data[6] = "**fixed**"
                cells_data[7] = today_str

                # Rebuild the line preserving leading whitespace
                leading: str = line[: len(line) - len(line.lstrip())]
                new_line: str = leading + "| " + " | ".join(cells_data) + " |"
                lines[i] = new_line
                fixed_count += 1
            break

    if fixed_count > 0:
        content = "\n".join(lines)
        print(f"Auto-marked {fixed_count} issue(s) as fixed on {today_str}.")

    return content, fixed_count


# ==========================================
# Issue status management (manual)
# ==========================================

def update_issue_status(issue_num: int, new_status: str, status_date: str = "") -> None:
    """Manually update a specific issue's status (e.g. --fix-issue, --ignore-issue)."""
    content: str = read_record_file()
    if not content:
        print("No review record file found.", file=sys.stderr)
        sys.exit(1)

    today_str: str = status_date or date.today().isoformat()
    lines: list[str] = content.split("\n")
    found: bool = False

    for i, line in enumerate(lines):
        stripped: str = line.strip()
        if not stripped.startswith("|"):
            continue
        cells: list[str] = [c.strip() for c in stripped.split("|")]
        if len(cells) < 2:
            continue
        try:
            row_num: int = int(cells[1])
        except (ValueError, IndexError):
            continue
        if row_num != issue_num:
            continue

        if len(cells) >= 9:
            cells_data = cells[1:]
            if cells_data and cells_data[-1] == "":
                cells_data = cells_data[:-1]
            if len(cells_data) >= 8:
                cells_data[6] = new_status
                cells_data[7] = today_str if new_status == "**fixed**" else "—"

                leading: str = line[: len(line) - len(line.lstrip())]
                new_line: str = leading + "| " + " | ".join(cells_data) + " |"
                lines[i] = new_line
            found = True
        break

    if not found:
        print(f"Issue #{issue_num} not found in the latest review.", file=sys.stderr)
        sys.exit(1)

    write_record_file("\n".join(lines))
    label: str = "fixed" if "fixed" in new_status else "ignored"
    print(f"Issue #{issue_num} marked as {label}.")


# ==========================================
# Review entry builder
# ==========================================

def build_review_entry(files: list[str], review_date: str = "") -> str:
    """Build a new review entry block for the given files."""
    today: str = review_date or date.today().isoformat()
    file_list: str = ", ".join(files)

    entry: str = f"""## [{today}] Review — {file_list}

**Review Date**: {today}

### Issues

*(pending — run smart-code-reviewer AI skill to populate)*

---
"""
    return entry


def prepend_review_entry(files: list[str], review_date: str = "") -> None:
    """Insert a new review entry at the top of the record file.

    CI 端僅負責偵測變動並建立 review entry 模板。
    Auto-fix 由 AI 在 smart-code-reviewer 流程中處理（Step 0 回溯比對）。
    """
    content: str = read_record_file()
    new_entry: str = build_review_entry(files, review_date)

    marker_start: str = REVIEW_MARKER_START
    if marker_start in content:
        start_idx: int = content.index(marker_start)
        prefix: str = content[: start_idx + len(marker_start)]
        suffix: str = content[start_idx + len(marker_start):]
        write_record_file(prefix + "\n\n" + new_entry.strip() + "\n" + suffix)
    else:
        write_record_file(new_entry)


# ==========================================
# Main entry point
# ==========================================

def main() -> None:
    """Entry point: detect changes, manage issue status, or preview."""
    args: list[str] = sys.argv[1:]

    # --fix-issue N [--date=YYYY-MM-DD]
    fix_match: Optional[str] = None
    ignore_match: Optional[str] = None
    for a in args:
        if a.startswith("--fix-issue="):
            fix_match = a[len("--fix-issue="):]
        elif a.startswith("--ignore-issue="):
            ignore_match = a[len("--ignore-issue="):]

    if "--fix-issue" in (a.split("=")[0] for a in args if "=" in a):
        # Already handled via --fix-issue= pattern above
        pass

    # Parse --fix-issue and --ignore-issue with optional value
    for i, a in enumerate(args):
        if a == "--fix-issue" and i + 1 < len(args) and not args[i + 1].startswith("--"):
            fix_match = args[i + 1]
        elif a == "--ignore-issue" and i + 1 < len(args) and not args[i + 1].startswith("--"):
            ignore_match = args[i + 1]

    if fix_match is not None:
        status_date: str = ""
        for a in args:
            if a.startswith("--date="):
                status_date = a[len("--date="):]
        update_issue_status(int(fix_match), "**fixed**", status_date)
        return

    if ignore_match is not None:
        update_issue_status(int(ignore_match), "❌ ignored", "")
        return

    # --scan-fixes: only scan without creating a new entry
    if "--scan-fixes" in args:
        content: str = read_record_file()
        changed = get_changed_files_all()
        if not changed:
            changed = get_changed_files()
        if not changed:
            print("No src/ changes detected. Nothing to scan.")
            return
        scan_date: str = ""
        for a in args:
            if a.startswith("--date="):
                scan_date = a[len("--date="):]
        updated, count = auto_fix_pending_issues(content, changed, scan_date)
        if count > 0:
            write_record_file(updated)
        else:
            print("No pending issues matched the changed files.")
        return

    # Default: detect changes and create new review entry
    files = get_changed_files_all()
    if not files:
        files = get_changed_files()

    if not files:
        print("No changes detected in src/ directory. Nothing to do.")
        return

    review_date: str = ""
    for a in args:
        if a.startswith("--date="):
            review_date = a[len("--date="):]

    if "--dry-run" in args:
        print(f"[DRY RUN] Would create review entry for: {', '.join(files)}")
        entry_preview: str = build_review_entry(files, review_date)
        print(entry_preview)
        return

    prepend_review_entry(files, review_date)
    print(f"Created review entry for: {', '.join(files)}")


if __name__ == "__main__":
    main()