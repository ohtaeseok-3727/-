# 챕터 구조와 추가 방법

2026-09-24 · `codex/chapter-extensibility`

## 먼저 볼 파일

| 파일 | 책임 |
| --- | --- |
| `SimpleGame/src/chapters/ChapterCatalog.cpp` | 플레이 가능한 챕터 ID와 생성 함수 등록. 새 챕터의 공통 연결 지점 |
| `SimpleGame/src/chapters/Chapter.h` | 챕터가 제공할 진행·맵·표시 기능과 이동 요청 규약 |
| `SimpleGame/src/app/GameSession.h/.cpp` | 플레이어 성장·인벤토리·적·전리품, 챕터 인스턴스 보관, 이동 검증·적용 |
| `SimpleGame/src/chapters/Chapter1.h` | 챕터 1의 상태·지역/오브젝트 ID·메서드 선언을 한 곳에서 확인 |
| `SimpleGame/src/chapters/Chapter1.cpp` | 챕터 1 이벤트·상호작용·전투·증원 진행 |
| `SimpleGame/src/chapters/Chapter1Map.cpp` | 챕터 1 맵의 좌표·입구·이름·지형·오브젝트 배치 |
| `SimpleGame/src/chapters/Chapter1View.cpp` | 같은 `Chapter1` 클래스의 그리기 구현. 별도 View 클래스가 아님 |
| `SimpleGame/src/chapters/Chapter1Sprites.h/.cpp` | 이미지 디코딩·프레임 영역·OpenGL 텍스처 수명·캐시 |
| `SimpleGame/src/world/MapLayout.h/.cpp`, `LevelMap.h/.cpp` | 챕터가 제공한 지도 데이터의 조회·충돌·배치 |
| `SimpleGame/src/ui/GameView.h/.cpp`, `MapView.h/.cpp` | 공통 HUD·인벤토리·지형·전체 지도 표시 |
| `SimpleGame/src/app/PrototypeWorld.cpp`, `SceneRenderer.cpp` | 입력·카메라·장면 갱신과 공통 인터페이스를 통한 그리기 |

별도 ChapterManager, ChapterFactory 클래스, 챕터별 State/Director/Encounters/Layout/View 클래스를 계속 늘리지 않는다. 작은 챕터는 헤더·구현 한 쌍으로 충분하다. 맵 배치나 그리기가 길어지면 **같은 클래스의 구현 파일**만 나눈다. 이미지 캐시는 별도의 자원 수명이 있어 유지했다.

## 실행 흐름

1. `GameSession` 생성 시 `ChapterCatalog::Register()`가 ID와 생성 함수를 등록한다. 아직 챕터를 생성하거나 이미지를 읽지는 않는다.
2. 시작 시 `TravelTo()`가 필요한 챕터만 만들고 `Reset()`을 호출한다. 챕터의 `InitialMap()`과 `BuildMap()`으로 초기 지도를 가져온다.
3. `GameSession::Update()`가 현재 챕터의 이벤트를 진행하고 공통 적·전리품을 갱신한다. 상호작용도 현재 챕터로 전달한다.
4. 챕터는 `RequestTravel()`로 이동을 예약한다. 월드는 `ApplyPendingTravel()`의 성공 결과를 받아 플레이어·카메라를 이동하고 공격·발사체·장면 캐시를 정리한다.
5. 맵이 바뀌면 세션이 맵과 챕터를 교체하고 적·전리품·상호작용 표시를 정리한다. 목표 챕터의 `EnterMap()`은 지역 진입 상태만 갱신한다.
6. 렌더러는 현재 챕터의 `DrawEnemy`, `DrawLandmark`, `DrawActor`, `DrawWeather`를 호출한다. 공통 월드에서 `Chapter1State`를 읽지 않는다.

새 게임용 `GameSession::Reset()`은 성장·소지품·모든 챕터 진행을 초기화한다. **다음 챕터로 이동할 때 호출하면 안 된다.** 일반 맵/챕터 전환은 레벨·경험치·현재 HP/MP·돈·장비를 초기화하지 않는다. 기본 공격·발사체만 중단하고 스킬 습득·등록 등 기존 플레이어 시스템은 유지한다.

## 새 챕터 추가 순서

1. 확정 스토리와 맵/몬스터 기획을 확인한다. `Chapter`를 상속하는 `Chapter2` 등을 만들고 필요한 상태와 이벤트를 그 클래스에 둔다. 작은 상태 구조체를 같은 헤더에 두어도 된다.
2. `InitialMap()`과 `BuildMap(MapId)`을 구현한다. `MapDefinition`에 제목·입구·지역·길·상호작용 오브젝트·전투 위치·물 사각형·선택적 실내 범위를 지정한다. 없는 맵 ID는 예외로 거부한다. `MapDefinition.id`는 요청한 ID와 일치해야 한다.
3. 진행 관련 필수 메서드와 `DrawEnemy()`를 구현한다. 추가 연출·날씨·전용 오브젝트가 없으면 기본 구현을 그대로 사용한다. `EnterMap()`은 `noexcept`이며 할당·이미지 로딩 같은 실패 가능한 작업 없이 상태만 갱신한다. 최초 진입과 재방문 모두 호출될 수 있다.
4. `ChapterCatalog.cpp`에 헤더 포함과 다음 등록을 추가한다.

```cpp
session.RegisterChapter("chapter2", []
{
    return std::make_unique<Chapter2>();
});
```

5. 이전 챕터의 확정된 출구 조건을 충족했을 때 이동을 요청한다.

```cpp
// 다른 챕터의 초기 맵으로 이동
RequestTravel({"chapter2", std::nullopt, std::nullopt});

// 같은 챕터의 특정 맵 입구로 이동
RequestTravel({{}, nextMapId, std::nullopt});

// 같은 맵 내부의 지정 좌표로 이동
RequestTravel({{}, currentMapId, WorldPosition{120.f, -80.f}});
```

6. 새 `.h/.cpp`를 `SimpleGame.vcxproj`와 `.filters`에 등록한다. PNG는 `SimpleGame/Assets` 아래에 두면 하위 경로를 유지해 빌드 출력 폴더로 복사된다. 실제 스프라이트 로딩·프레임 정의는 챕터에서 구현한다. 파일만 놓는 것으로 애니메이션이 자동 등록되지는 않는다.
7. 새 진행 분기·완료 조건과 이전 챕터에서 이어지는 경로의 검증을 추가한다. 빌드와 실행은 사용자 지침에 따라 사용자가 수행한다.

챕터 ID는 등록소의 고유 문자열이다. 맵과 오브젝트 ID는 **챕터 내부 정수**라 다른 챕터에서 같은 번호를 사용해도 된다. 상수는 `Chapter1Maps`, `Chapter1Objects`처럼 해당 챕터 헤더에서 관리한다. 새 챕터 때문에 공통 맵/오브젝트 enum에 항목을 추가할 필요가 없다. `MapRegion`과 `LandmarkKind`는 공통 지형 표현 종류이며 완전히 새로운 지형 표현은 별도 확장이 필요하다.

`EnemyStats`는 적의 체력·피해·이동 속도·감지 거리·최저 체력·시체 보존·챕터 내부 외형 번호를 제공한다. 기존 근접/원거리/치료/보스 AI는 재사용할 수 있다. 그르마의 최저 HP 1과 처치 연출용 시체 보존은 챕터 1 생성 코드가 설정하며, 다른 보스에 자동 적용되지 않는다. 새 행동 패턴은 해당 AI 확장이 필요하다.

## 전환 보장과 현재 범위

- 등록되지 않은 챕터, 잘못된 맵, 막힌 좌표, NaN/무한 좌표는 현재 맵을 교체하기 전에 거부한다. 실패한 예약 이동은 한 번 소비하고 안내한다.
- 같은 맵 안의 순간이동은 적·전리품·맵 revision을 초기화하지 않는다. 다른 챕터라면 맵 번호가 같아도 맵을 교체하고 revision을 올린다.
- 방문한 챕터의 객체와 진행 상태는 세션에 보관된다. 재방문 시 `Reset()`하지 않는다. 반면 맵별 살아 있는 적·바닥 전리품은 저장하지 않는다. 재방문 전투 정책은 챕터의 `EnterMap()`/진행 코드에서 정해야 한다.
- 이 보존은 실행 중 메모리 보존이다. 저장 파일·불러오기·체크포인트 시스템은 추가하지 않았다.
- 챕터의 `BuildMap()`은 지도를 만들 뿐 진행 상태를 바꾸지 않는 `const` 함수다. 전환 실패 전에 퀘스트 보상을 확정하는 등 되돌리기 어려운 변경은 새 챕터 작성 시 피한다.
- 현재 플레이 등록은 챕터 1만 있다. `Chapter2Route`는 기존 연결용 맵이며, 챕터 2의 스토리·오크 전투를 임의로 구현하지 않았다.
- 2026-09-24 후속 작업에서 C02를 일반 카탈로그에 연결하고 `--preview-chapter2` 호출 호환을 유지했다. C02 실제 전투·용사 협력·바르칸 패턴·v2 스프라이트를 연결했으며, 수치는 실행 후 조정 대상으로 남겼다. 상세 범위와 미정 사항은 `CHAPTER_02_IMPLEMENTATION.md`를 따른다.
- 기존 지역 재방문 규칙, 챕터 해금 UI, 개별 몬스터 보상표는 이번 공통 인터페이스가 자동으로 결정하지 않는다.

## STL과 검증

`std::unique_ptr`로 챕터 인스턴스와 수명을 한 곳에서 소유하고, `std::unordered_map`으로 ID에 맞는 생성 함수/인스턴스를 찾는다. `std::function`은 등록된 생성 함수를 보관하고 `std::optional`은 기본 입구 이동과 좌표 지정 이동을 구분한다. `MapDefinition`의 `std::vector`/`std::string`은 지역 데이터와 문자열을 직접 소유해 임시 데이터의 수명 문제를 피한다. MSVC v143의 C++17 기능을 사용한다.

기존 `--verify-chapter1` 진입점에 `GameSessionVerification.cpp`의 전환 검증을 연결했다. 등록 중복, 지연 생성, 같은 번호의 다른 챕터 이동, 순간이동, 예약 이동 1회 소비, 재방문 진행 보존, 플레이어 데이터 보존, 잘못된 이동 거부, 챕터 초기 맵, 보스 최저 HP, 새 게임 초기화를 검사한다. 기존 C01 전체 진행·접근성·기본 공격 검증도 새 공통 전환 경로를 사용한다.

이번 작업에서는 해당 검증 코드를 작성하고 정적으로 검토했으며 빌드·실행하지 않았다. 사용자가 빌드한 뒤 `SimpleGame.exe --verify-chapter1`로 실행할 수 있다. 화면에서는 직업 선택 → 마을/숲/폐광 이동 → 승강기 → 그르마 연출 → 연결용 맵, 스킬 발사체, 지도·인벤토리·사망 복귀를 별도로 확인한다.
