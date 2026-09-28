# 프로젝트 지원 도구

프로젝트 루트에서 아래 상대 경로로 실행한다.

| 폴더 | 용도 | 도구 |
| --- | --- | --- |
| `knowledge/` | 로컬 작업 기록·함수 색인·문서 검색 | `project_hub.py`, `local_knowledge.py` |
| `docs/` | 문서 인덱스·상대 링크·C++ 경로의 정적 검사 | `build_game_doc_index.py`, `check_project_links.py`, `check_cpp_paths.py` |
| `assets/` | 챕터 2 PNG 상태 검사·준비 | `audit_chapter2_assets.py`, `prepare_chapter2_sprites.py` |

각 도구는 자신의 위치에서 프로젝트 루트를 계산한다. DB와 생성 결과의 위치는 프로젝트 안의 상대 경로로 지정한다. 게임 빌드나 실행 검증은 이 도구의 역할이 아니다.
