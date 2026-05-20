#!/usr/bin/env python3
"""
gen_code_review.py - Detect src/ changes and prepend a new review entry

Scans ``git diff`` for changes in the ``src/`` directory and prepends a
structured code-review entry to ``docs/code_review_record/code_review.md``.
The actual review content (issues found, refactoring suggestions) is filled
in by the smart-code-reviewer AI skill and committed separately.

Usage:
    python3 tools/gen_code_review.py          # detect changes, create entry
    python3 tools/gen_code_review.py --fix    # mark latest entry as fixed
    python3 tools/gen_code_review.py --dry-run  # preview without writing
"""

import os
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


def get_changed_files_staged() -> list[str]:
    """Return list of src/ files staged for commit (pre-commit mode)."""
    try:
        result = subprocess.run(
            ["git", "-C", BASE_DIR, "diff", "--cached", "--name-only", "--", "src/"],
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
    staged: list[str] = get_changed_files_staged()
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
        combined: list[str] = sorted(set(staged + unstaged))
        return combined
    except Exception:
        return staged


def build_review_entry(files: list[str], review_date: str) -> str:
    """Build a new review entry block for the given files."""
    today: str = review_date or date.today().isoformat()

    file_rows: str = "\n".join(
        f"| `{f}` | pending |" for f in files
    )

    entry: str = f"""## [{today}] Review — {', '.join(files)}

**Review Date**: {today}
**Fix Date**: (pending)

### Changed Files

| File | Status |
|------|--------|
{file_rows}

### Review Results

*(pending — run smart-code-reviewer AI skill to populate)*

### Fix Summary

*(pending)*

---
"""
    return entry


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


def prepend_review_entry(files: list[str], review_date: str = "") -> None:
    """Insert a new review entry at the top of the record file."""
    content: str = read_record_file()
    new_entry: str = build_review_entry(files, review_date)

    if REVIEW_MARKER_START in content:
        start_idx: int = content.index(REVIEW_MARKER_START)
        prefix: str = content[: start_idx + len(REVIEW_MARKER_START)]
        suffix: str = content[start_idx + len(REVIEW_MARKER_START):]
        write_record_file(prefix + "\n\n" + new_entry.strip() + "\n" + suffix)
    else:
        write_record_file(new_entry)


def mark_latest_as_fixed(fix_date: str = "") -> None:
    """Update the most recent review entry's Fix Date field to today."""
    content: str = read_record_file()
    if not content:
        print("No review record file found.", file=sys.stderr)
        sys.exit(1)

    today_str: str = fix_date or date.today().isoformat()

    lines: list[str] = content.split("\n")
    replaced: bool = False

    for i, line in enumerate(lines):
        if line.startswith("**Fix Date**: (pending)") and not replaced:
            lines[i] = f"**Fix Date**: {today_str}"
            replaced = True
            break

    if not replaced:
        print("No pending fix-date entry found (already fixed or missing).", file=sys.stderr)
        sys.exit(1)

    write_record_file("\n".join(lines))
    print(f"Marked latest review as fixed on {today_str}.")


def main() -> None:
    """Entry point: detect changes, optionally apply --fix or --dry-run."""
    args: list[str] = sys.argv[1:]

    if "--fix" in args:
        fix_date: str = ""
        for a in args:
            if a.startswith("--date="):
                fix_date = a[len("--date="):]
        mark_latest_as_fixed(fix_date)
        return

    files = get_changed_files_all()
    if not files:
        # Fall back to HEAD~1 diff (CI pipeline context)
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