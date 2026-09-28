# 미니 마법사 이동·공격

- walk-transparent-v1.png: 1536×1024, 4×2 이동8프레임.
- attacks-transparent-v1.png: 1536×1024, 6×3 공격18프레임. 손바닥 타격, 마력칼날 휘두르기, 양손 폭발 준비 순서.
- PNG 두 파일의 실제 RGBA 알파0을 확인했다. 최초 생성의 체크무늬 배경은 별도 추출로 제거했다. 생성 편집이므로 원본과 픽셀 단위 동일함을 보장하지 않는다.
- preview.html에서 기존 Attack/Mage/mage-melee-effects-v1.png와 mage-burst-b-v2.png를 가산 합성한다. 기존 효과 원본은 새로 생성하거나 변경하지 않았다.
- 캐릭터와 효과의 시간을 분리하고 칼날0.8배, B안1.2배를 유지했다. 미니 캐릭터에 맞춰 효과 배율0.55/0.9/0.65를 사용한다. 게임 판정값은 아니다.
- 우측 시점 시안이며 좌우 반전 버튼 제공. 방향별 게임 연결은 하지 않았다.
- 이미지 육안 및 알파·경로 정적 확인만 수행했다. 빌드·게임 실행·HTML 실행은 하지 않았다.

## 생성 프롬프트

내장 image_gen 사용. 기사 미니 시안을 비율 참조, 기존 마법사 시안을 의상 참조로 사용했다. 후속 편집으로 체크무늬만 제거하고 실제 투명 알파를 요청했다.

Create MINI MAGE walk sprite atlas. Reference1 knight is PROPORTIONS ONLY: large head cute2.2heads tall stubby body. Reference2 mage is COSTUME: brown hair navy tunic olive cape brown gloves with cyan circle marks belt boots. NO sword shield staff hat or weapon. TRUE TRANSPARENT PNG RGBA background alpha0, not painted checkerboard, no backdrop shadow glow. Crisp coarse square pixel art. EXACT1536x1024 FOUR columns TWO rows8 chronological walking frames, each384x512 with wide64px margins. Samefront3quarter facing screenRIGHT all8. Character about260px tall fixed scale footbaseliney400 centerx192. Eightframe continuous grounded gait: left foot reaches, lowers weight, right passes under, right reaches, right takes weight, left lifts, left passes, left reaches returning to1. Short arms swing opposite legs with visible glove circles, cape gently follows, head only1logicalpixel bob. Whole character intact in every cell. Maintain exactly two connected arms and hands, consistent face equipment proportions, no attack or magic effects.

Create MINI MAGE MELEE ANIMATION atlas. Reference1 is tiny cute2.2headproportions only, reference2 gives MAGE costume: brown hair NAVY tunic olive cape brown boots gloves with cyan circles. Absolutely no sword shield staff or held weapon. REAL TRANSPARENT PNG alpha0 backdrop, no checkerboard drawn, no shadows. Crisp coarse pixel clusters.1536x1024 EXACT6columns3rows18 fullbody frames samefront3quarter facingRIGHT. All poses including hands inside their256x341cell with24px margins; same scale head45percent height, bodyheight190px, feetlocaly285. Columns1guard2shortwindup3accelerate4impact5followthrough6recover. ROW1 LEFT PALM STRIKE: LEFT arm draws near chest then palm thrusts right across front chest at impact, keep SAME LEFT palm extended in5, opposite RIGHT hand retracted and OCCLUDED by torso in4and5, no fist attached to back. ROW2 MANA BLADE SWEEP: one open hand coils across torso then sweeps horizontally ending right, slight body turn, opposite hand guards chest, no physical weapon or blade (effect separate). ROW3 TWO PALM BLAST: both hands gather at chest, then extend forward side by side as knees bend, hold open palms forward, recover. Short simple clean silhouettes, only two connected arms/hands, no hands on back, no extra limbs. NO VFX outside glove markings; large effects will be added separately. All frames consistent tiny proportions and equipment.

