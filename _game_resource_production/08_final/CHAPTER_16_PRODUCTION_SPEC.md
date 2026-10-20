# C16 개발 준비 — 기근의 왕

[착수 안내](C03_C18_DEVELOPMENT_START.md) · [공통 구현](C03_C18_IMPLEMENTATION_GUIDE.md) · [공유 자산](../02_resource_ledger/C03_C18_SHARED_RESOURCES.md) · [최종 스토리](../../_story_scene_production/05_final_chapters/CHAPTER_16_FINAL.md)

## 상태·입출력

`SPEC_ONLY`. 입력 C15 전선 종료/흔적 1·C14 귀환 상태, C09 기근 소형 규칙. 제안 출력 `C16_COMPLETE / C16_DELIVERY_INTERCEPT_RULE / C16_WELL_REVERSED / C16_THRONE_CORE_DESTROYED / C16_RESIDUE_VECTOR`. 흔적 1도 C17/18까지 유지한다.

## 맵 통합 제안

`GP-M16-FAMINE` 회복 관로/이동 마력 관로/보급 목적지 세 갈래→중앙 우물→왕좌/핵 HYBRID/BOSS_ARENA. 지옥 공통 지형+관로·유량 표시. 우물은 회복/마력/보급선의 합류점이며 지상 수문과 기능을 혼동하지 않는다. C17 전쟁핵 방향 출구.

## 장면별 작업표

| 장면 | 구현·완료 조건 | 필수 리소스 |
| --- | --- | --- |
| S01 | 기존 물자는 유지·새 회복/마력 일부만 전달 중 탈취 | C16-DELIVERY, 검은 선/수신량 UI |
| S02 | 작은 회복원 생성→선 추적·벽 관로 절단→포로 회복 정상 전달 | C16-HEALPIPE, 포로·전달 사건 |
| S03 | 용사 힘 방출 미끼→이동 관로 측면 잠금 | C16-MANAPIPE, 이동/잠금·미끼 표시 |
| S04 | 운송은 유지하며 가짜 목적지로 탈취 지점 노출 | C16-SUPPLYPIPE, 목적지 교체/완료 직전 상태 |
| S05 | 세 유량 대조·밸브 순서+용사 압력 버티기→우물 역류 | C16-WELL, 유량/압력/원 전달선 |
| S06 | 기존 보유분·패링/회피·짧은 공격 창, 역류 때 큰 공격 | C16-KING, 흡수 강화/역류 취약 상태 |
| S07 | 왕좌/핵 파괴→공급 종료·흔적 2를 1과 겹쳐 기록 | C16-THRONECORE/RESIDUE |

## 필수 리소스

| ID | 최소 구성 | 상태 |
| --- | --- | --- |
| C16-KING | 기근의 왕, 새 전달량 흡수/강화·우물 역류 취약/공격·전선 종료 | SPEC_ONLY |
| C16-DELIVERY | 회복 구슬·새 마력·운송 물자와 검은 선/가로채기, 기존 재고 표시 | SPEC_ONLY |
| C16-HEALPIPE / C16-MANAPIPE / C16-SUPPLYPIPE | 고정/이동/목적지 변경 세 관로·잠금/차단/정상 전달 | SPEC_ONLY |
| C16-WELL / C16-THRONECORE | 세 유량·밸브·압력/역류 우물·왕좌/권능핵 | SPEC_ONLY |
| C16-RESIDUE | C15와 같은 형태/방향 흔적·두 벡터 겹침 기록 | SPEC_ONLY |

공유 GP-KIT-DEMON/NETWORK/LOGISTICS/HERO/CIVILIAN/UI/VFX/AUDIO. 관로 기본형은 공유하되 세 규칙의 이동/목적지/생성 표시를 구별한다.

## 구현 순서·완료 조건

P0: Chapter16·자원 전달 사건 ID/생성량/목적지/탈취량/정산 상태. 기존 인벤토리·현재 HP/MP를 매 프레임 비율 삭감하는 방식 금지. ‘이미 보유한 물약 사용’과 ‘새 회복 발생’ 경계는 실제 자원 API 확인 후 조정 가정으로 명시. P1: 세 관로→밸브/압력 협력→역류 창→보스 흡수/취약 FSM. P2: 왕좌+핵 종료·흔적 비교/인계, 수신/탈취 피드백.

기근은 보유분 강탈이 아니다. 정상 전달/가로채기/역류를 사건당 한 번 정산하고 재시도·지역 비활성화로 자원을 복제하지 않는다. 왕 육체 생사와 잔여 정체는 OPEN. 왕좌/핵·흔적 유지 정적 검토. 게임 실행 검증 미수행.
