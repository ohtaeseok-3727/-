# 게임소프트웨어공학 실습 지침

이 파일은 루트 [공통 지침](../AGENTS.md)에 더해 이 프로젝트에 적용된다. 게임 프로젝트 파일을 수정할 때는 작업을 시작한 위치와 관계없이 [공통 작업 원칙](../.agents/constitution/common/workflow.md)을 먼저 읽고, 아래 프로젝트 규칙도 함께 따른다. 상세 규칙은 [프로젝트 헌법 인덱스](../.agents/constitution/game-software/INDEX.md)에서 작업별로 찾아 읽는다.

- 모든 작업 전에 [최우선 작업 배정 원칙](../.agents/constitution/game-software/delegation-priority.md)과 [프로젝트 작업 안내](../.agents/constitution/game-software/project.md)를 읽는다.
- 이 프로젝트의 기획·구현·설명 전에 [문서 인덱스](DOCS_INDEX.md)와 [기획 인덱스](GAME_DESIGN.md)에서 관련 문서를 찾아 읽는다. 새 작업은 `codex/` 접두사의 새 Git 브랜치에서 진행한다.
- 문서 인덱스는 `python tools/docs/build_game_doc_index.py`로 갱신한다. 프로젝트 내부 문서 링크는 해당 문서 위치를 기준으로 한 상대 경로를 사용한다.
- 작업 시작·종료 기록과 함수 검색에는 [프로젝트 로컬 인덱스](.agents/projects/INDEX.md)의 도구와 DB를 사용한다. 함수 설명은 [로컬 함수 사전](.agents/projects/FUNCTIONS.md)에 기록한다. 상위 폴더의 작업 DB와 함수 사전은 사용하지 않는다.
- 사용자의 명시적인 요청이 없는 한 빌드하거나 실행하여 결과를 확인하지 않는다. 정적 검토는 가능하다.
- 코드 작성·수정에는 [코드 구성 원칙](../.agents/constitution/game-software/code.md)을, 스프라이트 작업에는 [스프라이트 필수 지침](../.agents/constitution/game-software/sprites.md)을 읽는다.
- 서브 에이전트 사용이 허용된 복수 독립 작업에서만 인덱스의 모델 배정 문서를 읽는다.
