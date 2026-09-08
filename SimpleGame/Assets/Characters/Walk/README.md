# 이동 스프라이트 시트 v1

작성일: 2026-09-08

## 파일

- knight-walk-sheet-v1.png: 기사, 4방향 × 4프레임.
- mage-walk-sheet-v1.png: 마법사, 4방향 × 4프레임. 무기 없이 장갑 마법진 유지.

## 배열

위에서 아래로 SOUTH(아래), WEST(왼쪽), EAST(오른쪽), NORTH(위).
각 행은 왼쪽부터 걷기 프레임 0, 1, 2, 3이며 반복 재생을 위한 초안이다.

## 확인된 제약

두 파일 모두 1254×1254 RGB 이미지이며 알파 채널이 없다. 체크무늬는 실제 이미지 배경이다.
생성 요청에는 투명 배경을 명시했으나 결과는 이를 충족하지 않았다.
원본을 그대로 게임 텍스처로 사용하면 체크무늬도 표시되므로 배경 제거가 필요하다.
1254는 4로 나누어떨어지지 않으므로 정수 크기 셀로 정리하고 발 기준점과 몸 크기를 맞춰야 한다.
정적 이미지로 방향과 16개 구성을 확인했으며, 장비 좌우 일관성과 걷기 연결의 자연스러움은 정식 프레임 정리 시 추가 조정할 대상이다.
현재 게임 코드에는 연결하지 않았다. 기존 필드 캐릭터 크기(32×52)는 변경하지 않았다.
게임 빌드·실행·애니메이션 재생은 수행하지 않았다.

## 생성 도구와 프롬프트

내장 image_gen 사용. 기준 이미지는 승인된 knight-coarse-preview-v2.png와 mage-coarse-preview-v2.png이다.

### 기사

Create a production-oriented WALKING SPRITE SHEET based on the attached approved coarse pixel character. EXACTLY 16 full body sprites in a perfectly regular 4 columns by 4 rows layout. Equal square cells, every character centered horizontally in its cell, feet share the same local baseline, ample padding, no overlap. Row 1 walking SOUTH facing viewer; row 2 walking WEST facing left profile; row 3 walking EAST facing right profile; row 4 walking NORTH facing away, ONLY back of head and cape, no face. Columns in EACH row show a coherent four-phase loop: 1 left foot forward/right arm forward opposite leg, 2 passing with feet close, 3 right foot forward/left arm forward, 4 other passing pose. Alternating feet MUST be visibly different, not four copies. Keep body mass and head size invariant, only legs/arms/cape move slightly. Coarse LOW PIXEL COUNT aesthetic of the reference, approximately 32x48 logical pixel character per cell displayed enlarged with hard square blocks. No finer detail than reference, no smoothing, no texture painting, flat restrained colors. Match brown hair, adult mercenary face, olive cape, brown boots, original proportions. Do not beautify or change to youthful new design. True transparent alpha background, no painted checkerboard, no ground shadows, NO text, NO cell border lines, no labels, no extra characters. Entire square sheet fills canvas evenly. KNIGHT: keep grey steel breastplate and shoulder armor, leather belt, sword in the character's RIGHT HAND and shield on the character's LEFT ARM consistently in every view (do NOT simply mirror gear handedness). Sword points downward in safe walking carry, shield travels with arm. Back views show olive cape and back of hair, sword/shield visible only where anatomically appropriate. Exactly four walking views per direction, 16 sprites total.

### 마법사

Create a production-oriented WALKING SPRITE SHEET based on the attached approved coarse pixel character. EXACTLY 16 full body sprites in a perfectly regular 4 columns by 4 rows layout. Equal square cells, every character centered horizontally in its cell, feet share the same local baseline, ample padding, no overlap. Row 1 walking SOUTH facing viewer; row 2 walking WEST facing left profile; row 3 walking EAST facing right profile; row 4 walking NORTH facing away, ONLY back of head and cape, no face. Columns in EACH row show a coherent four-phase loop: 1 left foot forward/right arm forward opposite leg, 2 passing with feet close, 3 right foot forward/left arm forward, 4 other passing pose. Alternating feet MUST be visibly different, not four copies. Keep body mass and head size invariant, only legs/arms/cape move slightly. Coarse LOW PIXEL COUNT aesthetic of the reference, approximately 32x48 logical pixel character per cell displayed enlarged with hard square blocks. No finer detail than reference, no smoothing, no texture painting, flat restrained colors. Match brown hair, adult mercenary face, olive cape, brown boots, original proportions. Do not beautify or change to youthful new design. True transparent alpha background, no painted checkerboard, no ground shadows, NO text, NO cell border lines, no labels, no extra characters. Entire square sheet fills canvas evenly. MAGE: same approved dark navy split robe, olive cape, brown belt and boots. NO WEAPONS whatsoever: no sword, shield, staff, wand, book, orb, or carried object. Both empty hands wear brown gloves bearing small cyan RING magic circles drawn directly on the backs of hands, visible only when that side of the hand is visible. Walk naturally with opposite arm/leg swing; not casting spells, no floating effects. Preserve robe split so alternate feet are clearly readable. North row shows cape and back of hair, no face. Exactly 16 sprites. Use a TRUE TRANSPARENT PNG alpha channel, not a baked checkerboard texture. Match reference coarse low-pixel art, do not add detailed shading.

