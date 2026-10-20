# 거친 단면의 검 이펙트 v3

사용자 요청: 기존 기사 효과처럼 투박하고 강렬하게 표현하되, 깨진 조각이나 떨어지는 파편을 추가하지 않고 하나로 이어진 궤적의 단면과 가장자리를 거칠게 표현한다.

## 보기

프로젝트 루트에서 `py -m http.server 8765 --bind 127.0.0.1`을 실행하고 `http://127.0.0.1:8765/output/procedural-sword-v3/preview.html`을 연다. 이미 서버가 실행 중이면 추가 실행하지 않는다.

- 가로 베기 / 내려베기 전환
- 원본 스프라이트 효과 / 새 코드 효과 / 캐릭터만 비교
- 궤적 자세히 버튼으로 주요 순간 정지
- 시간 슬라이더, 재생 속도, 검기 색상과 검날 오라 조절
- [부드러운 v2](../procedural-sword-v2/preview.html)와 별도 비교

## 표현

- 원본의 금빛 테두리와 넓은 흰색 중심을 사용한다.
- 연결된 하나의 실루엣에 불규칙한 굴곡과 4px 단위 도트 가장자리를 적용한다.
- 궤적 생성은 105ms, 짧은 잔광으로 설정했다. 수치는 미리보기용이다.
- 새 효과의 분리된 조각·비산 입자·착지 파편은 모두 제거했다. 원본 비교 모드에서는 원본 이미지에 포함된 입자가 보일 수 있다.
- 끝부분의 짧은 섬광과 내려베기의 바닥 빛은 유지한다.
- [원본 공격](../../SimpleGame/Assets/Characters/SmallKnight/attacks-transparent-v2.png)과 [원본 효과](../../SimpleGame/Assets/Characters/SmallKnight/effects-transparent-v2.png)를 참조한다. 기존 캐릭터 이미지와 공격 자세·박자를 유지한다.

## 파일과 흐름

| 파일 | 역할 |
| --- | --- |
| [preview.js](preview.js) | `SwordStudio`가 입력·시간·효과 선택과 그리기를 연결 |
| [motion.js](motion.js) | `KnightMotion`이 기존 캐릭터 자세와 검날 연결점, 원본 비교를 담당 |
| [sword-effects.js](sword-effects.js) | `SwordEffects`가 연결된 거친 궤적, 오라·섬광·바닥 빛을 담당 |
| [pixel-geometry.js](pixel-geometry.js) | `PixelGeometry`가 도트 다각형·선분·연결된 섬광 도형을 그림 |
| [arena.js](arena.js) | 미리보기 배경·그림자를 담당 |
| [preview.html](preview.html), [preview.css](preview.css) | 화면·비교 제어와 도트 표시 스타일 |

시간에 따라 원본 자세를 고르고 → 바닥 빛 → 캐릭터 → 별도 저해상도 캔버스에 그린 궤적을 보간 없이 확대하여 표시한다. 매 프레임 효과 캔버스를 비워 시간 이동 후 잔상이 남지 않는다. 검 연결점은 시각적으로 맞춘 근사값이며 자동 검 추적이나 피해 판정이 아니다.

## 확인

- JavaScript 5개 파일 구문 검사 통과.
- 사용자 요청에 따라 브라우저에서 가로 베기·내려베기 주요 시점을 확인. 분리된 파편이 없는 연결된 궤적을 확인했다.
- 확인 시 브라우저 오류·경고 없음.
- 게임 적용·게임 빌드·게임 실행은 수행하지 않았다.
