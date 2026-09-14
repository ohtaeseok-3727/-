# 마법사 근접공격 시안 v1

## 수정 v3 — 등 뒤의 잘못된 손 제거

- `mage-melee-v3.png`: 손바닥 타격 5번에서 등 뒤에 붙어 보이던 갈색 손을 제거했다. 앞으로 뻗은 왼손만 보이며 오른손은 몸에 가려진 자세다.
- `preview-v3.html`: v3 이미지를 내장하고 수정한 5번 프레임부터 표시한다.
- 이미지에서 잘못된 손이 사라진 것을 육안 검토했다. 미리보기 내장 이미지 교체는 정적으로 확인했으며 빌드·게임 실행·HTML 실행은 하지 않았다.

## 수정 v2

- `mage-melee-v2.png`: 손바닥 타격의 5번째 프레임을 4번째와 동일한 왼손을 뻗는 자세로 수정했다. 반대 팔은 몸 쪽에 접어 유지한다.
- `preview-v2.html`: 수정 시트가 내장되어 있으며 처음에 손바닥 타격 5번째 프레임을 표시한다. 이전·다음 버튼으로 연결 자세를 비교할 수 있다.
- 생성 이미지의 팔 자세를 육안 검토하고 내장 이미지 교체를 정적으로 확인했다. 빌드·게임 실행·HTML 실행은 하지 않았다.
- v1 원본과 미리보기는 보존했다.

- mage-melee-v1.png: 1536×1024, 6열×3행, 공격별 6프레임.
- 첫 행 손바닥 타격, 둘째 행 마력 휩쓸기, 셋째 행 양손 충격파. 이 구성은 제작용 제안이며 확정된 공격 규칙이 아니다.
- preview-v1.html은 이미지가 내장된 재생·속도 조절·프레임 선택용 미리보기다. 공격별 발 기준점과 고정 배율을 사용한다.
- 원본에 배경과 작은 마력 효과가 포함되어 있다. 방향별 투명 게임 아틀라스, 공격 판정, Bloom 분리는 아직 적용하지 않았다.
- 생성된 시트의 배치를 육안 검토했다. 빌드·게임 실행·HTML 실행은 하지 않았다.

## 생성 기록

내장 image_gen 사용. 승인된 mage-coarse-preview-v2.png를 외형 참조로 사용했다. 아래 수치는 생성 요청이며 출력 보장값이 아니다.

Create a coarse pixel art MAGE MELEE ANIMATION sprite sheet matching attached approved character: brown hair, navy tunic and pants, olive cape, brown boots and brown gloves with tiny CYAN MAGIC CIRCLES. Absolutely NO weapon, no staff, no shield. 1536x1024 canvas, EXACT 6 columns x3 rows,18 full body poses, each cell256x341. Character upright height ONLY180px. Mandatory at least40px EMPTY MARGIN on LEFT and RIGHT of every pose INCLUDING hands cape and effects. Every pose wholly within its cell, no neighboring sprite intrusion. Fixed front three quarter view towards screen right. Same scale and foot baseline local y=280 in every cell. Chunky hard pixel clusters, no detailed illustration. Solid flat dark slate background RGB43,48,55. NO text, lines, gradients. Each row ONE continuous 6frame attack: guard, loaded anticipation, fast transition, impact, follow through, recovery. ROW1 SINGLE PALM STRIKE: right hand pulls to waist while knees bend, front foot steps and hip drives right palm forward screen right, elbow extends, rear hand stays near chest, impact has tiny cyan circle at palm, retract to guard. ROW2 LEFT HAND HORIZONTAL MAGIC SWEEP: left hand draws back across chest, torso coils then rotates and left open palm sweeps horizontally across front ending screen right, wide planted stance and cape follows a beat later; short faint cyan crescent at hand on impact only no long trail. ROW3 TWO PALM SHOCKWAVE: both hands draw near chest palms forward, knees load, both palms thrust forward together with low lunge and torso inclined, small cyan magic circle between palms at contact only, settle and rise to guard. TWO arms and TWO hands anatomically connected at all times, constant limbs, expressive whole body motion, feet planted except clear step, never airborne. Effect stays within cell margins and does not hide hands. Preserve reference costume and face. The spacing is critical: keep figures small in their large cells rather than filling canvas.
