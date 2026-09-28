# 마력칼날 효과 교체 시안

mana-blade-v1.png: 1536×1024, 3열 2행, 512×512 셀, 6프레임.
실제 RGBA, 모서리 알파 0 확인. 내장 image_gen으로 제작했다.
작은 궤적 → 초승달 칼날 → 최대 베기 → 얇은 잔상 → 끊어진 궤적 → 입자 소멸.
미니 마법사 preview.html의 2번 효과만 교체했다. 캐릭터 동작과 궤적 0.8배 속도는 유지한다.
별도 프레임 캔버스의 64px 여백을 사용한다. 원본 효과는 보존한다.
빌드 및 미리보기 실행 검증은 하지 않았다.

## 생성 프롬프트

Game VFX sprite sheet ONLY mana blade horizontal melee slash, coarse crisp pixel art for tiny chibi fantasy mage. Six animation frames arranged THREE columns TWO rows on1536x1024, each cell512 square with >=80px empty margin all sides. Consistent anchored horizontal elliptical sweep centered within each cell, no flying projectile. Reading left to right then next row: 1 tiny cyan ignition streak;2 growing tapered crescent;3 full powerful wide horizontal crescent white core cyan edge;4 followthrough same arc thinning;5 broken short fading cyan trail;6 sparse small cyan pixel fragments. Smooth progressive sweep, decisive blade edge, sparse sparks, simple strong silhouette, no magic circles no characters no text no grid no background. Actual transparent RGBA alpha-zero PNG, no checkerboard. Effects maximum340px wide220px high, center256,256 in each cell. Flat discrete pixel clusters no airbrushed glow.
