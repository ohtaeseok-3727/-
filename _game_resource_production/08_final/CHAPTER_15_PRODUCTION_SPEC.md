# C15 개발 준비 — 정복의 왕

[착수 안내](C03_C18_DEVELOPMENT_START.md) · [공통 구현](C03_C18_IMPLEMENTATION_GUIDE.md) · [공유 자산](../02_resource_ledger/C03_C18_SHARED_RESOURCES.md) · [최종 스토리](../../_story_scene_production/05_final_chapters/CHAPTER_15_FINAL.md)

## 상태·입출력

`SPEC_ONLY`. 입력 C14 지옥 진입·두 좌표 조각/외핵 보존 상태와 C09 정복 소형 규칙. 제안 출력 `C15_COMPLETE / C15_OWNERSHIP_RULE / C15_THRONE_DESTROYED / C15_POWER_CORE_DESTROYED / C15_RESIDUE_VECTOR`. C14 귀환 필수 상태를 누락하지 않는다. 흔적의 정체를 C18 이전에 왕권이라고 확정하지 않는다.

## 맵 통합 제안

`GP-M15-CONQUEST` 소유된 입구→약탈 창고/포획병/중첩 깃발 세 점령지→왕좌/권능핵 BOSS_ARENA/BRANCHING. 같은 지옥 지형+소유 표식 상태층. 세 점령지마다 전체 타일셋을 만들지 않는다. 해방된 사람은 아군 모집 지점이 아닌 자기 출구로 이탈. C16 공급망 방향 출구.

## 장면별 작업표

| 장면 | 구현·완료 조건 | 필수 리소스 |
| --- | --- | --- |
| S01 | 밀어내는 바닥/비소유자 거부 문→첫 표식 역전 | C15-OWNERSHIP, 소유/해방 지형 |
| S02 | 운송 기록+원 소유 표식 대조→물자 잠금/공급 연결 해제 | C15-CARGO, GP-LOGISTICS/RECORD |
| S03 | 용사 방어 중 목 귀속 고리 해제→포획병 각자 탈출 | C15-CAPTIVES, GP-CIVILIAN/인간 병사 변형 |
| S04 | 3깃발의 소유선을 왕좌로 재배치→동시 비활성화 | C15-FLAGS, 하나 파괴 시 영역 흡수 규칙 |
| S05 | 왕좌 연결은 지형/지휘 감소, 핵 연결은 증원/출력 감소 | C15-THRONE/CORE, 서로 다른 기능 |
| S06 | 남은 소유 슬롯 해방→왕의 명령/지형 능력 약화 | C15-KING, 소유 슬롯 가시화 |
| S07 | 왕좌와 권능핵 모두 파괴→전선 종료·검은 궤적 방향 기록 | C15-RESIDUE, 발생/흐름/이탈 |

## 필수 리소스

| ID | 최소 구성 | 상태 |
| --- | --- | --- |
| C15-KING | 정복의 왕 고유 실루엣·소유 명령/지형 조작·슬롯 상실별 패턴 | SPEC_ONLY |
| C15-OWNERSHIP / C15-FLAGS | 소유 문장/바닥·문·슬롯·귀속선·깃발 3개 | SPEC_ONLY |
| C15-CARGO / C15-CAPTIVES | 원 표식/운송 기록·목 귀속 고리·해방/각자 탈출 모션 | SPEC_ONLY |
| C15-THRONE / C15-CORE | 소유/지휘 왕좌와 공급/출력 핵·연결/차단/파괴 | SPEC_ONLY |
| C15-RESIDUE | 정체불명 비활성 귀속 흔적의 발생 위치/방향·기록 | SPEC_ONLY |

공유 GP-KIT-DEMON/NETWORK/LOGISTICS/RECORD/DEMON/HERO/UI/VFX/AUDIO. C09 정복 문장, C18 소유 규칙/잔여 시각 자산 재사용.

## 구현 순서·완료 조건

P0: Chapter15·소유 주체/대상/슬롯을 정수 HP와 분리한 상태로 구현. 위치 판정·문·공급·명령이 같은 소유 상태를 참조. P1: 세 점령지 조작→3깃발 동시 해제 조건→왕좌/핵 기능별 연결과 보스 패턴. 파괴만으로 깃발 퍼즐을 우회하지 않음. P2: 왕좌/핵 둘 다 종료 후 흔적 1 좌표/방향 생성·C16 인계.

전선 종료는 왕 육체 사망과 다르다. 포획병이 동료로 합류하지 않으며 물자 귀환과 병력 획득을 혼동하지 않는다. 소유 전환·공급 해제·탈출 중복·3깃발 동시 조건·핵/왕좌 둘 다 파괴·벡터 전달 정적 검토. 게임 실행 검증 미수행.
