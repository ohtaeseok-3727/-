# 게임 소스 구성

[SimpleGame.vcxproj](../SimpleGame.vcxproj)는 아래 폴더의 C++ 파일을 상대 경로로 등록한다. 헤더는 프로젝트 파일에 지정한 `src/` 하위 폴더와 프로젝트 루트의 검색 경로에서 찾는다. 외부 라이브러리, 에셋, 셰이더 및 빌드 설정 파일은 게임 프로젝트의 기존 전용 폴더와 프로젝트 파일에 둔다.

| 폴더 | 책임 |
| --- | --- |
| `app/` | 프로그램 진입점, 게임 세션, 월드 업데이트 조립 |
| `chapters/` | 챕터 등록·상태·맵·연출 |
| `combat/` | 기본 공격, 적 AI와 전투, 전리품, 발사체 |
| `core/` | 공통 선행 컴파일 헤더와 플랫폼 설정 |
| `generated/` | 자동 생성 전투 시각 데이터 |
| `player/` | 성장·인벤토리·스킬 상태와 입력 연결 |
| `profiling/` | 프레임 계측과 성능 로그 |
| `rendering/` | 장면 그래프, 카메라, 렌더링, 후처리, 이미지·글자 처리 |
| `ui/` | 게임 HUD, 지도, 인벤토리, 스킬과 상호작용 표시 |
| `verification/` | 챕터·세션 검증 코드 |
| `world/` | 레벨 데이터, 지도 배치와 공통 좌표형 |

진입점은 `app/`의 `SimpleGame.cpp`다. `PrototypeWorld`가 세션, 챕터, 전투, 플레이어 상태를 갱신하고 렌더링·UI 모듈에 표시 데이터를 전달한다. 각 모듈의 선언을 다른 폴더에서 사용하는 경우 헤더 검색 경로를 통해 연결한다. 새 C++ 파일을 추가하면 프로젝트 파일과 필터에 상대 경로를 등록한다.

## 플레이어 행동 상태

`player/PlayerStateMachine.h`는 행동을 Active 아래 Locomotion(Idle, Moving)과 Action(BasicAttack, Casting)으로 분류하고, Casting 아래에 세 시전 종류를 둔다. Defeated는 별도 상태다. 일시정지·메뉴·이벤트 조작 잠금은 독립적인 입력 제한으로 취급한다. `PrototypeWorld::UpdatePlayerState`가 공격·스킬 진행 상태와 이동 결과를 반영하며 입력 허용과 `SceneRenderer`의 표시 선택에 같은 상태를 사용한다. 공격 연계와 스킬 타이밍은 기존 담당 객체가 소유한다. 상태 갱신은 해당 객체의 현재 진행 상태를 기반으로 하며, 전환 시 진입·종료 콜백은 없다.

## GLFW 창과 입력

`app/GlfwWindow`가 GLFW 창·컨텍스트와 이벤트 대기를 소유한다. `SimpleGame`은 GLFW 콜백을 게임 입력으로 연결하고 update/draw/swap 루프를 실행한다. 외부 의존성과 배포 범위는 [GLFW 안내](../Dependencies/GLFW/README.md)를 따른다.
