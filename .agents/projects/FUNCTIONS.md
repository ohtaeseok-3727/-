# 게임소프트웨어공학 실습 로컬 함수 사전

`python tools/knowledge/project_hub.py index-functions --project game-software`로 이 프로젝트의 헤더 선언을 로컬 DB에 색인하고, `find-function game-software "이름"`으로 검색한다. 아래 목록은 코드 탐색의 출발점이다. 실제 동작은 구현 파일과 관련 기획 문서에서 확인한다.

| 영역 | 선언·구현 | 함수 | 코드에서 확인한 역할 |
| --- | --- | --- | --- |
| 게임 세션 | [GameSession.h](../../SimpleGame/src/app/GameSession.h), [GameSession.cpp](../../SimpleGame/src/app/GameSession.cpp) | `ApplyPendingTravel` | 챕터가 요청한 이동을 꺼내고 요청을 지운 뒤 `TravelTo`를 호출한다. |
| 챕터 흐름 | [Chapter.h](../../SimpleGame/src/chapters/Chapter.h), [Chapter.cpp](../../SimpleGame/src/chapters/Chapter.cpp) | `PendingTravel`, `RequestTravel` | 보류 중인 이동 요청을 조회하거나 저장한다. |
| 전투 입력 | [BasicAttack.h](../../SimpleGame/src/combat/BasicAttack.h), [BasicAttack.cpp](../../SimpleGame/src/combat/BasicAttack.cpp) | `Press`, `Update`, `ConsumeHits` | 3단계 연속 공격 입력을 처리하고, 시간에 맞춰 타격을 기록한 뒤 소비자에게 넘긴다. |
| 적 동작 | [EnemySystem.h](../../SimpleGame/src/combat/EnemySystem.h), [EnemySystem.cpp](../../SimpleGame/src/combat/EnemySystem.cpp) | `Update` | 활성 적을 시간 간격에 맞춰 갱신하고 플레이어에게 들어오는 피해를 반환한다. |
| 스킬 | [SkillSystem.h](../../SimpleGame/src/player/SkillSystem.h), [SkillSystem.cpp](../../SimpleGame/src/player/SkillSystem.cpp) | `Update` | 남은 재사용 대기 시간을 줄이고 완료된 항목을 제거한다. |

프로젝트 코드가 바뀌면 선언과 구현을 대조해 이 표를 갱신한다.
