"""Track work and index function declarations within this game project."""

from __future__ import annotations

import argparse
import ast
import contextlib
import json
import os
import re
import sqlite3
import sys
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CATALOG = ROOT / ".agents" / "projects" / "catalog.json"
DATABASE = ROOT / ".local_knowledge" / "work" / "project_hub.sqlite3"
EXTRACTOR_VERSION = 2
SKIP_DIRS = {".git", ".idea", ".venv", "__pycache__", "debug", "release", "x64"}
CPP_FUNCTION = re.compile(
    r"^\s*(?P<prefix>.*?)\b(?P<name>~?[A-Za-z_]\w*)\s*"
    r"\((?P<args>[^()]*)\)\s*"
    r"(?:(?:const|noexcept|override|final)\s*)*(?:=\s*0\s*)?(?:[;{])?\s*(?://.*)?$"
)


def now() -> str:
    return datetime.now(timezone.utc).isoformat(timespec="seconds")


def catalog() -> dict[str, dict]:
    data = json.loads(CATALOG.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError("프로젝트 목록 형식이 올바르지 않습니다")
    for project_id, settings in data.items():
        path = (ROOT / settings["path"]).resolve()
        try:
            path.relative_to(ROOT.resolve())
        except ValueError as error:
            raise ValueError(f"프로젝트 경로가 작업 폴더 밖입니다: {project_id}") from error
        if not path.is_dir():
            raise ValueError(f"프로젝트 폴더가 없습니다: {project_id}")
    return data


def project_config(project_id: str, projects: dict[str, dict]) -> dict:
    if project_id not in projects:
        raise ValueError(f"알 수 없는 프로젝트 ID: {project_id}")
    return projects[project_id]


def connect() -> sqlite3.Connection:
    DATABASE.parent.mkdir(exist_ok=True)
    db = sqlite3.connect(DATABASE)
    db.execute("PRAGMA foreign_keys = ON")
    db.executescript(
        """
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY,
            project TEXT NOT NULL,
            title TEXT NOT NULL,
            status TEXT NOT NULL CHECK(status IN ('in_progress', 'done', 'blocked', 'cancelled')),
            started_at TEXT NOT NULL,
            finished_at TEXT,
            summary TEXT
        );
        CREATE TABLE IF NOT EXISTS work_items (
            id INTEGER PRIMARY KEY,
            task_id INTEGER NOT NULL REFERENCES tasks(id),
            title TEXT NOT NULL,
            status TEXT NOT NULL CHECK(status IN ('planned', 'in_progress', 'done', 'blocked', 'skipped')),
            started_at TEXT,
            finished_at TEXT,
            note TEXT
        );
        CREATE INDEX IF NOT EXISTS work_items_task_id ON work_items(task_id);
        CREATE TABLE IF NOT EXISTS function_files (
            project TEXT NOT NULL,
            path TEXT NOT NULL,
            mtime_ns INTEGER NOT NULL,
            size INTEGER NOT NULL,
            PRIMARY KEY (project, path)
        );
        CREATE TABLE IF NOT EXISTS functions (
            project TEXT NOT NULL,
            path TEXT NOT NULL,
            line INTEGER NOT NULL,
            name TEXT NOT NULL,
            kind TEXT NOT NULL,
            signature TEXT NOT NULL,
            description TEXT NOT NULL,
            PRIMARY KEY (project, path, line, name)
        );
        CREATE INDEX IF NOT EXISTS functions_project_name ON functions(project, name);
        CREATE TABLE IF NOT EXISTS function_index_versions (
            project TEXT PRIMARY KEY,
            version INTEGER NOT NULL
        );
        """
    )
    return db


def nonempty(value: str, label: str) -> str:
    value = value.strip()
    if not value:
        raise ValueError(f"{label}을(를) 입력하세요")
    return value


def task(db: sqlite3.Connection, task_id: int) -> tuple:
    row = db.execute(
        "SELECT id, project, title, status, started_at, finished_at, summary FROM tasks WHERE id = ?",
        (task_id,),
    ).fetchone()
    if row is None:
        raise ValueError(f"작업 ID가 없습니다: {task_id}")
    return row


def item(db: sqlite3.Connection, task_id: int, item_id: int) -> tuple:
    row = db.execute(
        "SELECT id, title, status FROM work_items WHERE id = ? AND task_id = ?",
        (item_id, task_id),
    ).fetchone()
    if row is None:
        raise ValueError(f"실행 단위 ID가 작업에 없습니다: {item_id}")
    return row


def start_task(db: sqlite3.Connection, args: argparse.Namespace, projects: dict) -> None:
    project_config(args.project, projects)
    title = nonempty(args.title, "작업 제목")
    items = [nonempty(value, "실행 단위") for value in args.item]
    with db:
        cursor = db.execute(
            "INSERT INTO tasks(project, title, status, started_at) VALUES (?, ?, 'in_progress', ?)",
            (args.project, title, now()),
        )
        task_id = cursor.lastrowid
        for value in items:
            db.execute(
                "INSERT INTO work_items(task_id, title, status) VALUES (?, ?, 'planned')",
                (task_id, value),
            )
    print(f"task={task_id} project={args.project} status=in_progress items={len(items)}")
    show_task(db, task_id)


def add_item(db: sqlite3.Connection, args: argparse.Namespace) -> None:
    parent = task(db, args.task_id)
    if parent[3] != "in_progress":
        raise ValueError("종료된 작업에는 실행 단위를 추가할 수 없습니다")
    with db:
        cursor = db.execute(
            "INSERT INTO work_items(task_id, title, status) VALUES (?, ?, 'planned')",
            (args.task_id, nonempty(args.title, "실행 단위")),
        )
    print(f"item={cursor.lastrowid} task={args.task_id} status=planned")


def begin_item(db: sqlite3.Connection, args: argparse.Namespace) -> None:
    if task(db, args.task_id)[3] != "in_progress":
        raise ValueError("종료된 작업의 실행 단위를 시작할 수 없습니다")
    current = item(db, args.task_id, args.item_id)
    if current[2] != "planned":
        raise ValueError(f"시작할 수 없는 상태입니다: {current[2]}")
    with db:
        db.execute(
            "UPDATE work_items SET status = 'in_progress', started_at = ? WHERE id = ?",
            (now(), args.item_id),
        )
    print(f"item={args.item_id} status=in_progress")


def finish_item(db: sqlite3.Connection, args: argparse.Namespace) -> None:
    if task(db, args.task_id)[3] != "in_progress":
        raise ValueError("종료된 작업의 실행 단위를 변경할 수 없습니다")
    current = item(db, args.task_id, args.item_id)
    expected = "planned" if args.status == "skipped" else "in_progress"
    if current[2] != expected:
        raise ValueError(f"{args.status} 처리 전 상태는 {expected}여야 합니다")
    with db:
        db.execute(
            "UPDATE work_items SET status = ?, finished_at = ?, note = ? WHERE id = ?",
            (args.status, now(), args.note.strip(), args.item_id),
        )
    print(f"item={args.item_id} status={args.status}")


def finish_task(db: sqlite3.Connection, args: argparse.Namespace) -> None:
    current = task(db, args.task_id)
    if current[3] != "in_progress":
        raise ValueError(f"이미 종료된 작업입니다: {current[3]}")
    summary = nonempty(args.summary, "작업 결과")
    unfinished = db.execute(
        "SELECT COUNT(*) FROM work_items WHERE task_id = ? AND status NOT IN ('done', 'skipped')",
        (args.task_id,),
    ).fetchone()[0]
    if args.status == "done" and unfinished:
        raise ValueError(f"완료되지 않은 실행 단위 {unfinished}개가 있습니다")
    ended = now()
    with db:
        if args.status != "done":
            child_status = "blocked" if args.status == "blocked" else "skipped"
            db.execute(
                "UPDATE work_items SET status = ?, finished_at = ?, note = COALESCE(note, ?) "
                "WHERE task_id = ? AND status IN ('planned', 'in_progress')",
                (child_status, ended, summary, args.task_id),
            )
        db.execute(
            "UPDATE tasks SET status = ?, finished_at = ?, summary = ? WHERE id = ?",
            (args.status, ended, summary, args.task_id),
        )
    print(f"task={args.task_id} status={args.status}")


def show_task(db: sqlite3.Connection, task_id: int) -> None:
    record = task(db, task_id)
    print(f"[{record[0]}] {record[1]} | {record[2]} | {record[3]}")
    print(f"  started={record[4]} finished={record[5] or '-'}")
    if record[6]:
        print(f"  summary={record[6]}")
    for row in db.execute(
        "SELECT id, title, status, started_at, finished_at, note "
        "FROM work_items WHERE task_id = ? ORDER BY id",
        (task_id,),
    ):
        print(f"  ({row[0]}) {row[2]} | {row[1]}")
        if row[5]:
            print(f"      note={row[5]}")


def list_tasks(db: sqlite3.Connection, project: str | None, open_only: bool) -> None:
    query = "SELECT id, project, title, status, started_at, finished_at FROM tasks WHERE 1 = 1"
    parameters: list[str] = []
    if project:
        query += " AND project = ?"
        parameters.append(project)
    if open_only:
        query += " AND status = 'in_progress'"
    query += " ORDER BY id DESC LIMIT 50"
    for row in db.execute(query, parameters):
        print(f"[{row[0]}] {row[1]} | {row[3]} | {row[2]} | {row[4]} -> {row[5] or '-'}")


def text_source(path: Path) -> str | None:
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


def project_sources(settings: dict):
    base = (ROOT / settings["path"]).resolve()
    excluded = SKIP_DIRS | {name.lower() for name in settings.get("exclude_dirs", [])}
    suffixes = {".py"} if settings["languages"] == ["python"] else {".h", ".hpp"}
    for source_root in settings["source_roots"]:
        target = (base / source_root).resolve()
        try:
            target.relative_to(base)
        except ValueError as error:
            raise ValueError(f"소스 경로가 프로젝트 밖입니다: {source_root}") from error
        if not target.is_dir():
            continue
        for current, directories, filenames in os.walk(target):
            directories[:] = sorted(
                name for name in directories
                if name.lower() not in excluded and not (Path(current) / name).is_symlink()
            )
            for filename in sorted(filenames):
                path = Path(current) / filename
                if path.suffix.lower() in suffixes and not path.is_symlink():
                    try:
                        metadata = path.stat()
                    except OSError:
                        continue
                    if metadata.st_size <= 2 * 1024 * 1024:
                        yield path, metadata


def python_functions(content: str):
    tree = ast.parse(content)
    lines = content.splitlines()

    def visit(nodes, parents=()):
        for node in nodes:
            if isinstance(node, ast.ClassDef):
                yield from visit(node.body, (*parents, node.name))
            elif isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)):
                name = ".".join((*parents, node.name))
                signature = lines[node.lineno - 1].strip()[:300]
                description = (ast.get_docstring(node) or "").splitlines()
                yield (
                    node.lineno, name, "method" if parents else "function",
                    signature, description[0][:300] if description else "",
                )
                yield from visit(node.body, (*parents, node.name))

    yield from visit(tree.body)


def cpp_functions(content: str, stem: str):
    lines = content.splitlines()
    for number, line in enumerate(lines, start=1):
        stripped = line.strip()
        if not stripped or stripped.startswith(("//", "#", "*")):
            continue
        match = CPP_FUNCTION.match(line)
        if not match:
            continue
        prefix = match.group("prefix").strip()
        name = match.group("name")
        if not prefix and name.lstrip("~") != stem:
            continue
        if any(token in prefix for token in ("->", ".", "=", "<<", '"')):
            continue
        if prefix.split(" ", 1)[0] in {"if", "for", "while", "switch", "return", "delete", "new"}:
            continue
        if not stripped.endswith((";", "{")):
            following = next((part.strip() for part in lines[number:] if part.strip()), "")
            if not following.startswith("{"):
                continue
        previous = lines[number - 2].strip() if number > 1 else ""
        description = previous[2:].strip() if previous.startswith("//") else ""
        yield number, name, "declaration", stripped[:300], description[:300]


def index_functions(db: sqlite3.Connection, projects: dict, selected: str | None) -> None:
    chosen = [selected] if selected else list(projects)
    for project_id in chosen:
        settings = project_config(project_id, projects)
        version_row = db.execute(
            "SELECT version FROM function_index_versions WHERE project = ?", (project_id,)
        ).fetchone()
        refresh_all = version_row is None or version_row[0] != EXTRACTOR_VERSION
        known = {
            path: (mtime, size)
            for path, mtime, size in db.execute(
                "SELECT path, mtime_ns, size FROM function_files WHERE project = ?",
                (project_id,),
            )
        }
        seen = set()
        updated = 0
        skipped = 0
        for path, metadata in project_sources(settings):
            relative = path.relative_to(ROOT).as_posix()
            seen.add(relative)
            signature = (metadata.st_mtime_ns, metadata.st_size)
            if not refresh_all and known.get(relative) == signature:
                continue
            content = text_source(path)
            if content is None:
                skipped += 1
                continue
            try:
                symbols = list(
                    python_functions(content) if path.suffix.lower() == ".py"
                    else cpp_functions(content, path.stem)
                )
            except SyntaxError:
                skipped += 1
                continue
            with db:
                db.execute("DELETE FROM functions WHERE project = ? AND path = ?", (project_id, relative))
                db.executemany(
                    "INSERT INTO functions(project, path, line, name, kind, signature, description) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?)",
                    ((project_id, relative, *symbol) for symbol in symbols),
                )
                db.execute(
                    "INSERT OR REPLACE INTO function_files(project, path, mtime_ns, size) "
                    "VALUES (?, ?, ?, ?)",
                    (project_id, relative, *signature),
                )
            updated += 1
        removed = known.keys() - seen
        with db:
            for relative in removed:
                db.execute("DELETE FROM functions WHERE project = ? AND path = ?", (project_id, relative))
                db.execute("DELETE FROM function_files WHERE project = ? AND path = ?", (project_id, relative))
            db.execute(
                "INSERT OR REPLACE INTO function_index_versions(project, version) VALUES (?, ?)",
                (project_id, EXTRACTOR_VERSION),
            )
        count = db.execute(
            "SELECT COUNT(*) FROM functions WHERE project = ?", (project_id,)
        ).fetchone()[0]
        print(f"{project_id}: functions={count} updated_files={updated} removed_files={len(removed)} skipped_files={skipped}")


def find_function(db: sqlite3.Connection, project_id: str, term: str, limit: int) -> None:
    needle = nonempty(term, "함수 검색어").replace("\\", "\\\\").replace("%", "\\%").replace("_", "\\_")
    pattern = f"%{needle}%"
    rows = db.execute(
        "SELECT path, line, name, kind, signature, description FROM functions "
        "WHERE project = ? AND (name LIKE ? ESCAPE '\\' OR signature LIKE ? ESCAPE '\\') "
        "ORDER BY name, path, line LIMIT ?",
        (project_id, pattern, pattern, limit),
    )
    found = 0
    for path, line, name, kind, signature, description in rows:
        print(f"{path}:{line} {name} [{kind}] {signature}")
        if description:
            print(f"  {description}")
        found += 1
    if not found:
        print("일치하는 함수가 없습니다. index-functions를 실행하거나 원본에서 rg로 확인하세요.")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("projects")
    start = commands.add_parser("start")
    start.add_argument("project")
    start.add_argument("title")
    start.add_argument("--item", action="append", default=[])
    add = commands.add_parser("add-item")
    add.add_argument("task_id", type=int)
    add.add_argument("title")
    begin = commands.add_parser("begin")
    begin.add_argument("task_id", type=int)
    begin.add_argument("item_id", type=int)
    finish_item_parser = commands.add_parser("finish-item")
    finish_item_parser.add_argument("task_id", type=int)
    finish_item_parser.add_argument("item_id", type=int)
    finish_item_parser.add_argument("--status", choices=("done", "blocked", "skipped"), default="done")
    finish_item_parser.add_argument("--note", default="")
    finish = commands.add_parser("finish")
    finish.add_argument("task_id", type=int)
    finish.add_argument("--status", choices=("done", "blocked", "cancelled"), default="done")
    finish.add_argument("--summary", required=True)
    show = commands.add_parser("show")
    show.add_argument("task_id", type=int)
    for name in ("history", "open"):
        listing = commands.add_parser(name)
        listing.add_argument("--project")
    indexing = commands.add_parser("index-functions")
    indexing.add_argument("--project")
    finding = commands.add_parser("find-function")
    finding.add_argument("project")
    finding.add_argument("term")
    finding.add_argument("--limit", type=int, default=20)
    return parser


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    args = build_parser().parse_args()
    projects = catalog()
    if args.command == "projects":
        for project_id, settings in projects.items():
            print(f"{project_id}: {settings['label']} ({settings['path']})")
        return 0
    if getattr(args, "project", None):
        project_config(args.project, projects)
    with contextlib.closing(connect()) as db:
        if args.command == "start":
            start_task(db, args, projects)
        elif args.command == "add-item":
            add_item(db, args)
        elif args.command == "begin":
            begin_item(db, args)
        elif args.command == "finish-item":
            finish_item(db, args)
        elif args.command == "finish":
            finish_task(db, args)
        elif args.command == "show":
            show_task(db, args.task_id)
        elif args.command in ("history", "open"):
            list_tasks(db, args.project, args.command == "open")
        elif args.command == "index-functions":
            index_functions(db, projects, args.project)
        else:
            if args.limit < 1:
                raise ValueError("--limit은 1 이상이어야 합니다")
            find_function(db, args.project, args.term, args.limit)
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, sqlite3.Error, json.JSONDecodeError) as error:
        print(f"오류: {error}", file=sys.stderr)
        sys.exit(1)
