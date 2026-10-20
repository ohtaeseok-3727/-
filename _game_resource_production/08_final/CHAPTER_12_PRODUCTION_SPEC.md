# C12 개발 준비 — 왕을 멈추는 전쟁

[착수 안내](C03_C18_DEVELOPMENT_START.md) · [공통 구현](C03_C18_IMPLEMENTATION_GUIDE.md) · [공유 자산](../02_resource_ledger/C03_C18_SHARED_RESOURCES.md) · [최종 스토리](../../_story_scene_production/05_final_chapters/CHAPTER_12_FINAL.md)

## 상태·입출력

`SPEC_ONLY`. 입력 C11 후송 안전·지휘 인계·둘만 복귀, C06 짧은 신호/용족 치료 구조 및 C02 분리 대응 경험. C02 기존 신호 구현은 착수 시 실제 파일에서 확인하고 없는 연속성을 임의 주장하지 않는다. 제안 출력 `C12_COMPLETE / C12_ARC_ALIVE_DISABLED / C12_ORDER_REVOKED / C12_CONQUEST_BRIDGEHEAD`.

## 맵 재사용 제안

`GP-M10-ROYAL`의 왕정 베이스를 재사용한 `DIVIDED_BATTLE → REJOINED → GROUNDED → MEASUREMENT_ANCHORED` 상태. 플레이어/용사 구역·엄폐·낙하 위험·신호 노드·용맥 기둥·착지 지점을 같은 전장 안에 둔다. 단계 사이 맵 전환/휴식으로 무중단 전투를 끊지 않는다. C13 첫 문은 왕정과 전선 경계로 연결한다.

## 장면별 작업표

| 장면 | 구현·완료 조건 | 필수 리소스 |
| --- | --- | --- |
| S01 | 둘만 왕정 재진입·군대 없음 확인 | GP-ARC/HERO, 왕정 재방문 |
| S02 | 왕의 전술적 분단→왕은 플레이어 쪽 | C12-DIVIDE, 용맥 벽·구역 분리 |
| S03 | 플레이어 회피/엄폐/낙하 생존하며 신호 노드 3개 해제 | C12-NODES, 왕 공격 예고·노드 |
| S04 | 용사는 분신/비행 충격 버티며 짧은 신호로 반대편 봉인 파괴 | C12-CLONE/SIGNAL, 용사 위치/신호 상태 |
| S05 | 플레이어 착지 표시→용사 해당 기둥 파괴→실제 합류 | C12-LANDING, 두 행동 결합 |
| S06 | 치료 구조/용맥으로 재생 지점 제한→착지 제한 3회 | C12-REGEN, 날개 재생 주기/용맥 차단 |
| S07 | 지상 고정·전투불능→현재 섬멸 명령 중단 | GP-ARC 생존, 목표 ‘명령권 상실’ |
| S08 | 초반부터 약하게 보인 계측 문양→일제 점등/고정→정복 교두보 | C12-MEASURE, 누적 연출·첫 문장 |

## 필수 리소스

| ID | 최소 구성 | 상태 |
| --- | --- | --- |
| C12-DIVIDE / C12-NODES | 분단 용맥/벽·엄폐·낙하 구역·3노드·봉인 | SPEC_ONLY |
| C12-CLONE / C12-SIGNAL | GP-ARC 변형 분신·비행 충격, C06 짧은 신호 패턴 | SPEC_ONLY |
| C12-LANDING / C12-REGEN | 착지 표식/파괴 기둥·날개막 재생/연결·3회 누적 표시 | SPEC_ONLY |
| C12-MEASURE | 희미한 계측→활성→땅 고정, 정복 문장·교두보 | SPEC_ONLY |

공유 GP-ARC는 C10 동일 왕. 비행/착지/피격/지상 고정/생존 전투불능 추가 모션 필요. GP-HERO/SIGNAL/KIT-DRAGON/KIT-DEMON/NETWORK/UI/VFX/AUDIO 공유.

## 구현 순서·완료 조건

P0: Chapter12에서 기존 왕정 좌표/건물 공유 방식 결정(다른 Chapter에 지도 복제 대신 공통 layout helper 제안), 분단 지형 충돌과 양측 FSM. P1: 노드 3개→신호→착지 표시와 실제 기둥 파괴→재합류→착지 제한 3회→생존 전투불능. 모든 피해 경로에 왕의 생존 하한 적용. P2: 명령 중단/초반 계측 복선/소진 후 교두보 활성.

플레이어 단독 폭딜로 왕 처치·용사 순간 이동 합류·지휘 인계 취소를 허용하지 않는다. 왕은 용서/동맹을 선언하지 않는다. 신호 끊김·잘못된 착지·단계 재시도·3회 횟수 중복·C13 결과를 정적 확인한다. 게임 실행 검증 미수행.
