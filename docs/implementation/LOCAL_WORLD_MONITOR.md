# 로컬 JSON 월드 모니터링

## 사용자 요청과 구현 선택

2026-10-08 사용자 요청: 각 NPC 상태, 각 마을의 전체 상태, 몹·NPC 개수를 로컬 PC의 HTML에서 확인하며 JSON 메시지를 수신한다.

HTTP POST, 포트 8766, 1초 전송, 5초 연결 만료, Python 표준 라이브러리 수신기는 이번 구현의 선택이다. 사용자가 별도로 확정한 통신 규격이나 게임 설정으로 취급하지 않는다. 실행 방법은 [모니터 안내](../../tools/monitor/README.md)에 있다.

## 연결과 책임

```text
게임 Timer → PrototypeWorld::CaptureMonitoring
           → GameSession::CaptureMonitoring → Chapter::ObserveMap
           → MonitorJson::Serialize → MonitorPublisher 작업 스레드
           → HTTP POST /api/snapshot → SnapshotValidator → SnapshotStore
브라우저   → HTTP GET /api/state → WorldDashboard
```

| 파일·클래스 | 책임 |
| --- | --- |
| [PrototypeWorldMonitoring.cpp](../../SimpleGame/src/app/PrototypeWorldMonitoring.cpp) | 선택 화면·진행·메뉴·일시정지·패배 상태를 수집 |
| [GameSessionMonitoring.cpp](../../SimpleGame/src/app/GameSessionMonitoring.cpp) | 모든 등록 지역과 적을 읽고 좌표를 월드 좌표로 변환 |
| [ChapterMonitoring.cpp](../../SimpleGame/src/chapters/ChapterMonitoring.cpp) | 챕터 1·2의 사건과 실제 NPC 액터를 모니터용 관측값으로 변환 |
| [MonitorSnapshot.h](../../SimpleGame/src/monitoring/MonitorSnapshot.h) | 관측값의 공통 자료형 |
| [MonitorSnapshot.cpp](../../SimpleGame/src/monitoring/MonitorSnapshot.cpp) | UTF-8 JSON 직렬화, 문자열 이스케이프와 미정의 값 처리 |
| [MonitorPublisher.cpp](../../SimpleGame/src/monitoring/MonitorPublisher.cpp) | 전송 간격, 단일 최신 대기 메시지, WinHTTP 전송과 종료 정리 |
| [protocol.py](../../tools/monitor/protocol.py) | 형식·중복 ID·순서 검증, 집계 및 스레드 안전 상태 보관 |
| [server.py](../../tools/monitor/server.py) | 127.0.0.1 수신, 정해진 정적 파일과 조회 API 제공 |
| [dashboard.js](../../tools/monitor/web/dashboard.js) | 수신 상태·지역·NPC·몬스터 렌더링과 검색 |

활성 지역의 적은 RegionRuntime에서 `m_Enemies`로 이동되어 있으므로 활성 지역만 그 저장소를 읽는다. 다른 지역은 보관된 적을 읽는다. 챕터 인터페이스의 기본 관측값은 미확인이고, 챕터 1·2가 자신이 가진 상태를 해석한다. 월드 수집은 게임 스레드에서 수행하며 직렬화된 문자열만 작업 스레드로 넘긴다. 수집이 게임 진행 값을 변경하지 않는다.

C++17의 `std::optional`은 없는 체력·위치를 0과 구별한다. `std::vector`는 지역·개체 목록을 담고 `std::move`는 목록과 송신 버퍼 복사를 줄인다. `std::thread`, `std::mutex`, `std::condition_variable`은 통신 대기를 게임 루프에서 분리한다. `std::unique_ptr`의 사용자 정의 삭제자는 WinHTTP 핸들을 모든 반환 경로에서 정리한다. `std::chrono::steady_clock`은 PC 시계 변경과 무관한 간격 측정에 사용한다.

## 관측 의미

- NPC는 챕터 1의 배급소 주민·농부·포로 대표·용사 및 챕터 2 동행 용사다. 배경 시설·다리·장치는 NPC로 집계하지 않는다. 포로 대표 1개는 실제 포로 인구 수가 아니다.
- NPC `present=false`는 미등장·퇴장이다. 체력 자료가 없으면 `health`와 `maxHealth` 모두 `null`이다. 챕터별 용사 액터는 이야기상 같은 인물이더라도 다른 ID를 가진다.
- `settlement`는 Village 영역을 포함한 맵이다. 국경 마을·농경지와 회색벽·방어선의 맵 전체를 집계한다. 총 6개 등록 지역을 내보내며 발견 여부를 별도 제공한다.
- 지역 `status`는 챕터 사건의 진행 상태다. `stable`/`cleared`가 몬스터 0마리라는 뜻은 아니다. 몬스터 생존 개수와 함께 해석한다.
- 비활성 지역 값은 게임의 보관 상태다. 모니터가 AI 업데이트 범위나 스폰 규칙을 바꾸지 않는다. 몬스터 수는 등록된 생존 개체이며 향후 스폰 예정 수나 누적 처치 수가 아니다.

## JSON 규격 1

UTF-8 `application/json`을 `http://127.0.0.1:8766/api/snapshot`에 POST한다. 매번 전체 월드를 교체한다. 아래는 규격 설명용 예시이며 기본 화면에 주입하지 않는다.

```json
{
  "schemaVersion": 1,
  "type": "world.snapshot",
  "sessionId": "example-process",
  "sequence": 1,
  "sentAtUnixMs": 1791417600000,
  "game": {
    "status": "running",
    "chapter": "chapter1",
    "playerPosition": {"x": 10, "y": 20}
  },
  "regions": [{
    "id": "chapter1:0",
    "name": "예시 마을권",
    "kind": "settlement",
    "active": true,
    "discovered": true,
    "status": "watch",
    "detail": "상황 확인 전",
    "npcs": [{
      "id": "chapter1:0:npc:1",
      "name": "예시 주민",
      "role": "civilian",
      "state": "waiting",
      "detail": "대기 중",
      "present": true,
      "position": {"x": 30, "y": 40},
      "health": null,
      "maxHealth": null
    }],
    "monsters": [{
      "id": "chapter1:0:monster:100",
      "role": "melee",
      "state": "patrol",
      "position": {"x": 100, "y": 120},
      "health": 45,
      "maxHealth": 45,
      "scripted": false
    }]
  }]
}
```

- `sessionId`: 프로세스별 비어 있지 않은 문자열. `sequence`: 같은 세션에서 단조 증가하는 양의 정수. 동일·과거 순서는 거절한다. 최근 세션 32개의 순서를 보관한다.
- `sentAtUnixMs`: Unix 밀리초. 연결 만료는 송신 PC의 이 값이 아니라 서버의 단조 증가 수신 시각으로 판단한다.
- `game.status`: `selecting`, `running`, `paused`, `menu`, `defeated`. `selecting`이면 `regions=[]`로 이전 개체를 제거한다.
- `region.kind`: `settlement` 또는 `region`. 동시에 활성 지역은 최대 1개다.
- 모든 지역 ID는 고유하며 NPC·몬스터 ID도 메시지 전체에서 고유하다. 64비트 몬스터 ID는 브라우저 정밀도 손실을 피하기 위해 문자열로 전송한다.
- 위치는 월드 단위의 유한한 x/y 또는 `null`이다. 절댓값 최대 10억, C++ 송신 소수점 2자리. NPC 체력은 모두 `null`이거나 `0 <= health <= maxHealth`, `maxHealth > 0`인 정수다. 몬스터는 정수 체력이 필수다.
- 최대 2 MiB, 지역 64개, 지역당 각 종류 10,000개, 총 개체 20,000개. 초과 메시지는 부분 적용하지 않는다. 문자열 길이도 수신기에서 제한한다.

정상 POST는 `200 {"accepted":true,"sequence":1}`을 반환한다. 잘못된 형식·순서·활성 세션 충돌은 400, 본문 크기 위반은 413, Content-Type 위반은 415다. 거절 시 마지막 정상 값과 수신 시각을 유지한다. 다른 게임 세션은 이전 수신 후 5초 이상 지났을 때만 수용한다.

`GET /api/state`는 `connected`, `ageSeconds`, `receivedAtUnixMs`, `snapshot`을 반환한다. 첫 메시지 전에는 `snapshot=null`이다. 수신기는 각 지역에 `counts`, 스냅샷에 `totals`를 계산해 추가한다. 이들은 `npcs`(현재 존재), `npcTracked`(부재 포함 등록), `monsters`(생존), `monsterDead`(사망 보관)다. 송신자가 제공하는 합계는 신뢰하지 않고 다시 계산한다.

서버는 루프백에만 바인딩하고 같은 주소의 Host·Origin만 허용한다. CORS는 제공하지 않는다. UI는 수신 문자열을 `textContent`로 표시한다. JSON 중복 키·NaN·Infinity를 거절하고 프록시·리디렉션·쿠키 없이 로컬 주소로만 전송한다. 저장 파일·이력 DB·외부 전송은 없으며 서버 종료 시 최신 상태는 사라진다.

## 비용과 검증 범위

게임 수집·직렬화는 약 1초마다 O(지역 + 개체)이며 게임 스레드에 작은 추가 비용이 있다. 대기 메시지는 최신 1개만 보관하므로 미수신 큐가 계속 늘지 않는다. WinHTTP 각 단계 제한은 250ms이고 통신은 작업 스레드에서 수행한다. 이 값이 전체 요청이나 종료 시간을 250ms로 보장하는 것은 아니다. 서버가 없어도 다음 스냅샷에서 재시도한다.

기존 벤치마크 렌더 실행에서는 전송하지 않으며 기존 성능 로그 schema 2는 변경하지 않았다. 새로운 JSON 채널은 별도 schemaVersion 1이다. 단위·비용 및 미측정 범위는 [성능 기록](../performance/PERFORMANCE_ANALYSIS.md)에 남긴다.

이번 작업은 Python·JavaScript 구문과 Visual Studio 등록·참조 등 정적 검토만 수행했다. 빌드, 프로토콜 회귀 검사 실행, 실제 송수신·브라우저 외형·게임 성능 확인은 수행하지 않았다. [회귀 검사](../../tools/monitor/test_protocol.py)는 현재 존재/사망 집계, 잘못된 입력 보존, 수신 만료, 순서, 세션 변경, 전체 교체를 검증할 수 있도록 작성했다.
