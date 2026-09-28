# 후속 교정 및 정밀 검사

2026-09-23. 내장 image_gen 사용. 완성 기준 미달, 게임 적용 보류.

## 실제 검사

System.Drawing으로 모든 픽셀의 알파 값을 검사했다.

| 파일 | 크기 | 알파 0 | 알파 1~254 | 알파 255 |
| --- | --- | --- | --- | --- |
| 기준 walk-transparent-v1.png | 1536×1024 | 1083124 | 489740 | 0 |
| thrust-draft-v1.png | 2172×724 | 1107908 | 462020 | 2600 |
| sword-wave-draft-v2.png | 1536×1024 | 1175250 | 397614 | 0 |

기준도 반투명으로 저장되어 있으므로 알파가 255가 아니라는 이유만으로 불합격 처리하지 않는다. 외곽 배경 잔여물은 별도 확인이 필요하다.
기준 첫 캐릭터 영역을 4px 간격으로 표본 검사했을 때 알파 253이 가장 많았다. 피부색 FED6AB/FDD6AA, 머리색 B46531/B36531 등 가까운 RGB 값이 공존했다. 단일 정수 격자 및 소수 색상만으로 구성된 원본은 아니다.

## 교정 방식과 결과

전체 시트 재생성 대신 검기 발산의 수평 베기 종료 자세 한 장만 새로 그렸다. 검이 화면 왼쪽을 향하고 팔꿈치를 굽힌 자세로 만들어, 이전 정면 찌르기와 다른 동작을 확보했다.
파일: horizontal-slash-keyframe-draft-v3.png.
그러나 지정한 384×512 캔버스/원본 도트 크기를 출력에서 유지하지 못했다. 미니 이동 시트와 정확한 픽셀 크기·팔레트 일치를 검증할 수 없어 완성본으로 채택하지 않았다.

## 필요한 다음 제작 단계

이미지 생성만으로 동일 격자와 팔레트를 고정할 수 없으므로 직접 픽셀 편집 또는 코드 기반 래스터 제작이 필요하다. 원본 픽셀은 복사하지 않고 색상과 체형 치수만 참조한다.
고정 격자에서 새로 그린 동작, 공통 발 기준점, 분리된 검기 효과를 작성한 뒤 프레임별 체형과 손/무기 연결을 확인한다. 파일 크기·프레임 배치·허용 팔레트·외곽 투명도도 검사한다.
현재 이미지 편집 도구 지침상 다른 제작 수단은 사용자 명시 요청이 필요하므로 코드 기반 이미지 편집은 아직 하지 않았다.

## 실제 사용 프롬프트

Use case: stylized-concept. Produce a SINGLE knight animation keyframe, not a sheet. Reference image is the mandatory visual-only mini walk model; redraw from scratch, do not copy or paste original pixels. Preserve EXACT same head-to-torso proportions, head size, shoulder width, short arm/leg lengths and thickness, coarse square pixel size, brown hair/silver armor/olive shield/brown boots palette. Canvas 384x512 if possible with sprite roughly 230px wide and 320px tall, reference first cell scale. Real alpha transparent empty background. Knight turns torso in a horizontal sword slash at waist height: anatomical right hand holds sword, bent elbow, fist just below chest, BLADE POINTS TOWARD SCREEN-LEFT across the body in follow-through. Left arm keeps shield on screen-right, visibly separate from sword forearm. Body facing screen-right three-quarter view as reference. This is the END of a lateral sweep across the torso, NEVER a forward thrust, never an overhead chop. Two natural arms, two legs, one sword, one shield. No magic, no crescent, no effects, no ground, no shadow, no brown halo, no haze, no checkerboard, no grid, no text. Solid flat crisp pixel clusters; transparent pixels immediately beyond silhouette.
