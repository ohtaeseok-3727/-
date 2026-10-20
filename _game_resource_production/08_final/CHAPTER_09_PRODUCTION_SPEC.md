# C09 개발 준비 — 전쟁의 왕(제한 투영)

[착수 안내](C03_C18_DEVELOPMENT_START.md) · [공통 구현](C03_C18_IMPLEMENTATION_GUIDE.md) · [공유 자산](../02_resource_ledger/C03_C18_SHARED_RESOURCES.md) · [최종 스토리](../../_story_scene_production/05_final_chapters/CHAPTER_09_FINAL.md)

## 상태·입출력

`SPEC_ONLY`. 입력 C08 상부 보고 코드/좌표, C07 계약 및 C08 관측 증거. 제안 출력 `C09_COMPLETE / C09_FOUR_KINGS / C09_RING_FUNCTIONS / C09_WAR_PROJECTION_REPELLED / C09_EMPIRE_CROSSED_BORDER`. C17은 같은 전쟁의 왕의 후속 전선이며 새 왕으로 분리하지 않는다.

## 맵 통합 제안

`GP-M09-BLOODCASTLE` 혈문성 DUNGEON/BOSS_ARENA. 계급 인증→네 문장 소형 기믹 구역→현계 고리와 세 기능 전투 구역→투영전/증거 대조→국경 관측창·현세 귀환 출구. GP-KIT-STONE/DEMON 공유. 고리는 고정/공급/좌표가 구별되는 하나의 설비.

## 장면별 작업표

| 장면 | 구현·완료 조건 | 리소스 |
| --- | --- | --- |
| S01 | 이름 아닌 계급/보고 경로 인증 | C09-RANK, 계급문 |
| S02 | 정복 소유 변경·기근 새 마력 가로채기·전쟁 충돌 충전·죽음 종료 흔적 수집 | C09-FOUR, 네 문장·소형 시험 장치 |
| S03 | 고정/공급/좌표 기능과 제한 투영 확인 | C09-RING, 세 기능 상태 UI |
| S04 | 지휘관 3명: 이동 제한/지속 공급/위치 왜곡 해제 | C09-COMMANDERS, 기능별 예고/장치 |
| S05 | 맞교환을 줄여 제한 투영 상대 | C09-WARKING, 충돌 강도 게이지 |
| S06 | 전투 중단 구간에 왕 주장과 C07/08 증거 대조 | C09-CLAIM, 증거 있음/근거 없음 분리 |
| S07 | 세 기능 차단→완전 현현 실패·퇴각→군대 이동 관찰·연락 수신 | 관측창/국경 불빛/침입 지도·신호 |

## 필수 리소스

| ID | 요구 | 상태 |
| --- | --- | --- |
| C09-RANK / C09-FOUR | 계급 인증 장치·네 고유 문장/소형 규칙 판 | SPEC_ONLY |
| C09-RING | 고정점/공급점/좌표점·차단/투영 불안정 상태 | SPEC_ONLY |
| C09-COMMANDERS | 악마 지휘관 3개 식별 실루엣/도구 변형, 각 기능 전투 모션 | SPEC_ONLY |
| C09-WARKING | 동일 전쟁의 왕 베이스+불완전 투영 재질/형상·공격·불안정·퇴각, C17과 공유 | SPEC_ONLY |
| C09-CLAIM | 주장 카드·대조 근거·국경 관측·현세 연락 | SPEC_ONLY |

공유: GP-DEMON/KIT-DEMON/KIT-STONE/NETWORK/RECORD/SIGNAL/HERO/UI/VFX/AUDIO. 네 왕 문장은 C15~18에 재사용. C09의 죽음 소형 기믹은 실제 개인 사자를 소모품으로 쓰지 않는다.

## 구현 순서·완료 조건

P0: Chapter9·계급문/네 소형 규칙 → 고리 세 기능 FSM과 지휘관 encounter. P1: 기능 차단이 왕 투영을 약화하는 연동·충돌 사건 집계·증거 대조 중단 상태. 제한 투영을 죽여 왕 사망을 기록하지 않음. P2: 군대가 이미 이동 중임을 관측창에서 먼저 제시하고 연락 신호로 침입 확정→C10 출구.

네 왕과 현계 고리·일부 개입만 확정. 왕의 “모든 전쟁 통제” 주장은 근거 없음으로 남긴다. 세 기능 차단/정체성 재사용/증거 대조/침입 결과의 전달·재시도를 정적 검토한다. 게임 실행 검증 미수행.
