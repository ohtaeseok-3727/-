# 코드 기반 이펙트 시안

- `preview.html`: 두 효과를 비교하고 재생 속도·검 오라 색상을 조절하는 화면.
- `preview.js`: `EffectsPreview`가 재생 시계와 UI를 연결한다.
- `shockwave.js`: `Shockwave`가 지면 타원 파동, 균열, 포물선으로 솟는 파편을 그린다. 실제 지형을 수정하지 않는다.
- `sword-aura.js`: `SwordAura`가 검 도형, 발광, 주변 입자와 이전 각도의 잔상을 그린다.
- `drawing.js`: 공통 선 그리기와 시간에 따른 투명도 계산.

외부 이미지·라이브러리가 없는 Canvas 시안이다. 기존 게임에는 아직 연결하지 않았다. 캐릭터 동작은 포함하지 않고 무기와 효과 발생 위치를 보여준다. 모든 수치는 시안용 가정이다.

ES 모듈을 사용하므로 프로젝트 루트에서 `py -m http.server 8765`를 실행한 다음 `http://localhost:8765/output/procedural-effects-v1/preview.html`을 연다. 종료는 터미널에서 Ctrl+C.

사용자 지침에 따라 빌드·실행 및 시각 재생 검증은 수행하지 않았다. 모듈 경로, Canvas 상태 복원, 효과 시간 범위를 정적으로 검토했다.
