# C10 개발 준비 — 전쟁 속으로

[착수 안내](C03_C18_DEVELOPMENT_START.md) · [공통 구현](C03_C18_IMPLEMENTATION_GUIDE.md) · [공유 자산](../02_resource_ledger/C03_C18_SHARED_RESOURCES.md) · [최종 스토리](../../_story_scene_production/05_final_chapters/CHAPTER_10_FINAL.md)

## 상태·입출력

`SPEC_ONLY`. 입력 C09 침입 사실, C07 자발적 개문/C08 소모 관측/C09 네 왕·고리 증거, 용사의 현장 지휘권. 제안 출력 `C10_COMPLETE / C10_EVAC_PRIORITY / C10_AUDIENCE_COMPLIED / C10_DEMON_EVIDENCE_ACCEPTED / C10_EXTERMINATION_ORDER`. 후송 지휘를 아직 인계하지 않는다.

## 맵 통합 제안

`GP-M10-FRONT` 국경 다중 전선·협곡·후송 회랑 BATTLEFIELD/ESCORT → `GP-M10-ROYAL` 왕문·왕정 접견 HYBRID. 왕정은 C12 전투에 재사용하고 C10 정상/접견과 C12 용맥 분단 상태를 분리한다. 왕문은 조건부 출입, 후송대는 왕정 내부 군대가 아님. 출구는 C11 후송 경로.

## 장면별 작업표

| 장면 | 구현·완료 조건 | 리소스 |
| --- | --- | --- |
| S01 | 국경 침입·부상자/고장 보급 수레 확인 | C10-FRONT, 침입 지도/군세·GP-LOGISTICS |
| S02 | 지휘판에서 전진 지원 보류→후송 우선→실제 배치/수레 경로 변경 | C10-COMMAND, 지휘 인장 |
| S03 | 타르곤 신호탑 3개 차단→후송 폭만 확보·추격 중단 | C10-TARGON/TOWERS, 협곡 방어망 |
| S04 | 라우렌 매복·공격 명령 장치 무력화→부상자 통로 분리 | C10-LAUREN/COMMAND, 인간 전진파 |
| S05 | 대표 수/중무장 해제/증거 봉인/정지 거리 모두 충족 | C10-SERAGON/GATE, 비전투 접견 검사 |
| S06 | 세 장 증거 실제 제시→아크 드래곤 직접 확인 | GP-ARC, GP-RECORD |
| S07 | 개입 인정과 침입 책임 지적 둘 다 기록 | 왕정 침입 지도·증거 판 |
| S08 | 전투 능력 제거/저항 시 섬멸 왕명 수령→후송 위험 목표 | C10-ORDER, 왕명 원문/봉인 |

## 필수 리소스

| ID | 최소 구성 | 상태 |
| --- | --- | --- |
| C10-TARGON | 몬스터 지휘관 고유 외형·방어 지휘, 종족 외형은 원문만으로 확정 불가 | SPEC_ONLY |
| C10-LAUREN | 인간 독자 전진파 고유 지휘관·매복/명령 중단, 병사는 GP-HUMAN 변형 후보 | SPEC_ONLY |
| C10-SERAGON | 왕문 수문장 고유 용족 외형·검사/거부/허가. 필수 보스전 없음 | SPEC_ONLY |
| C10-TOWERS / C10-COMMAND | 신호탑 3인스턴스·지휘판·공격 명령 장치·후송 분리 표식 | SPEC_ONLY |
| C10-GATE / C10-ORDER / C10-FRONT | 접견 체크리스트·증거 봉인함·정지선·왕명 원문·국경 지도 | SPEC_ONLY |

공유: GP-ARC(동일 왕)/DRAGON/HERO/LOGISTICS/SIGNAL/RECORD/KIT-FIELD/KIT-DRAGON/KIT-STONE/UI/VFX/AUDIO. 중무장 해제는 일시 보관/비활성 상태 제안이며 소유 아이템 삭제가 아님.

## 구현 순서·완료 조건

P0: Chapter10·전선/왕문 지역·C09 결과 연결 → 후송 route와 지휘판 선택 결과 → 신호탑 세 상태. P1: 라우렌 명령 차단과 후송 전투 분리 → 접견 조건 FSM(조건별 거부 사유 제공) → 증거 제시/왕명 수령. P2: 병력·수레 실제 동선 변화, GP-ARC 접견 연출과 C11 위험 안내.

군세 전멸·왕문 강제 파괴로 우회 성공시키지 않는다. 악마 증거 인정이 인간 책임 면제/평화로 바뀌지 않음. 지휘권 유지·세 탑·접견 네 조건·증거/왕명 전달·후송 생존을 정적 확인. 게임 실행 검증 미수행.
