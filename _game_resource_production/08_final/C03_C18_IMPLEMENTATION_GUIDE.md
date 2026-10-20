# C03~C18 공통 구현 지침 — 개발 방법과 연결 지점

[빠른 착수 안내](C03_C18_DEVELOPMENT_START.md) · [공유 리소스](../02_resource_ledger/C03_C18_SHARED_RESOURCES.md)

## 기준과 우선순위

장면 사실은 각 장의 최종 스토리, 최신 사용자 확정 변경, 현재 소스에서 확인한다. 오래된 기획의 ‘미정’을 최종 장면에 덮어쓰지 않는다. 스토리의 CONFIRMED/PROVISIONAL/OPEN을 유지한다. 아래 신규 구조는 구현 제안이며 이미 구현된 API로 오해하지 않는다. C++17, 기존 클래스 책임과 공개 인터페이스를 따른다.

## 확인한 코드와 적용 방법

| 현재 파일 | 확인한 역할 | 후속 개발에서 할 일 |
| --- | --- | --- |
| [Chapter.h](../../SimpleGame/src/chapters/Chapter.h) | 지도·진행·상호작용·그리기·결과 계약 | `ChapterN final : Chapter`가 상태를 소유. `BuildMap/Update/Interact/Goal/Retry/ExportFlags/ImportPrevious` 구현 |
| [Chapter3.h](../../SimpleGame/src/chapters/Chapter3.h), [Gameplay](../../SimpleGame/src/chapters/Chapter3Gameplay.cpp), [Map](../../SimpleGame/src/chapters/Chapter3Map.cpp), [View](../../SimpleGame/src/chapters/Chapter3View.cpp) | 조사·비살상·추격·지도·표현 분리 선례 | 새 장은 `ChapterN.h/.cpp`, 필요 시 `ChapterNGameplay.cpp/Map.cpp/View.cpp`로 정의 분리. 다른 장의 상태를 직접 캐스팅하지 않음 |
| [ChapterCatalog.cpp](../../SimpleGame/src/chapters/ChapterCatalog.cpp) | 현재 chapter1/2/3 팩토리 등록 | 새 장 하나씩 정상 등록. factory와 선행 완료를 함께 연결 |
| [GameSessionWorld.cpp](../../SimpleGame/src/app/GameSessionWorld.cpp) | 지역, 활성화, 결과 전달, 선행 조건·목표 선택 | C03 한정 분기와 지역 시작 인덱스가 있음. `OpenWorld/StoryChapter/StoryReady/SynchronizeChapterHistory/TrackedGoal`을 새 장까지 확장 |
| [GameSession.cpp](../../SimpleGame/src/app/GameSession.cpp) | 이동·세션 완료·스토리 스킵 등 | `Complete()`가 현재 chapter3 완료에 고정됨. 후속 장 등록만으로 진행되지 않으므로 완료 기준도 확장. 스킵 범위 확대는 별도 요청 없으면 하지 않음 |
| [GameSessionPortals.cpp](../../SimpleGame/src/app/GameSessionPortals.cpp) | 등록된 같은 챕터 포탈과 도착점 검증 | 현재 기록관/Argen에 특화된 목표 안내. 다른 실내 적용 시 출입 관계를 일반화. 챕터 간 이동을 이 API가 지원한다고 가정하지 않음 |
| [MapLayout.h](../../SimpleGame/src/world/MapLayout.h), [WorldAtlas](../../SimpleGame/src/world/WorldAtlas.h), [LevelMap](../../SimpleGame/src/world/LevelMap.h) | C++ 월드 좌표, 지역, 도로, 건물, 물, 포탈, 충돌 | 타일/TMX 파일을 새로 발명하지 않음. 로컬/세계 좌표 변환은 Atlas 경계에서. 문서용 맵 ID와 런타임 MapId를 구분 |
| [EnemySystem.h](../../SimpleGame/src/combat/EnemySystem.h) | 병종·EnemyStats, scriptedAI, minimumHealth | 일반 적 FSM/A* 재사용. 보스 단계는 장 상태로 제어. 생존 적 피해 하한·보상 없는 퇴장 경로 재사용 |
| [Chapter1Sprites.cpp](../../SimpleGame/src/chapters/Chapter1Sprites.cpp) | 기존 NPC/용사 시트 로더 | Hero 시트는 균일 격자가 아님. 기존 프레임 영역을 재사용. 신규 팩은 별도 로더/메타데이터 설계 후 연결 |
| [ExpeditionView.cpp](../../SimpleGame/src/ui/ExpeditionView.cpp), [MapView.cpp](../../SimpleGame/src/ui/MapView.cpp) | 조사 결과·목표·지역 안내 | 현재 C03 전용 결과 화면 있음. 대상 장의 증거 분류·게이지·목표·금지 입력을 표시 |
| [Chapter3Verification.cpp](../../SimpleGame/src/verification/Chapter3Verification.cpp) | 선행/비살상/재시도 검사 소스 | 후속 장의 상태 불변식 검증 선례. 문서 작성 단계에서 검사 실행하지 않음 |
| [vcxproj](../../SimpleGame/SimpleGame.vcxproj), [filters](../../SimpleGame/SimpleGame.vcxproj.filters) | 빌드 소스 목록 | 새 헤더/소스를 각각 한 번 등록. 문서 ID를 곧바로 enum 값으로 사용하지 않음 |

## 장 하나를 만드는 작업 순서

1. 해당 장의 입력 계약과 이전 `ExportFlags()`를 비교한다. C04부터 표의 플래그는 제안 이름이므로 실제 생산자·소비자에 같은 이름/값을 적용한다. 이전 조사 결과 전체를 매 프레임 복사하지 않는다. 장 진입·이전 결과 변화 시 전달한다.
2. 장면 표를 단계 enum과 상태 값으로 옮긴다. 병렬 조사는 bitset, 미선택은 optional, 고정 노드/신호는 array, 내보내는 정수 결과는 기존 ChapterFlags 사용을 우선 검토한다. 이름이나 수치를 확정 세계 설정으로 승격하지 않는다.
3. 제안 베이스 맵의 입구→필수 지점→출구를 C++ MapDefinition으로 블록아웃한다. 기존 지도 크기/문 폭을 절대 규칙으로 복사하지 않는다. 충돌·A*·투사체 시야가 같은 LevelMap 판정을 사용하도록 한다.
4. 각 상호작용에 고유 ObjectId와 선행 조건을 둔다. 조사 완료/문 열림/자료 회수와 대사 열람을 구분한다. 재방문·동일 버튼 반복으로 보상/사본/구조 인원/핵 파괴가 중복되지 않게 한다.
5. 전투·호위·퍼즐을 순서대로 연결한다. 목표 갱신은 실제 노드/운반대/구조 상태를 검사한다. 화면 연출이 끝났다는 이유만으로 물리 목표를 완료시키지 않는다.
6. `Retry`는 현재 전투의 임시 값만 초기화한다. 확정 증거·완료한 구조/선택은 보존한다. C12처럼 무중단 구간은 단계 간 이동/휴식으로 위험을 지우지 않도록 경계를 정한다. 정확한 체크포인트/영구 저장은 구현 제안이며 별도 정책 확인 대상이다.
7. 지역 등록·챕터 선택·목표 안내·완료 판정·결과 전달·여정 UI까지 연결한다. 지도 이동과 스토리 진행을 분리하여 조기 탐험으로 결론이 열리지 않게 한다.
8. 도형 임시 표현으로 전체 목표 흐름을 먼저 작성한 뒤 Blender 자산을 연결한다. 최종 자산 미완성과 게임 로직 미완성을 따로 기록한다.

## 반복 기믹의 공유 경계 — 아직 없는 신규 시스템은 제안

| 후보 모듈/데이터 | 사용 장 | 구현해야 할 계약 |
| --- | --- | --- |
| EvidenceBoard / 증거 항목 데이터 | 03~10, 11, 14, 18 | 물증·직접 목격·전언·해석과 확정 강도를 별도 필드로 저장. 근거 부족한 확정 배치 거부 |
| EscortRoute / 호송 상태 | 03, 05, 10, 11 | 경유점/정지/위험/완료를 실제 위치로 계산. 지역 비활성화가 호송 실패나 승리로 오인되지 않게 설계 |
| CooperativeSignal / 짧은 신호 | 06, 12, 13, 17 | 보낸 신호·수신·유효 구역·역할 분리. 용사 행동과 플레이어 행동이 모두 완료돼야 합류 |
| FlowNetwork / 노드와 방향 | 04, 07~09, 13~18 | 노드 활성·흐름 방향·공급·차단/복구 사건 표현. 공명/관로/왕권의 서로 다른 규칙을 단순 색 변형으로 통합하지 않음 |
| FactionRule / 제한 행동 규칙 | 04, 10~14 | 비공격/후퇴/구조/접견 조건. 장 종료를 영구 동맹 전환으로 처리하지 않음 |
| ResourceDelivery / 자원 전달 사건 | 16, 18 | 기존 보유량과 새 전달량을 분리하고 생성→운송→목적지 수령→가로채기→역류를 한 번만 정산 |
| ConflictMeter / 충돌 사건 | 09, 17, 18 | 공격/처치/패링/회피/환경 충격을 구분. 다단히트 중복 집계 규칙은 조정값으로 명시 |
| GateCore / 외핵·내핵 | 13, 14, 18 | 단면문/양면문 구분. 귀환 좌표와 복구파·쐐기 대기·동일 주기 폐쇄를 명시적 상태로 관리 |

처음 한 장에만 필요한 로직은 해당 Chapter에 둔다. 두 번째 실제 사용에서 계약이 같음을 확인한 것만 공통 모듈로 분리한다. 새 전역 관리자를 먼저 대량 생성하지 않는다.

## 비살상·정체성·결말 불변식

- `minimumHealth`를 정하는 것만으로 충분하지 않다. 근접/투사체/광역/궁극기/환경 피해 모두에서 생존 보장하고, 기믹 완료 후 보상 없는 전투 퇴장과 생존 표현을 연결한다.
- C03 세라드, C04 가르칸, C11 지형으로 무력화한 친위룡, C12 아크 드래곤은 처치 수로 성공을 판정하지 않는다. C05 점령 지휘자는 전투불능 목표이며 현재 약탈자다.
- C15~C17은 왕좌·핵/전선 종료만 확정한다. 왕의 육체 사망은 미확정. C18 죽음의 왕은 실제 사망한다.
- C17 잔영은 기록 재생, C18 실제 사자는 개인이다. 실제 사자의 공격 입력/적 자동 타기팅을 차단하고 귀속 해제로만 해방한다.
- 용사는 여성 주요 NPC이며 기존 용사 자산을 재사용한다. C18 예정 사령 상태에서도 죽지 않는다. 후유증을 즉시 완전 회복으로 지우지 않는다.
- 악마 개입 확인과 인간의 책임, 제한 협력과 화해, 현재 망 폐쇄와 모든 전쟁 종결을 구분한다.

## 아직 정하지 않은 것

정확한 지도 좌표·크기, 일반 적 수, HP/피해/시간/회복량, 신규 시트 프레임·해상도, 음원 파일 형식/로더, 전체 영구 저장 형식은 여기서 확정하지 않는다. 플레이스홀더로 개발을 시작할 수 있으며 구현 시 조정값과 출처를 기록한다. C14의 10초 실험·C12의 세 번 착지 제한처럼 최종본에 있는 횟수/시간만 해당 장에 보존한다.

## 검토·검증 계획

문서 단계: 장면 전수 대응, 상대 링크, 리소스 ID/맵 재사용, 소스 스냅샷·인덱스·DB 검색 확인. 후속 구현: 선언/정의와 프로젝트 등록, 선행 잠금, 비살상 모든 피해 경로, 재도전, 포탈·도착 충돌, 결과 전파·중복 보상, 지역 비활성화 시 상태 수명 정적 검토. 게임 빌드·실행·회귀·시각·음향 검증은 사용자 명시 요청 시 수행한다.
