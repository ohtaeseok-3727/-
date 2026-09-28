"""Small SQLite text index restricted to this game project."""

from __future__ import annotations

import argparse
import os
import sqlite3
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
DB_PATH = ROOT / ".local_knowledge" / "search" / "index.sqlite3"
MAX_BYTES = 2 * 1024 * 1024
EXTENSIONS = {
    ".c", ".cc", ".cmake", ".cpp", ".cxx", ".h", ".hpp", ".html",
    ".ini", ".js", ".json", ".lua", ".md", ".py", ".rst", ".sln",
    ".txt", ".vcproj", ".vcxproj", ".xml", ".yaml", ".yml",
}
SKIP_DIRS = {
    ".git", ".idea", ".local_knowledge", ".venv", ".vs", "__pycache__",
    "debug", "dependencies", "node_modules", "output", "release", "venv", "x64",
}
SKIP_NAMES = {".env", "id_rsa", "id_ed25519"}


def connect() -> sqlite3.Connection:
    DB_PATH.parent.mkdir(exist_ok=True)
    connection = sqlite3.connect(DB_PATH)
    connection.execute(
        """CREATE TABLE IF NOT EXISTS documents (
            path TEXT PRIMARY KEY,
            mtime_ns INTEGER NOT NULL,
            size INTEGER NOT NULL,
            content TEXT NOT NULL
        )"""
    )
    return connection


def source_files():
    for current, directories, filenames in os.walk(ROOT):
        directories[:] = sorted(
            name for name in directories
            if name.lower() not in SKIP_DIRS
            and not (Path(current) / name).is_symlink()
        )
        for name in sorted(filenames):
            path = Path(current) / name
            if (
                path.suffix.lower() not in EXTENSIONS
                or name.lower() in SKIP_NAMES
                or name.lower().startswith(".env.")
                or path.is_symlink()
            ):
                continue
            try:
                metadata = path.stat()
            except OSError:
                continue
            if metadata.st_size <= MAX_BYTES:
                yield path, metadata


def read_text(path: Path) -> str | None:
    try:
        data = path.read_bytes()
    except OSError:
        return None
    if b"\x00" in data:
        return None
    for encoding in ("utf-8-sig", "cp949"):
        try:
            return data.decode(encoding)
        except UnicodeDecodeError:
            pass
    return None


def index(connection: sqlite3.Connection) -> None:
    known = {
        path: (mtime_ns, size)
        for path, mtime_ns, size in connection.execute(
            "SELECT path, mtime_ns, size FROM documents"
        )
    }
    seen = set()
    updated = 0
    skipped = 0
    for path, metadata in source_files():
        relative = path.relative_to(ROOT).as_posix()
        seen.add(relative)
        signature = (metadata.st_mtime_ns, metadata.st_size)
        if known.get(relative) == signature:
            continue
        content = read_text(path)
        if content is None:
            skipped += 1
            connection.execute("DELETE FROM documents WHERE path = ?", (relative,))
            continue
        connection.execute(
            "INSERT OR REPLACE INTO documents(path, mtime_ns, size, content) VALUES (?, ?, ?, ?)",
            (relative, *signature, content),
        )
        updated += 1
    removed = len(known.keys() - seen)
    connection.executemany(
        "DELETE FROM documents WHERE path = ?",
        ((path,) for path in known.keys() - seen),
    )
    connection.commit()
    total = connection.execute("SELECT COUNT(*) FROM documents").fetchone()[0]
    print(f"indexed={total} updated={updated} removed={removed} skipped={skipped}")


def search(connection: sqlite3.Connection, query: str, limit: int) -> None:
    if not query.strip():
        raise ValueError("검색어를 입력하세요.")
    needle = query.casefold()
    found = 0
    for path, content in connection.execute("SELECT path, content FROM documents ORDER BY path"):
        folded = content.casefold()
        position = folded.find(needle)
        if position < 0:
            continue
        line_number = content.count("\n", 0, position) + 1
        line_start = content.rfind("\n", 0, position) + 1
        line_end = content.find("\n", position)
        if line_end < 0:
            line_end = len(content)
        excerpt = content[line_start:line_end].strip().replace("\t", " ")
        if len(excerpt) > 240:
            excerpt = excerpt[:237] + "..."
        print(f"{path}:{line_number}: {excerpt}")
        found += 1
        if found >= limit:
            break
    if found == 0:
        print("일치하는 문서가 없습니다. 인덱스를 갱신하거나 rg로 원본을 확인하세요.")


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("index", help="새 파일과 변경된 파일을 반영")
    finder = commands.add_parser("search", help="문서 내용에서 문자열 검색")
    finder.add_argument("query")
    finder.add_argument("--limit", type=int, default=20)
    commands.add_parser("stats", help="인덱스 문서 수와 크기 확인")
    arguments = parser.parse_args()
    if arguments.command == "search" and arguments.limit < 1:
        parser.error("--limit은 1 이상이어야 합니다")
    with connect() as connection:
        if arguments.command == "index":
            index(connection)
        elif arguments.command == "search":
            search(connection, arguments.query, arguments.limit)
        else:
            count, size = connection.execute(
                "SELECT COUNT(*), COALESCE(SUM(size), 0) FROM documents"
            ).fetchone()
            print(f"documents={count} source_bytes={size} database={DB_PATH.relative_to(ROOT).as_posix()}")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, sqlite3.Error, ValueError) as error:
        print(f"오류: {error}", file=sys.stderr)
        sys.exit(1)
