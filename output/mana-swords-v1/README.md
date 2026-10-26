# 다중 마력검 미리보기

- 사용자 확정: 마력검 5개, 본체는 개별 객체, 적 관통, 이동거리 500px, 돌진 효과는 별도 스프라이트.
- 제작 범위: 이미지와 발사 미리보기. 실제 게임 스킬·피해 시스템에는 연결하지 않았다.
- 사용자 후속 요청: 이동을 더 빠르게 하고 배열을 불규칙하게 한다. 시안에서는 이동 800→1200px/s(1.5배), 앞뒤·높이가 엇갈리는 고정 비대칭 좌표 5개를 적용했다. 재생마다 무작위 재배치하지 않는다.
- 시안 가정: 기존 마력칼날 모션 시작 후 340ms에 첫 발사, 이동 1200px/s, 순차 발사 간격 80ms. 간격 0으로 일제 발사를 비교할 수 있다. 검마다 자신의 생성 위치에서 500px를 이동한다.
- `../shared/mage-player-motion.js`: `MagePlayerMotion`이 원본 8프레임과 [140,110,90,50,60,65,90,150]ms 재생 시간을 그대로 사용한다. `../../SimpleGame/Assets/Characters/SmallMage/mana-blade-motion-v1.png`는 기존 `mana-blade-motion-v1.png`의 수정 없는 복사본이다. 기존 근접 효과는 함께 재생하지 않고 해당 스킬의 발사체를 연결한다.
- 후속 간격 조정: 검끼리 약간 겹쳐도 된다는 요청에 따라 세로 간격을 약 60%로 축소했다. 앞뒤 위치는 유지하고 세로 좌표를 160, 111, 215, 191, 135로 조정했다.
- `preview.html`: 비교 이미지, 재생·정지·시간 탐색·발사 간격·효과 표시 UI.
- `preview.js`: `ManaSword`가 각 검의 위치·500px 이동거리·표적 통과를 관리한다. 본체는 Canvas 도형으로 직접 그린다. `ManaSwordPreview`가 시간과 표시를 관리한다. `Set`은 검별 통과 표적을 구분한다. 시간 탐색 시 상태를 재계산한다.
- `../../SimpleGame/Assets/Characters/Skills/dash-trail.png`: 내장 image_gen으로 만든 독립 돌진 효과. 실제 RGBA 투명도 확인. 기준 이동 시트와 색상·도트의 엄밀한 일치는 보장되지 않아 검토용 시안으로 보관한다.
- 검 본체의 색상과 모양도 미확정 디자인 시안이다. 캐릭터 스프라이트는 새로 만들거나 수정하지 않았다.
- 검증: JavaScript 구문 검사 및 PNG 알파 확인. 게임 빌드·실행은 하지 않았다.

## 생성 프롬프트

Create a standalone transparent pixel-art DASH TRAIL effect sprite for a flying magical sword in a 2.5D game. Reference image SmallMage/walk-transparent-v2.png is the fixed visual standard: preserve its chunky pixel size and palette, use only the cyan and teal colors already visible in the glove rune. No character drawn; any character body proportions must match reference exactly. Draw new pixels, never copy source pixels. One horizontal compact tapered wake moving RIGHT: wide near the leading right end, three short broken trailing streaks narrowing toward the left. The leading end is open because a separate geometric sword object will be drawn there by code. EFFECT ONLY: no sword, no hilt, no blade silhouette, no characters, no scenery, no text, no labels. True transparent RGBA alpha-zero background, no black or checkerboard painted background. Coarse hard square pixels, no soft glow, gradients, smooth curves, anti-aliasing or haze. Centered on square canvas, wake occupies central 70 percent width and 20 percent height, generous transparent padding. This sprite will be attached behind each independently moving sword object.

2026-10-26: 중복 이미지 복사본을 삭제하고 런타임 자산을 직접 참조한다. 공통 모션 코드는 `output/shared/mage-player-motion.js`에 통합했다.
