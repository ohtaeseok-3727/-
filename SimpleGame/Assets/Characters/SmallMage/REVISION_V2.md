# 미니 마법사 v2

이동: walk-transparent-v2.png, 4열 2행.
공격: attacks-transparent-v2.png, 6열 3행.
두 파일 모두 1536×1024 RGBA이며 모서리 알파 0을 파일에서 확인했다.
preview.html이 v2를 사용한다. v1은 보존했다.

기사의 짧은 체형을 참조하고 큰 마법사 mage-melee-v3.png의 동작을 미니 체형으로 옮겼다. 마력칼날 준비 프레임의 중복 팔을 수정했다. 생성 편집 특성상 원본과 픽셀 단위 동일성을 보장하지 않는다.
기존 궤적 0.8배 및 폭발 1.2배 효과 속도는 유지했다. 빌드 및 미리보기 실행 검증은 하지 않았다.

도구: 내장 image_gen. 아래는 주요 생성·수정 프롬프트다.

## mageProportionWalkPrompt

Edit target image 2 mage walk sheet using image 1 knight as STRICT anatomy and coarse pixel style reference. The mage must have EXACTLY the knight's squat proportions: hair+head approximately half total standing height, compact torso, very short legs, broad head, chunky large square pixels. Not the tall small-headed mage. Essentially redress the knight body template in navy mage tunic, olive short cape, brown gloves with tiny cyan circle, no weapon/shield. 8 walking poses 4 columns 2 rows on 1536x1024. Match knight frame placements and size: each standing character about 320px tall, top row feet y453, bottom row feet y900, centers x192,576,960,1344. Same consistent anatomy every pose. Facing right three-quarter. Actual transparent PNG RGBA alpha zero background, no painted background, no checkerboard, no ground or glow. Preserve mage costume identity only; replace tall anatomy with knight anatomy.

## mageFinalAttackPrompt

STYLE TRANSFER sprite sheet. Image1 old tall mage is EXACT MOTION REFERENCE: reproduce all 18 poses corresponding one-for-one, same torso twists leg stances arm gestures and cape dynamics. Image2 mini knight sets exact squat anatomy and coarse pixel style; image3 mini mage sets costume and large-head short-body proportions. Convert image1 motions to image3 chibi body, hair+head half height, stubby legs. Do NOT just use repeated standing arm poses. Row1 left palm thrust with crouching anticipation then deep stepping lunge; frames4 and5 SAME LEFT palm extended. RIGHT arm MUST remain clearly visible bent at waist in EVERY frame, distinct navy sleeve brown glove cyan dot below attacking arm; never missing or growing from back. Row2 same big cross-body windup and broad sweep from original, torso twist, both arms visible with non-attacking glove guarding waist. Row3 original two-palm brace and release. No detached effects (will overlay existing effects), only glove cyan circles. 1536x1024, six equal columns three rows, each sprite inside own cell with clear gutters, centered x128+256n, feet y320,650,980. Transparent cutout PNG actual RGBA alpha0, no checkerboard or backdrop. No weapons. Keep all18 frames and two arms/two gloves each.

## mageArmCorrectionPrompt

Anatomy correction on image: middle row THIRD column only (x512-767,y341-682). Current arms look like duplicate right arms. REPLACE BOTH arms in this cell, do not add appendages. Exactly TWO arms total: character right arm arises at screen-left shoulder, bends down with glove at screen-left waist x568 y578. Character left arm arises at screen-right shoulder x660 y526, crosses diagonally IN FRONT of chest toward screen-left with glove at x597 y522. Clearly trace each navy sleeve from its own shoulder to elbow to its own brown glove. Delete the current extra glove at x687 y548 and its associated sleeve completely. No third hand, no duplicate right forearm. This is a winding-up sweep posture with bent knees, same as prior tall mage motion. Preserve all other17 frames, face, squat proportions, costume, feet, six columns three rows,1536x1024. Remove checkerboard and deliver actual transparent cutout RGBA PNG alpha zero.

## mageSecondFrameFixPrompt

Precise anatomy correction ONLY middle row SECOND column of this 6x3 mage sprite sheet (cell x256..511,y341..682). Current pose reads as duplicate right arms. Replace BOTH arms in that one cell. Match anatomical arrangement of neighboring middle row THIRD column: one arm comes from screen-RIGHT shoulder and crosses chest to glove near screen-center-left x370 y535; opposite arm comes from screen-LEFT shoulder and bends down with glove at screen-left waist x326 y580. REMOVE existing glove at screen-right waist x434 y572 and its sleeve entirely. Exactly two arms, one per shoulder; exactly two hands, no extra limb. Preserve winding-up pose with bent knees and all other17 frames unchanged. Preserve big head short-body proportions, navy costume and cape. Deliver actual TRANSPARENT CUTOUT RGBA PNG alpha0 background no checkerboard, same1536x1024 canvas.
 
