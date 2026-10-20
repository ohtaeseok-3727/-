# C07 개발 준비 — 지워진 자

[착수 안내](C03_C18_DEVELOPMENT_START.md) · [공통 구현](C03_C18_IMPLEMENTATION_GUIDE.md) · [공유 자산](../02_resource_ledger/C03_C18_SHARED_RESOURCES.md) · [최종 스토리](../../_story_scene_production/05_final_chapters/CHAPTER_07_FINAL.md)

## 상태·입출력

`SPEC_ONLY`. 입력: C03 삭제/필체·인장, C04 친필 원정 명령, C06 백색성 좌표·방위 표식. 제안 출력 `C07_COMPLETE / C07_VOLUNTARY_CONTACT / C07_INTERNAL_GATE_APPROVAL / C07_DISGUISED_SUPPLY_CODES / C07_OBSERVATORY_COORDS`. 앞선 장의 증거는 이름만 복사하지 말고 비교 가능한 기록 항목으로 전달한다.

## 맵 통합 제안

`GP-M07-WHITECASTLE` 백색성 DUNGEON/INVESTIGATION. 문장석 입구→명령실→계약실↔지하 위장 창고→개문 노드 전투→관측 기록·송신대→출구. 같은 성의 석재/회랑 키트, 봉인 정상/불안정/안정화 상태. 과거의 제7영웅을 현재 살아 있는 보스로 추가하지 않는다.

## 장면별 작업표

| 장면 | 구현·완료 조건 | 필수 리소스 |
| --- | --- | --- |
| S01 | 삭제 기록+방위로 원 출입 규칙 복원 | C07-HERALDRY: 깎인 문장석·메운 이름 홈 |
| S02 | 성 명령/원정 친필/인장 흠집 일치 확인 | C07-HAND: 필획 꺾임·개인 인장 |
| S03 | 수정 계약·두 번 반려·세 번째 자필 승인·내부 입력 대조 | C07-CONTRACT: 수정본·개문 순서 장치 로그 |
| S04 | 인간 상자/몬스터 포대/용 봉인함을 열어 내부 코드 비교 | C07-SUPPLY: 같은 식별 코드·관측소 규격 |
| S05 | 잔존 악마 재활성화→승인 순서 역차단 | C07-DEMON/NODES: 개문 규칙 전투 |
| S06 | 편향/분노/결정 속도 측정표와 자필 책임 함께 분류 | C07-OBSERVATION, GP-RECORD/UI |
| S07 | 코드+송신 방향→좌표 복사·잔존 봉인 안정화 후 출구 | C07-NODES, 관측소 좌표 |

## 필수 리소스

| ID | 구성·요구 | 상태 |
| --- | --- | --- |
| C07-HERALDRY / C07-HAND | 문장석 전후·문서 비교·필획 확대·인장 흠집 | SPEC_ONLY |
| C07-CONTRACT / C07-OBSERVATION | 서로 다른 수정/반려/승인본·조작 로그·반응 측정표 | SPEC_ONLY |
| C07-SUPPLY | 세 진영 포장과 공통 내부 코드, GP-LOGISTICS 외피 변형 | SPEC_ONLY |
| C07-DEMON / C07-NODES | GP-DEMON 잔존 악마, 승인·재활성·역차단 노드/연결선·봉인·송신 방향 | SPEC_ONLY |

공유: GP-KIT-STONE/DEMON, GP-RECORD/NETWORK/LOGISTICS/HERO/UI/VFX/AUDIO. 새 주요 인물 스프라이트는 필수 아님; 기록 속 제7영웅의 외형/이름을 창작하지 않는다.

## 구현 순서·완료 조건

P0: Chapter7·지도/목표·입력 증거 연결 → 조사 완료 상태 → 순서 노드 FSM. P1: 실제 승인 이력과 플레이어 역차단 배열을 구분하고 잘못된 노드는 재시도 가능한 전투 위험으로 처리(정확한 수치는 제안). 봉인 안정화와 좌표 복사 둘 다 출구 조건. P2: 계약 비교/같은 내부 코드/반응표 UI 및 전투 예고.

확정은 자발적 접촉·직접 내부 개문 승인·위장 물자·관측이다. 적극적 심리 유도는 정황, 동시 피격 사전 인지·최종 목적/운명·전쟁 전체 설계는 미확정. 전투 승리만으로 S06~07 조사를 생략하지 않는다. 역순 차단·재활성 방지·재시도·결과 전달을 정적 확인한다. 게임 실행 검증 미수행.
