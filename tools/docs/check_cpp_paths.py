"""Check Visual Studio source paths and quoted local C++ includes without building."""

from __future__ import annotations

import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PROJECT = ROOT / "SimpleGame"
NS = "{http://schemas.microsoft.com/developer/msbuild/2003}"
CODE_TAGS = {NS + "ClInclude", NS + "ClCompile"}
INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.MULTILINE)


def entries(path: Path) -> tuple[ET.Element, list[ET.Element]]:
    root = ET.parse(path).getroot()
    return root, [element for element in root.iter() if element.tag in CODE_TAGS and element.get("Include")]


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8")
    project, code = entries(PROJECT / "SimpleGame.vcxproj")
    filter_root, filtered = entries(PROJECT / "SimpleGame.vcxproj.filters")
    missing = []
    source_paths = [Path(entry.get("Include").replace("\\", "/")) for entry in code]
    filter_paths = [Path(entry.get("Include").replace("\\", "/")) for entry in filtered]
    if len(source_paths) != len(set(source_paths)):
        missing.append("duplicate project code entry")
    pch_source = next(
        (entry for entry in code if entry.get("Include", "").replace("\\", "/") == "src/core/stdafx.cpp"),
        None,
    )
    if pch_source is None or len([
        option for option in pch_source.findall(NS + "PrecompiledHeader")
        if option.text == "Create"
    ]) != 4:
        missing.append("stdafx.cpp must create the precompiled header in all four configurations")
    if set(source_paths) != set(filter_paths):
        missing.append("project and filter code entries differ")
    actual_paths = {
        path.relative_to(PROJECT)
        for path in (PROJECT / "src").rglob("*")
        if path.is_file() and path.suffix.lower() in {".cpp", ".h", ".hpp"}
    }
    if set(source_paths) != actual_paths:
        missing.append("project entries and physical code files differ")
    defined_filters = {
        entry.get("Include") for entry in filter_root.iter(NS + "Filter") if entry.get("Include")
    }
    for entry in filtered:
        relative = Path(entry.get("Include").replace("\\", "/"))
        expected = "src\\" + relative.parts[1]
        value = entry.findtext(NS + "Filter")
        if value != expected or value not in defined_filters:
            missing.append(f"filter mismatch: {relative.as_posix()}")

    include_groups = []
    for definition in project.iter(NS + "ItemDefinitionGroup"):
        compiler = definition.find(NS + "ClCompile")
        if compiler is None:
            continue
        value = compiler.findtext(NS + "AdditionalIncludeDirectories")
        if value is None:
            missing.append("compiler configuration lacks include directories")
            continue
        include_groups.append([Path(part.replace("\\", "/")) for part in value.split(";") if part and not part.startswith("%(")])
    if len(include_groups) != 4 or any(group != include_groups[0] for group in include_groups):
        missing.append("compiler configurations have different include directories")
    search_dirs = [PROJECT / directory for directory in (include_groups[0] if include_groups else [])]
    for directory in search_dirs:
        if not directory.is_dir():
            missing.append(f"include directory missing: {directory.relative_to(ROOT).as_posix()}")

    include_count = 0
    for relative in source_paths:
        source = PROJECT / relative
        if not source.is_file():
            missing.append(f"source missing: {relative.as_posix()}")
            continue
        if source.suffix.lower() not in {".cpp", ".h", ".hpp"}:
            continue
        try:
            content = source.read_text(encoding="utf-8-sig")
        except UnicodeError:
            missing.append(f"cannot read: {relative.as_posix()}")
            continue
        for name in INCLUDE.findall(content):
            include_count += 1
            name_path = Path(name.replace("\\", "/"))
            if not any((directory / name_path).is_file() for directory in [source.parent, *search_dirs]):
                missing.append(f"include missing: {relative.as_posix()} -> {name}")
    for item in missing:
        print(item)
    print(f"code_files={len(source_paths)} quoted_includes={include_count} missing={len(missing)}")
    return 1 if missing else 0


if __name__ == "__main__":
    sys.exit(main())
