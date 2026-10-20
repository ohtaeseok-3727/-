# SOURCE_INDEX

C01 FOUND
C02 FOUND
C03 FOUND
C04 FOUND
C05 FOUND
C06 FOUND
C07 FOUND
C08 FOUND
C09 FOUND
C10 FOUND
C11 FOUND
C12 FOUND
C13 FOUND
C14 FOUND
C15 FOUND
C16 FOUND
C17 FOUND
C18 FOUND

FINAL_SCENE_PRODUCTION_AUDIT.md FOUND

## Sources used for Chapter 1
- _story_scene_production/05_final_chapters/CHAPTER_01_FINAL.md
- GAME_DESIGN.md
- SimpleGame/Assets/Characters/SmallKnight/
- Existing project asset folders under SimpleGame/Assets/

## Canon override applied
HERO-01 = Female, CONFIRMED.

## C03~C18 개발 준비 자료 — 2026-10-20

- [빠른 착수 안내](../08_final/C03_C18_DEVELOPMENT_START.md)에서 대상 장 하나와 공통 구현 지침을 먼저 읽는다. 각 장 문서에 최종본의 모든 장면을 작업표로 대응시켰다.
- 기준: [최종 챕터 목록](../../_story_scene_production/05_final_chapters/INDEX.md)의 CHAPTER_03_FINAL~CHAPTER_18_FINAL 원문 전체. 과거 초안이나 적용 방안으로 덮어쓰지 않았다.
- 현재 코드 기준: Chapter/ChapterCatalog, GameSession/World/Portals, Chapter3 및 도시 구조, MapLayout/WorldAtlas/LevelMap, EnemySystem, 기존 스프라이트 로더/자산 목록과 UI. C03은 기존 구현 확장, C04~18은 미등록이다.
- [공통 구현 지침](../08_final/C03_C18_IMPLEMENTATION_GUIDE.md)은 실제 확장 지점과 신규 제안 모듈을 구분한다. [공유 리소스](../02_resource_ledger/C03_C18_SHARED_RESOURCES.md)는 기존 경로와 신규 제작 요구를 구분한다.
- [소스 스냅샷](../08_final/C03_C18_SOURCE_SNAPSHOT.json)에 읽은 주요 원본의 SHA-256과 각 장의 장면 ID·준비 문서·문서용 맵 ID를 기록한다. 이후 원본이 바뀌면 해당 장만 재검토한다.
- 범위는 개발 준비 문서이며 실제 PNG/Blender/음원/게임 코드 제작·배포나 게임 실행 검증이 아니다. 실제 자산 목록은 스냅샷의 경로/해시 목록으로 확인하며 품질 검증과 구분한다.
