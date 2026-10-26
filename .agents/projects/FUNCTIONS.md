# 게임소프트웨어공학 실습 로컬 함수 사전

`python tools/knowledge/project_hub.py index-functions --project game-software`로 이 프로젝트의 헤더 선언을 로컬 DB에 색인하고, `find-function game-software "이름"`으로 검색한다. 아래 목록은 코드 탐색의 출발점이다. 실제 동작은 구현 파일과 관련 기획 문서에서 확인한다.

| 영역 | 선언·구현 | 함수 | 코드에서 확인한 역할 |
| --- | --- | --- | --- |
| 게임 세션 | [GameSession.h](../../SimpleGame/src/app/GameSession.h), [GameSession.cpp](../../SimpleGame/src/app/GameSession.cpp) | `ApplyPendingTravel` | 지역 단위 검증에서는 예약 이동을 적용한다. 연속 월드에서는 예약 이동을 비우고 순간이동하지 않는다. |
| 챕터 흐름 | [Chapter.h](../../SimpleGame/src/chapters/Chapter.h), [Chapter.cpp](../../SimpleGame/src/chapters/Chapter.cpp) | `PendingTravel`, `RequestTravel` | 보류 중인 이동 요청을 조회하거나 저장한다. |
| 챕터 2 목표 | [Chapter2.h](../../SimpleGame/src/chapters/Chapter2.h), [Chapter2.cpp](../../SimpleGame/src/chapters/Chapter2.cpp) | `ChooseTarget` | 협곡 목표의 우선순위를 기록한다. 연속 월드에서는 위치를 유지하며 기존 지역 검증에서만 진입 지점 이동을 예약한다. |
| 전투 입력 | [BasicAttack.h](../../SimpleGame/src/combat/BasicAttack.h), [BasicAttack.cpp](../../SimpleGame/src/combat/BasicAttack.cpp) | `Press`, `Update`, `ConsumeHits` | 3단계 연속 공격 입력을 처리하고, 시간에 맞춰 타격을 기록한 뒤 소비자에게 넘긴다. |
| 적 동작 | [EnemySystem.h](../../SimpleGame/src/combat/EnemySystem.h), [EnemySystem.cpp](../../SimpleGame/src/combat/EnemySystem.cpp) | `Update` | 활성 적을 시간 간격에 맞춰 갱신하고 플레이어에게 들어오는 피해를 반환한다. |
| 스킬 | [SkillSystem.h](../../SimpleGame/src/player/SkillSystem.h), [SkillSystem.cpp](../../SimpleGame/src/player/SkillSystem.cpp) | `Update` | 남은 재사용 대기 시간을 줄이고 완료된 항목을 제거한다. |

프로젝트 코드가 바뀌면 선언과 구현을 대조해 이 표를 갱신한다.

## 2026-09-29 연속 오픈월드

| 영역 | 선언·구현 | 함수 | 코드에서 확인한 역할 |
| --- | --- | --- | --- |
| 월드 조립 | [WorldAtlas.h](../../SimpleGame/src/world/WorldAtlas.h), [WorldAtlas.cpp](../../SimpleGame/src/world/WorldAtlas.cpp) | `Build`, `RegionAt`, `ToWorld`, `ToLocal` | 여섯 지역을 공통 좌표에 합치고 지역 소유자·좌표·발견을 조회한다. |
| 월드 세션 | [GameSession.h](../../SimpleGame/src/app/GameSession.h), [GameSessionWorld.cpp](../../SimpleGame/src/app/GameSessionWorld.cpp) | `OpenWorld`, `FollowPlayer`, `ActivateRegion` | 지형을 한 번 조립하고 도보 이동에서는 지역 적·전리품과 챕터 소유자만 교체한다. |
| 주요 목표 | [ChapterGoals.cpp](../../SimpleGame/src/chapters/ChapterGoals.cpp), [GameSessionWorld.cpp](../../SimpleGame/src/app/GameSessionWorld.cpp) | `Goal`, `StoryChapter`, `TrackedGoal` | 스토리 진행으로 미완료 목표를 선택하고 현재 탐험 지역과 독립된 세계 위치로 안내한다. |
| 재방문 | [ChapterGoals.cpp](../../SimpleGame/src/chapters/ChapterGoals.cpp) | `VisitMap`, `PlayerArrived` | C01 전투 시작 기록과 C02 지역별 전투 상태를 보존하고 동행 위치를 연결한다. |
| 야영지 | [GameSessionWorld.cpp](../../SimpleGame/src/app/GameSessionWorld.cpp), [GameSession.cpp](../../SimpleGame/src/app/GameSession.cpp) | `InCombat`, `ReturnToCamp`, `Inspect` | 안전한 휴식·복귀 지점 등록·전투 중 귀환 차단·패배 재도전을 처리한다. |
| 충돌 좌표 | [LevelMap.cpp](../../SimpleGame/src/world/LevelMap.cpp) | `UseWorldGeometry`, `Blocked` | 지역의 적·챕터 좌표를 실제 월드 충돌로 변환해 렌더 지형과 일치시킨다. |
| 월드 렌더 | [GameSessionWorld.cpp](../../SimpleGame/src/app/GameSessionWorld.cpp), [SceneGraph.cpp](../../SimpleGame/src/rendering/SceneGraph.cpp) | `AddWorldActors`, `DrawWorldEnemy`, `WorldDrop`, `SetDynamicOrigin` | 지역별 적·전리품을 공통 세계 좌표와 깊이 정렬에 연결한다. |
| 탐험 화면 | [ExpeditionView.cpp](../../SimpleGame/src/ui/ExpeditionView.cpp) | `DrawGuide`, `DrawMap`, `DrawJournal`, `DrawDefeat` | 목적지 방향·세계 지도·기록·패배 후 입력을 표시한다. |
| 월드 검증 | [OpenWorldVerification.cpp](../../SimpleGame/src/verification/OpenWorldVerification.cpp) | `RunOpenWorldVerification` | 도로·좌표·지역 보존·목표·재도전·선택 후 위치 유지 검증 코드. 빌드와 실행은 미수행이다. |


## 2026-09-29 프레임 캐시 최적화

| 선언·구현 | 함수 | 역할 |
| --- | --- | --- |
| [MapView.cpp](../../SimpleGame/src/ui/MapView.cpp) | `GroundGeometry`, `DrawTile` | 4096개 고정 슬롯에 지형 정점을 보관하고 맵 revision·시드 변경 시 비운다. 좌표가 일치한 타일은 지형을 재분류하지 않고 재사용한다. |
| [Renderer.cpp](../../SimpleGame/src/rendering/Renderer.cpp) | `DrawGeometry` | 로컬 정점을 vector::insert로 일괄 추가한 뒤 화면 원점을 적용한다. 기존 배치와 정점 순서를 유지한다. |
| [GeometryCache.h](../../SimpleGame/src/rendering/GeometryCache.h) | `Append` | 소품 정점 일괄 복사 후 연속 메모리에서 위치·크기를 적용한다. |
| [LevelMap.cpp](../../SimpleGame/src/world/LevelMap.cpp) | `CellProp`, `GenerateCellProp` | 빈 셀을 포함한 절차적 장애물 결과를 최대 4096개 저장한다. 기존 생성식은 GenerateCellProp에 두며 Load/Generate에서 캐시를 비운다. |
| [OpenWorldVerification.cpp](../../SimpleGame/src/verification/OpenWorldVerification.cpp) | `VerifyPropCache` | 음수 좌표·슬롯 교체·시드 변경·맵 재로드와 충돌을 검사한다. 실행은 미수행이다. |


## 2026-09-30 필드 몬스터 개체군

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [FieldPopulation.cpp](../../SimpleGame/src/combat/FieldPopulation.cpp) | `Populate`, `Spawn`, `Update` | 안전 지형에 최대 12개 3마리 무리를 배치한다. 스토리와 구분된 ID를 사용하고 1초 단위 생존 검사·45초 재생성 대기·거리 조건을 적용한다. |
| [GameSessionWorld.cpp](../../SimpleGame/src/app/GameSessionWorld.cpp), [GameSession.cpp](../../SimpleGame/src/app/GameSession.cpp) | `OpenWorld`, `Update` | 지역별 개체군을 초기화하고 현재/인접 지역 적 저장소와 연결한다. |
| [OpenWorldVerification.cpp](../../SimpleGame/src/verification/OpenWorldVerification.cpp) | `VerifyFieldPopulation` | 상한·유효 위치·ID 분리·중복·재생성 지연을 검사하는 미실행 회귀 코드다. |


## 2026-09-30 챕터 2 용사 합류 수정

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [Chapter2Combat.cpp](../../SimpleGame/src/chapters/Chapter2Combat.cpp) | `UpdateHero` | EnemyPathfinder 결과를 waypoint 인덱스로 따라 이동하며 구간별 충돌 확인. 실패 시 정지하고 간격을 두어 재탐색한다. |
| [Chapter2Map.cpp](../../SimpleGame/src/chapters/Chapter2Map.cpp) | `FortressMap` | 양 사선 진입로와 합류 신호를 연결하는 바닥 통로를 제공한다. |
| [Chapter2Sprites.cpp](../../SimpleGame/src/chapters/Chapter2Sprites.cpp) | `DrawHero` | 용사의 이동 방향을 기존 시트의 좌우 반전에 적용한다. |
| [Chapter2Verification.cpp](../../SimpleGame/src/verification/Chapter2Verification.cpp) | `VerifyOrders` | 실제 용사 A* 이동·벽 충돌·합류 완료를 검사하는 미실행 회귀 코드를 포함한다. |

## 2026-09-30 Esc 시스템 메뉴

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [SettingsMenu.cpp](../../SimpleGame/src/ui/SettingsMenu.cpp) | `Buttons`, `MouseButton`, `Activate`, `Draw` | 공통 버튼 배치와 클릭 판정, 눌렀다 놓는 입력, 확인/취소, 그래픽 설정을 처리한다. |
| [PrototypeWorld.cpp](../../SimpleGame/src/app/PrototypeWorld.cpp) | `ToggleSystemMenu`, `SystemMenuMouseButton` | 메뉴를 열고 닫으며 스킵 후 위치·카메라·공격·발사체를 갱신한다. |
| [GameSessionSkipping.cpp](../../SimpleGame/src/app/GameSessionSkipping.cpp) | `CanSkipChapter1`, `CanSkipChapter2FieldStory`, `SkipStory` | 선행 조건·중복 실행 차단, 목적지 확인, 스토리 적 정리, 지역 전환·회복·체크포인트를 연결한다. |
| [ChapterSkipping.cpp](../../SimpleGame/src/chapters/ChapterSkipping.cpp) | `Chapter1::SkipStory`, `Chapter2::SkipFieldStory` | C01 완료와 C02 회색벽·협곡 완료를 각각 적용한다. C02는 선택 순서를 보존하고 기존 결과 산식을 재사용한다. |

## 2026-10-07 필드 자동 스폰 수정

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [FieldPopulation.cpp](../../SimpleGame/src/combat/FieldPopulation.cpp) | `Populate` | 실내 중심 구역을 제외하고 야외 구역 외곽 320까지 80 간격으로 후보를 찾는다. 기존 안전 조건과 무리 간격·상한을 적용한다. |
| [OpenWorldVerification.cpp](../../SimpleGame/src/verification/OpenWorldVerification.cpp) | `RunOpenWorldVerification` | 실제 네 야외 지역의 최초 필드 생성·상한·금지 지형을 추가 검사한다. 실행하지 않았다. |

## 2026-10-07 스테이지 순찰·성능 검증

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [EnemyPatrol.h](../../SimpleGame/src/combat/EnemyPatrol.h) | `Detect`, `Update`, `DistanceSquared` | 0.2초 시야 검사, 짧은 인식·추격 포기 조건, 생성 위치 주변 순찰과 대기를 처리한다. |
| [EnemyStateMachine.cpp](../../SimpleGame/src/combat/EnemyStateMachine.cpp) | `Update`, `Transition`, `Chase` | Patrol/Chase/Return 연결, 전이 시 이전 경로 무효화, A* 복귀와 공격을 담당한다. |
| [FieldPopulation.cpp](../../SimpleGame/src/combat/FieldPopulation.cpp) | `Populate`, `Update` | 진행 도로 주변에 무리를 생성하고 초당 단일 적 순회로 생존 상태를 집계한다. |
| [OpenWorldVerification.cpp](../../SimpleGame/src/verification/OpenWorldVerification.cpp) | `VerifyPatrol`, `RunEnemyBenchmark` | 순찰·감지·차폐·복귀 회귀와 고정 조건 AI CPU 측정을 수행한다. Release/Debug 회귀 통과. |
| [SimpleGame.cpp](../../SimpleGame/src/app/SimpleGame.cpp) | `Timer`, `main` | 활성/비활성 갱신 간격과 자동 렌더 측정 CLI를 연결한다. |

## 2026-10-07 오크 사망 모션 복구

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [EnemySystem.h](../../SimpleGame/src/combat/EnemySystem.h) | `FieldEnemy::DeathAnimationPlaying` | 공통 0.8초 사망 재생 여부를 표시와 슬롯 재사용에 제공한다. |
| [EnemySystem.cpp](../../SimpleGame/src/combat/EnemySystem.cpp) | `Update`, `Add` | 사망 AI 없이 재생 시간만 완료하고, 진행 중 시체 슬롯을 보호한다. |
| [Chapter2View.cpp](../../SimpleGame/src/chapters/Chapter2View.cpp) | `EnemyVisible` | 비보존 오크도 사망 재생 중에는 표시한다. |
| [OpenWorldVerification.cpp](../../SimpleGame/src/verification/OpenWorldVerification.cpp) | `VerifyOrcDeathAnimation` | 일반/스크립트 및 시체 보존 네 조합의 시간·표시·재사용 회귀를 검사한다. |

## 2026-10-08 목표 안내·전투 판독성 및 자동 플레이

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [CombatCueView.cpp](../../SimpleGame/src/ui/CombatCueView.cpp) | `DrawArea`, `DrawEnemy` | 실제 지면 투영 배율의 공격 반경, 준비 진행 막대와 공격/사격 문구를 그린다. |
| [HudTextLayout.h](../../SimpleGame/src/ui/HudTextLayout.h) | `Fit` | UTF-8 문자 경계를 보존하며 글자 너비에 맞춰 문장을 축약한다. |
| [EnemySystem.h](../../SimpleGame/src/combat/EnemySystem.h) | `AttackRange`, `AttackWindupDuration` | 일반 적 AI와 표시가 같은 공격 거리와 준비 시간을 사용하도록 제공한다. |
| [ChapterGoals.cpp](../../SimpleGame/src/chapters/ChapterGoals.cpp) | `Chapter1::Goal`, `Chapter2::Goal` | 농가 전투 중심, 장치/방어 입력, 보스 약화 전후 목표를 구분한다. |
| [Chapter1View.cpp](../../SimpleGame/src/chapters/Chapter1View.cpp), [Chapter2View.cpp](../../SimpleGame/src/chapters/Chapter2View.cpp) | `InteractionPrompt` | 지점별 Space 확인, Space 유지, Ctrl 공격과 미충족 조건을 안내한다. |
| [GameSessionWorld.cpp](../../SimpleGame/src/app/GameSessionWorld.cpp) | `LandmarkPrompt`, `TrackedGoal` | 지점 소유 챕터의 입력 안내와 목표 위치 보정/제목/행동을 월드 화면에 연결한다. |
| [PrototypeWorld.cpp](../../SimpleGame/src/app/PrototypeWorld.cpp) | `Draw` | 다른 지점의 오래된 설명을 숨기고, 가까운 공격 준비 중에는 말풍선보다 위험 표시를 우선한다. |
| [FieldPopulation.cpp](../../SimpleGame/src/combat/FieldPopulation.cpp) | `Populate`, `Spawn` | 지역 정의의 필드 체력을 초기 생성·재생성 모두에 적용한다. |
| [VisualPlaytest.cpp](../../SimpleGame/src/verification/VisualPlaytest.cpp) | `RunVisualPlaytest`, `Walk`, `CaptureBoss`, `CombatTrial`, `Save` | 숨겨진 WGL 창에서 실제 이동/공격 갱신, 보스 상태 픽스처, 화면 캡처와 조건별 CSV를 만든다. |
| [OpenWorldVerification.cpp](../../SimpleGame/src/verification/OpenWorldVerification.cpp) | `VerifyCombatGuidance` | 농가 목표, 실제 입력 안내, 필드 오크 체력과 근접/원거리 공격 경계 판정을 검사한다. |

## 2026-10-08 코드 기반 충격파·검 오라 시안

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [preview.js](../../output/procedural-effects-v1/preview.js) | `EffectsPreview::frame` | 재생 시계·UI와 두 효과의 그리기를 연결한다. |
| [shockwave.js](../../output/procedural-effects-v1/shockwave.js) | `Shockwave::draw` | 원형 파동·균열·솟아오르는 파편을 그린다. |
| [sword-aura.js](../../output/procedural-effects-v1/sword-aura.js) | `SwordAura::angle`, `blade`, `draw` | 검 각도·오라·과거 각도의 잔상을 그린다. |
| [drawing.js](../../output/procedural-effects-v1/drawing.js) | `line`, `fade` | 공통 선과 투명도 계산을 제공한다. |

게임 연결 및 실행 검증은 수행하지 않았다.

## 2026-10-08 기사 스프라이트 기반 검 이펙트 v2

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [motion.js](../../output/procedural-sword-v2/motion.js) | `poseAt`, `KnightMotion::blade`, `draw`, `drawOriginal` | 원본 공격 시간·자세, 수동 검날 연결점, 원본 효과 비교 표시를 관리한다. |
| [sword-effects.js](../../output/procedural-sword-v2/sword-effects.js) | `SwordEffects::point`, `ribbon`, `aura`, `groundLight`, `draw`, `particles`, `impact` | 기존 금빛 효과를 참고한 곡선 잔상·오라·입자·착지 섬광을 시간 기준으로 그린다. |
| [arena.js](../../output/procedural-sword-v2/arena.js) | `Arena::draw` | 캐시한 배경과 캐릭터 그림자·주변 입자를 표시한다. |
| [preview.js](../../output/procedural-sword-v2/preview.js) | `loadImage`, `SwordStudio::bindControls`, `draw`, `frame` | 이미지 로딩·비교 UI·재생 시계와 그리기를 연결한다. |

JavaScript 4개 파일 구문 검사 및 브라우저에서 두 궤적·원본 비교·색상·오라 제어 확인. 게임 빌드·실행은 하지 않았다.

## 2026-10-08 거친 단면의 검 이펙트 v3

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [sword-effects.js](../../output/procedural-sword-v3/sword-effects.js) | `SwordEffects::ribbon`, `draw`, `aura`, `impact` | 분리 파편 없이 연결된 두꺼운 궤적의 가장자리를 거칠게 그리고 오라·짧은 섬광·바닥 빛을 연결한다. |
| [pixel-geometry.js](../../output/procedural-sword-v3/pixel-geometry.js) | `PixelGeometry::polygon`, `segment`, `burst` | 4px 단위 다각형·선분·연결된 섬광을 그린다. |
| [preview.js](../../output/procedural-sword-v3/preview.js) | `SwordStudio::bindControls`, `draw`, `frame` | 원본 모션·비교 제어·시간과 새 효과를 연결한다. |

최신 사용자 지시로 비산 파편·깨진 조각을 제거했다. JS 5개 구문 검사와 브라우저 가로 베기·내려베기 확인 완료. 게임 연결·빌드·실행은 하지 않았다.


## 2026-10-08 로컬 JSON 월드 모니터링

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [PrototypeWorldMonitoring.cpp](../../SimpleGame/src/app/PrototypeWorldMonitoring.cpp) | `PrototypeWorld::CaptureMonitoring` | 선택 화면·메뉴·일시정지·패배를 포함한 월드 스냅샷을 수집한다. |
| [GameSessionMonitoring.cpp](../../SimpleGame/src/app/GameSessionMonitoring.cpp) | `GameSession::CaptureMonitoring` | 모든 지역의 NPC 관측값과 적을 읽고 월드 좌표 및 지역별 고유 ID를 만든다. 활성 적 저장소와 비활성 지역 저장소를 구분한다. |
| [ChapterMonitoring.cpp](../../SimpleGame/src/chapters/ChapterMonitoring.cpp) | `Chapter1::ObserveMap`, `Chapter2::ObserveMap` | 챕터 사건·실제 NPC 액터를 지역 상태와 현재 존재·행동 정보로 변환한다. |
| [MonitorSnapshot.cpp](../../SimpleGame/src/monitoring/MonitorSnapshot.cpp) | `MonitorJson::Serialize` | 스냅샷을 UTF-8 JSON schemaVersion 1로 직렬화한다. |
| [MonitorPublisher.cpp](../../SimpleGame/src/monitoring/MonitorPublisher.cpp) | `MonitorPublisher::Due`, `Publish`, `Run`, `Send` | 초당 수집 간격과 최신 1개 대기 메시지, 작업 스레드의 로컬 HTTP 전송을 담당한다. |
| [protocol.py](../../tools/monitor/protocol.py) | `SnapshotValidator.validate`, `SnapshotStore.accept`, `read` | 형식·범위·순서 검증, 현재 존재 NPC와 생존 몬스터 집계, 수신 상태 보관을 담당한다. |
| [server.py](../../tools/monitor/server.py) | `MonitorHandler.do_POST`, `do_GET`, `local_request`, `main` | 로컬 JSON 수신·조회와 HTML 제공을 연결한다. |
| [dashboard.js](../../tools/monitor/web/dashboard.js) | `WorldDashboard.poll`, `connection`, `render`, `renderRegions`, `renderEntities` | 수신 상태·지역 카드·NPC/몬스터 목록을 표시하고 검색한다. |

Python·JavaScript 구문, 프로젝트 XML·필터·파일 참조, 화면 ID 연결과 문서 링크를 정적으로 확인했다. 게임 빌드·회귀 검사·서버·브라우저 실행은 수행하지 않았다. 사용법과 JSON 규격은 [구현 문서](../../docs/implementation/LOCAL_WORLD_MONITOR.md)를 따른다.


## 2026-10-08 기사 스킬 게임 연결

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [KnightSkills.cpp](../../SimpleGame/src/combat/KnightSkills.cpp) | `KnightSkills::Cast`, `Update`, `Step`, `Reset`, `Casting`; `KnightSkillShot::Visible`, `Phase` | 한 번의 기사 동작에 시간차 찌르기 6개 또는 독립 검기를 생성하고, 프레임별 효과 범위와 이동 구간으로 실제 피해를 전달한다. |
| [KnightSkillView.cpp](../../SimpleGame/src/ui/KnightSkillView.cpp) | `KnightSkillView::DrawBody`, `DrawShot`, `DrawAura` | 원본 시트의 모션·효과·검 연결점 오라를 렌더링한다. |
| [PlayerProgression.cpp](../../SimpleGame/src/player/PlayerProgression.cpp) | `KnightAuraActive`, `EffectiveAttack`, `AttackRangeScale` | Lv.5 기사 패시브의 공격력 20%, 기본 공격·스킬 범위 30%를 공유 계산한다. |
| [PlayerSkills.cpp](../../SimpleGame/src/player/PlayerSkills.cpp) | `CanUse`, `HasPassive` | 기사 스킬 레벨·액티브 조건과 검기 상인 선행 조건을 검사한다. |
| [SkillSystem.cpp](../../SimpleGame/src/player/SkillSystem.cpp) | `BuildRequest` | 패시브 적용 공격력에 스킬 비율과 STR 항을 합산한다. |
| [GameSession.cpp](../../SimpleGame/src/app/GameSession.cpp) | `Strike`, `StrikeProjectile` | 패시브 기본 공격 범위와 임의 높이의 스킬 판정을 지역별 적·오브젝트·보상 경로로 전달한다. |
| [EnemySystem.cpp](../../SimpleGame/src/combat/EnemySystem.cpp) | `Strike`, `StrikeProjectile` | 확대 기본 공격, 효과 높이와 선택 시선 원점을 사용한 관통·대상당 중복 방지를 처리한다. |
| [Chapter1.cpp](../../SimpleGame/src/chapters/Chapter1.cpp) | `StrikeObjects`, `StrikeProjectileObjects` | 확대 기본 공격과 기사 스킬의 지휘종 피해 및 파괴 이벤트를 처리한다. |
| [Chapter2Combat.cpp](../../SimpleGame/src/chapters/Chapter2Combat.cpp) | `StrikeObjects`, `StrikeProjectileObjects` | 확대 기본 공격과 스킬의 지휘 인장 판정을 처리한다. |
| [Renderer.cpp](../../SimpleGame/src/rendering/Renderer.cpp) | `ImageQuad`, `CharacterAttack`, `CombatVisual` | 캐시 PNG의 UV 영역 선택, 캐릭터 크기를 보존한 효과 전용 배율을 제공한다. |
| [KnightSkillVerification.cpp](../../SimpleGame/src/verification/KnightSkillVerification.cpp) | `RunKnightSkillVerification` | --verify-knight-skills 수동 실행용 레벨·MP·쿨타임·피해·패시브·시간차·수명·범위·관통 회귀 검사를 제공한다. 이번 작업에서는 실행하지 않았다. |

사용자 확정 수치와 구현 해석은 [기사 스킬 런타임](../../docs/implementation/KNIGHT_SKILLS.md)에 기록했다. 정적 서식·파일·프로젝트·프레임 범위·PNG 복사·문서 링크 확인만 수행했으며 빌드·게임·회귀 검사 실행은 하지 않았다.


## 2026-10-08 보행 오라 기준점 수정

- [KnightSkillView.cpp](../../SimpleGame/src/ui/KnightSkillView.cpp)의 `KnightSkillView::DrawAura`: 실제 이동 시트의 행·열에 맞는 8개 검 연결점을 선택한다. 첫 프레임 좌표 재사용으로 아래 행에서 9.36px 내려가던 오차를 제거했다. 정적 연결점 수직 범위는 1.008px이며 원본 검의 프레임별 움직임은 유지한다. 서식·좌표·공백 정적 확인만 수행했고 빌드·게임 실행은 하지 않았다.


## 2026-10-12 천단참 궁극기

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [CheondanView.cpp](../../SimpleGame/src/ui/CheondanView.cpp) | `CheondanView::DrawCast`, `DrawWave` | 검기 흡수·성장 검·연속된 거친 잔상·지연 폭발과 독립 충격파를 텍스처 없이 도형으로 그린다. |
| [KnightSkills.cpp](../../SimpleGame/src/combat/KnightSkills.cpp) | `KnightSkillShot::Bounds`, `Duration`; `KnightSkills::Cast`, `Casting`, `Step` | 궁극기 지연 발동, 위로 높이 5배·거리 2배, 최종 충격파만 단일 관통 피해, 공유 표시/충돌 범위를 처리한다. |
| [KnightSkillView.cpp](../../SimpleGame/src/ui/KnightSkillView.cpp) | `DrawBody`, `DrawShot`, `DrawAura` | 천단참 전용 직접 그리기로 분기하며 기존 오라 중복을 막는다. |
| [KnightSkillVerification.cpp](../../SimpleGame/src/verification/KnightSkillVerification.cpp) | `RunKnightSkillVerification` | 천단참 Lv.20·MP30·60초·피해·지연 발동·위쪽 5배·거리 2배·초기화 회귀 코드를 추가했다. 실행 미수행. |

[천단참 구현](../../docs/implementation/CHEONDAN_ULTIMATE.md)에 수치와 연출 시간·범위 해석을 기록했다. 서식·프로젝트·필터·파일·문서 링크·구분자 정적 확인 완료. 빌드·게임·회귀·화면 품질·성능 검증은 실행하지 않았다.


## 2026-10-13 마법과 검의 노래와 OpenGL 효과 v2

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [MageUltimate.cpp](../../SimpleGame/src/combat/MageUltimate.cpp) | `MageUltimate::Cast`, `Update`, `Reset`; `PulseTime` | 고정 중심·피해 스냅샷과 4초 40회 범위 타격, 합체 폭발 1회, 사망·제어 잠금 취소를 관리한다. double 시간과 정수 순번으로 지나간 타격 시점을 처리한다. |
| [EnemyAreaAttack.cpp](../../SimpleGame/src/combat/EnemyAreaAttack.cpp) | `EnemySystem::StrikeArea` | 지면 원 안의 살아 있는 적을 타격하고 기존 최소 체력 보호를 적용하며 사망 위치를 한 번 반환한다. |
| [GameSessionAreaAttack.cpp](../../SimpleGame/src/app/GameSessionAreaAttack.cpp) | `GameSession::StrikeArea` | 활성·비활성 지역 좌표 변환과 기존 경험치·전리품 경로를 연결한다. |
| [SkillSystem.cpp](../../SimpleGame/src/player/SkillSystem.cpp), [SkillLayout.h](../../SimpleGame/src/player/SkillLayout.h) | `BuildRequest`, `IconAt`; `SkillLayout::Create` | 검 세례·폭발 피해를 따로 계산하고 일곱 번째 마법사 스킬을 동일한 4열 카드 표시·입력 배치로 처리한다. |
| [MageUltimateView.cpp](../../SimpleGame/src/ui/MageUltimateView.cpp) | `MageUltimateView::DrawFloor`, `DrawSky`, `DrawCaster` | 지면 마법진·실제 판정 경계, 상공 마법진·장식 검·합체·폭발, 기존 캐릭터 모션을 분리한다. 새 효과 실패 시 도형 대체 경로를 사용한다. |
| [ArcaneEffect.cpp](../../SimpleGame/src/rendering/ArcaneEffect.cpp), [셰이더](../../SimpleGame/src/rendering/ArcaneEffectShaders.h) | `ArcaneEffect::Initialize`, `Draw`, 소멸자; GLSL `seal`, `convergence`, `explosion` | 복사 불가 GPU 자원 소유, 1회 프로그램 준비, 제한 영역·가산 합성 및 복원. 거리 함수 룬, 나선 응축, 18회 깊이 누적 발광 폭발을 계산한다. |
| [RendererArcane.cpp](../../SimpleGame/src/rendering/RendererArcane.cpp), [PostProcessing.cpp](../../SimpleGame/src/rendering/PostProcessing.cpp) | `Renderer::DrawArcane`, `Begin`; `PostProcessing::Composite` | 기존 배치를 먼저 제출하고 같은 HDR 장면에 효과를 그린다. 매 프레임 충격파를 초기화하며 UI 이전에 배경 굴절을 합성한다. |
| [MageUltimateVerification.cpp](../../SimpleGame/src/verification/MageUltimateVerification.cpp) | `RunMageUltimateVerification` | 레벨·MP·쿨타임·피해·원형 경계·타격 순번·시간 경계·폭발·취소 수동 회귀 소스를 제공한다. 이번 작업에서는 실행하지 않았다. |
| [export_arcane_preview.py](../../tools/effects/export_arcane_preview.py) | `ArcanePreviewExporter.export` | 게임 효과 셰이더를 WebGL2 문법으로 내보내고 기존 캐릭터 PNG를 변경 없이 데이터 URL로 포함한다. |
| [gpu-preview.js](../../output/mage-sword-song-v1/gpu-preview.js), [preview.js](../../output/mage-sword-song-v1/preview.js) | `ArcaneGpuPreview::compose`, `pass`, `drawEffect`, `dispose`; `SwordSongPreview::draw`, `frame` | 동일 셰이더의 로컬 미리보기, HDR/블룸/굴절, 프레임버퍼 피드백 방지, Canvas 대체 경로, 시간 탐색과 배속을 연결한다. |

[구현 안내](../../docs/implementation/MAGE_SWORD_SONG.md)에 사용자 확정값·연출 선택·모듈 흐름·한계를 기록했다. C++ 서식, JavaScript/Python 구문, 프로젝트·필터 등록, 셰이더 내보내기 일치, 기존 캐릭터 바이트 일치, 문서·HTML 참조를 정적으로 확인했다. 게임 빌드·드라이버 셰이더 컴파일·게임/미리보기 재생·회귀·화면·성능 확인은 수행하지 않았다.

## 2026-10-15 챕터 3 이름 없는 일곱째

| 파일 | 함수 | 역할 |
| --- | --- | --- |
| [Chapter3.cpp](../../SimpleGame/src/chapters/Chapter3.cpp) | `Chapter3::Interact`, `ImportPrevious`, `ExportFlags`, `Retry` | S01~S10 선행 조건·중복 방지·정치 결과를 관리하고 C02 피해를 수신한다. 조사 보존과 전투 재도전을 구분한다. |
| [Chapter3Gameplay.cpp](../../SimpleGame/src/chapters/Chapter3Gameplay.cpp) | `Update`, `UpdateResearch`, `UpdateInfiltration`, `UpdateInterception`, `Hold`, `Goal` | 이동 추격·구조 복원·필체 순서·순찰 회피/제압·실제 운반대 차단·패링·비살상 제압과 동적 목표를 처리한다. |
| [Chapter3Map.cpp](../../SimpleGame/src/chapters/Chapter3Map.cpp) | `BuildMap`, `MemorialMap`, `CityMap`, `ArchiveMap` | 세 지역의 지형·도로·상호작용·위장 상자·정치 변화 지점을 구성한다. |
| [Chapter3View.cpp](../../SimpleGame/src/chapters/Chapter3View.cpp) | `DrawLandmark`, `AddActors`, `DrawActor`, `DrawEnemy`, `ObserveMap` | 도형으로 인물·운반대·경고·사본 출발·봉인·명패 철거·제한 보급을 표시하고 지역 상태를 관측한다. |
| [GameSessionWorld.cpp](../../SimpleGame/src/app/GameSessionWorld.cpp) | `OpenWorld`, `StoryChapter`, `TrackedGoal`, `StoryReady`, `SynchronizeChapterHistory` | 9개 지역·도보 길·C03 목표·선행 잠금·미리보기 결과 전달을 연결한다. |
| [Chapter3Verification.cpp](../../SimpleGame/src/verification/Chapter3Verification.cpp) | `RunChapter3Verification` | --verify-chapter3 수동 실행용 진행 순서·중복 보상·생존 보호·재도전·분산 보존·최종 상태·세계 등록 검사를 제공한다. 이번 작업에서는 실행하지 않았다. |

C++17 std::array는 고정 경로/시계, std::bitset은 조사 완료/중복 방지, std::optional은 미선택 보관처/목표를 나타낸다. [구현·조작·한계](../../docs/implementation/CHAPTER_03_IMPLEMENTATION.md)를 참고한다. 게임 빌드·실행·회귀 실행은 하지 않으며 정적 검토만 수행한다.

## 챕터 3까지 스토리 스킵 — 2026-10-20

- `GameSession::CanSkipChapter2`, `CanSkipChapter3` ([GameSessionSkipping.cpp](../../SimpleGame/src/app/GameSessionSkipping.cpp)): 오픈월드·선행 챕터 완료·미리보기 시작·대상 미완료 조건으로 전체 스킵 가능 여부를 판단한다.
- `GameSession::SkipChapter2`, `SkipChapter3`, `SkipStory`: `StorySkip`으로 요청을 구분하고 목적지 검증, 챕터 상태 변경, 스토리 전투만 퇴장, 지역 활성화와 이전 결과 전달, 체크포인트 및 체력·마나 회복을 연결한다.
- `Chapter2::SkipStory`, `Chapter3::SkipStory` ([ChapterSkipping.cpp](../../SimpleGame/src/chapters/ChapterSkipping.cpp)): 기존 선택을 보존하며 전체 완료 플래그와 임시 전투·조작 상태를 정리한다. 챕터 3 미수령 보급은 지급 없이 처리한다.
- `SettingsMenu::Buttons`, `Activate`, `Draw`: 챕터 2 전체·챕터 3 스킵 버튼과 확인 문구를 제공한다. `PrototypeWorld`가 명령을 세션에 전달하고 위치·카메라·공격·발사체를 정리한다.
- `VerifySkipping` ([Chapter3Verification.cpp](../../SimpleGame/src/verification/Chapter3Verification.cpp)): 순차 진행·선택 보존·무보상·반복 차단·미리보기 경로의 회귀 검사 소스. 실행하지 않았다.

## 챕터 3 도시 구획·기록관 포탈 — 2026-10-20

| 함수·클래스 | 책임과 연결 |
| --- | --- |
| `Chapter3Architecture::DressCity`, `EncloseRoom`, `DressReadingRoom` ([Chapter3Architecture.cpp](../../SimpleGame/src/chapters/Chapter3Architecture.cpp)) | 도시 구획·분리 건물·성벽·포탈을 배치하고 실내를 폐쇄 외벽으로 둘러싼다. 긴 벽은 청크에 맞춰 분할한다. |
| `Chapter3::ReadingRoomMap`, `BuildMap` ([Chapter3Map.cpp](../../SimpleGame/src/chapters/Chapter3Map.cpp)) | 공개 열람실과 기록관 실내 지도를 구성한다. 필사본과 세라드 통보는 열람실에 둔다. |
| `MapLayout::BuildingBlocked`, `DistrictAt` ([MapLayout.cpp](../../SimpleGame/src/world/MapLayout.cpp)) | 건물 발판 영역 충돌과 도시 구획 탐색을 제공한다. `LevelMap::Blocked`·소품 생성·지역명과 바닥 캐시에 연결된다. |
| `LevelMap::ChunkObjects` ([LevelMap.cpp](../../SimpleGame/src/world/LevelMap.cpp)) | 개별 건물을 앞쪽 발선에 정적 액터 종류 13으로 등록한다. |
| `WorldAtlas::Build` ([WorldAtlas.cpp](../../SimpleGame/src/world/WorldAtlas.cpp)) | 지역 건물·구획을 세계 좌표로 변환하고 포탈 실내 사이의 자동 연결 도로를 제외한다. |
| `BuildingView::Draw` ([BuildingView.cpp](../../SimpleGame/src/ui/BuildingView.cpp)) | 충돌과 같은 footprint를 기준으로 건물 외관·성벽·서가·닫힌 문·이름판을 따로 그린다. SceneRenderer가 호출한다. |
| `GameSession::TravelThroughPortal`, `PortalGoal` ([GameSessionPortals.cpp](../../SimpleGame/src/app/GameSessionPortals.cpp)) | 등록 포탈·도착 충돌 검증 후 지역을 바꾸며, 실내 목표에 필요한 출입 포탈을 먼저 안내한다. |
| `Chapter3::Interact`, `GameSession::ApplyPendingTravel` | 챕터 단계 잠금 확인 → 이동 요청 → 등록 포탈 검증 → 지역 활성화 순서로 연결한다. 도시 복귀는 조사·선택·보상 상태를 초기화하지 않는다. |
| `VerifyArchitecture` ([Chapter3Verification.cpp](../../SimpleGame/src/verification/Chapter3Verification.cpp)) | 포탈 잠금·왕복·진행 보존·개별 건물 충돌·도착 지점·목표 안내 회귀 소스. 빌드·실행하지 않았다. |

## 2026-10-20 Blender 필살기 검

| 영역 | 선언·구현 | 함수 | 코드에서 확인한 역할 |
| --- | --- | --- | --- |
| 필살기 검 | [MageUltimateSwordView.h](../../SimpleGame/src/ui/MageUltimateSwordView.h) | `Draw` | Blender 16프레임 시트의 회전 프레임·검끝 피벗·상하 반전·기울기·청색 픽셀 잔상을 표시한다. 피해 판정은 기존 궁극기에 맡긴다. |
| 필살기 연출 | [MageUltimateView.cpp](../../SimpleGame/src/ui/MageUltimateView.cpp) | `DrawSky` | 기존 타격 시간과 상하 마법진 위치로 검의 가속 이동을 계산해 전용 검 뷰에 전달한다. |

### Chapter3::DrawActor — 용사 스프라이트 복구 (2026-10-20)
- actor 11은 Chapter1Sprites::Draw의 Hero 프레임 0을 사용한다. 기존 hero-v1.png를 높이 52, 발 위치 기준으로 표시하며 도형 인물로 대체하지 않는다. Chapter3가 보유한 캐시가 텍스처 수명을 관리한다.

## 2026-10-20 용사·플레이어 인물 도트 v2

- 위 용사 복구 기록의 자산 경로는 이번 재제작으로 갱신된다. C01/C02/C03 공통 용사는 생성 헤더가 지정한 `NPC/Hero/PixelRestyle-v2/hero-v2.png`를 사용한다. 캐시 수명과 발 기준 렌더링은 유지한다.
- [pixel_restyle.py](../../SimpleGame/Assets/Characters/pixel_restyle.py)의 `SheetInspector.inspect/write`: PNG의 RGBA는 수정하지 않고 알파 실루엣·투명 간격·프레임 경계·발 피벗·행별 배율·마법사 장갑 위치를 추출해 `frames.json`과 용사/기사 앵커 공통 헤더를 만든다.
- 같은 파일의 `PixelRestyle.apply`: 기존 8개 클립의 몸체만 새 시트에 대응하고 기존 몸체 시간·독립 효과를 유지한다. 마법 효과는 새 장갑 보석 위치를 따라간다. `export-combat-visuals.py`가 호출하여 게임·미리보기·배포 데이터를 갱신한다.
- [KnightSkillView.cpp](../../SimpleGame/src/ui/KnightSkillView.cpp)의 `SkillBodyTime`: 기존 스킬 준비/타격/회복 시간을 새 기사 검격 4자세에 대응한다. `DrawBody`는 새 몸체를 사용하고 피해 로직은 유지한다.
- 같은 파일의 `SpriteHilt`: 현재 이미지 경로·프레임 좌표를 새 픽셀 기준 칼자루 표와 대조하고 몸체 피벗·배율로 검 오라 위치·각도를 계산한다. 게임 화면 정밀 검증은 미실행이다.
- [최종 자산·재제작·검토 범위](../../SimpleGame/Assets/Characters/PixelRestyle-v2/README.md). 미채택 v1은 로컬 제외 경로에 보관하며 게임에서 참조하지 않는다.

## 2026-10-26 사용자 선택 이동 시트 v3

- [selected_walk.py](../../SimpleGame/Assets/Characters/selected_walk.py)의 `SelectedWalk.apply/write_header`: 승인된 투명 이동 시트와 수동 몸통 중심·발바닥 피벗을 읽고 기사·마법사 `*-walk`만 교체한다. 기존 8개 시간 슬롯을 4자세에 두 슬롯씩 대응하며 공격 6종은 수정하지 않는다. `export-combat-visuals.py`에서 v2 공격 연결 뒤 호출하고 용사 이동·기사 칼자루 공통 헤더를 생성한다.
- [HeroWalkView](../../SimpleGame/src/rendering/HeroWalkView.h)의 `Load/Draw`: 이동 전용 텍스처 수명을 소유하고 고정 행 배율과 프레임별 피벗으로 용사를 표시한다. C01/C03 대기는 첫 자세, C02 경로 이동 중에는 180ms 간격으로 4자세를 사용한다. 공격은 기존 챕터 시트 호출을 유지한다.
- `Chapter2::Update/UpdateHero`: 매 갱신의 `heroMoving`을 초기화하고 실제 경로 전진 때만 켜서 제자리 걸음을 막는다. 경로 탐색·이동 속도·공격 판정은 유지한다.
- `KnightSkillView::SpriteHilt`: 새 이동 시트 경로와 좌표를 먼저 대조해 이동 중 검 오라가 기존 시트의 칼자루 좌표를 사용하지 않도록 한다. 기존 공격 칼자루 표는 유지한다.
- 최종 경로와 검토 결과는 [이동 시트 문서](../../SimpleGame/Assets/Characters/Walk/Selected-v3/README.md)에 기록했다. 위 과거 기록의 미채택 v1 보관 폴더는 이번 사용자 정리 지시로 삭제했다.
- 색감·도트 후속 수정: 사용자의 정정에 따라 RGB 0.85 배율과 런타임 팔레트 처리를 제거했다. imagegen으로 이동 PNG의 피부·머리색을 공격 시트 기준으로 바꾸고 픽셀 덩어리·계단형 윤곽을 강화했다. `Renderer::Sprite`는 기존 흰색 정점 색상을 사용하며 재생 중 색상 보정 코드는 없다. 새 픽셀 경계에 맞춘 12개 버클·지면 피벗을 생성 데이터로 다시 내보냈다.


## 2026-10-26 천단참 Blender 리메이크

- `tools/effects/blender/build_cheondan.py`: `CheondanStudio.build`가 응축·원호 참격·지연 잔상·연결 균열의 메시와 키프레임을 만들고 `.blend` 및 46장 투명 PNG를 저장한다. `material`은 조명 없는 발광 재질, `ribbon`은 테이퍼 메시, `visibility`는 프레임별 표시 구간을 담당한다.
- `tools/effects/blender/pack_cheondan.py`: `CheondanPacker.build`가 기준 기사 시트의 6색으로 양자화·이진 알파 변환 후 320×180 셀 시트와 GIF·단계 비교·설정을 저장한다. RGB 팔레트 포함 관계와 알파·유효 프레임을 검사한다. 게임 실행·런타임 연결은 하지 않는다.
- 제작물·출처·검토 범위: [천단참 Blender](../../SimpleGame/Assets/Effects/CheondanBlender-v1/README.md).

## 2026-10-26 천단참 궤적 일치·거울 파쇄 v2

- `tools/effects/blender/mirror_geometry.py`: `MirrorGeometry.tip`은 칼끝·잔상·자상의 공통 원호 좌표를 반환한다. `clip`은 다각형을 반평면으로 자르고 `cells`는 고정 난수 보로노이 거울판 69개를 반환한다. 셀 면적 합 144는 화면 면적과 일치한다.
- `tools/effects/blender/build_cheondan_mirror.py`: `MirrorCheondanStudio.sweep`은 공통 표본에 날·참격·자상을 놓는다. `polygon`은 평면 메시를 만들고 `shards`는 두께·반사 무늬가 있는 판을 나누어 이동·회전·확대 키프레임을 만든다. `build`는 Blender 원본과 60장 투명 렌더를 저장한다. v1 `CheondanStudio`의 재질·리본·가시성 함수를 상속한다.
- `tools/effects/blender/pack_cheondan_mirror.py`: `MirrorCheondanPacker.build`는 기사 기준 12색과 이진 알파의 480×270 셀 시트·GIF·단계 비교·설정을 저장하고 팔레트·알파·활성 구간을 검사한다.
- 현재 자산·검토 한계: [거울 파쇄 v2](../../SimpleGame/Assets/Effects/CheondanMirror-v2/README.md). 게임 미적용, 빌드·실행 미수행.

### 천단참 즉시 파쇄 후속 교정 (2026-10-26)

`MirrorCheondanStudio.shards`의 거울판 표시·정지 구간을 제거했다. 25프레임에 이미 분리·회전한 조각을 표시하며 45프레임까지 재생한다. `sweep` 자상은 24프레임에 끝난다. `build`와 `MirrorCheondanPacker.build`의 현재 출력은 50프레임이며 패커는 첫 파편 프레임의 불투명 점유율 80% 미만도 확인한다. 이전 설명의 60프레임은 교정 전 사양이다.
