"""Check that relative links in project Markdown resolve to existing files."""

from __future__ import annotations

import re
import sys
from pathlib import Path
from urllib.parse import unquote


ROOT = Path(__file__).resolve().parents[2]
SKIP = {".git", ".vs", ".local_knowledge", "Debug", "Release", "x64", "output", "Dependencies"}
LINK = re.compile(r"!?\[[^\]]*\]\(([^)]+)\)")
SCHEME = re.compile(r"^[A-Za-z][A-Za-z0-9+.-]*:")


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    checked = 0
    missing = []
    for document in ROOT.rglob("*.md"):
        if SKIP.intersection(document.relative_to(ROOT).parts):
            continue
        try:
            content = document.read_text(encoding="utf-8-sig")
        except UnicodeError:
            continue
        for match in LINK.finditer(content):
            href = match.group(1).strip("<>").split("#", 1)[0].split("?", 1)[0]
            if not href or SCHEME.match(href):
                continue
            checked += 1
            if not (document.parent / unquote(href)).exists():
                line = content.count("\n", 0, match.start()) + 1
                missing.append(f"{document.relative_to(ROOT).as_posix()}:{line}: {href}")
    for entry in missing:
        print(entry)
    print(f"checked_links={checked} missing={len(missing)}")
    return 1 if missing else 0


if __name__ == "__main__":
    sys.exit(main())
