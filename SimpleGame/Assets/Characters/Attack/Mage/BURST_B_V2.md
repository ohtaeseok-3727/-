# B안 폭발 스프라이트 v2

- 사용자 선택: 고정된 마법진 앞에서 커지는 폭발. 분리되어 날아가는 파동은 대체했다.
- mage-burst-b-v2.png: 1536×1024, 4열×2행, 8프레임. 검정 배경 가산 합성용.
- 단계: 마법진 형성, 응축, 점화, 확장, 최대 폭발, 분해, 파편, 소멸.
- preview-effects-v2.html과 기존 preview-effects-v1.html의 3타에 연결했다. A/B 비교 파일은 과거 비교용으로 보존한다.
- 고정 배율0.76으로 재생하며 프레임별 마법진 원점을 맞춘다. 확대 보간으로 단일 프레임을 늘리는 기존 B안 대신 다른 형상의 프레임을 재생한다.
- 프레임 시간80/75/45/65/85/90/85/80ms. 캐릭터 속도와 마력칼날 궤적0.8배는 유지한다. 수치는 표현용이며 공격 판정을 확정하지 않는다.
- 후속 요청으로 폭발 효과 시간에만 1.2배 재생속도를 적용했다. 전체 효과는 605ms에서 약504.17ms로 단축되며 시작 시점과 캐릭터 속도는 유지한다. 두 효과 미리보기 HTML에 동일하게 반영했다.
- 이미지 육안 검토 및 HTML 내장 이미지·타임라인의 정적 검토만 수행했다. 빌드·게임 실행·HTML 실행은 하지 않았다.

## 생성 기록

내장 image_gen 사용, mage-melee-effects-v1.png를 참조했다.

Create NEW pixel art animation of the BOTTOM ROW THIRD effect in reference: a vertical rune magic circle releasing a broad explosive burst SCREEN RIGHT. Effects only. EXACT1536x1024 atlas4columns2rows8 chronological frames, each384x512. PURE BLACK RGB0 background for additive blending, no haze, no broad glow, no characters text borders. Coarse crisp cyan turquoise white pixel clusters. All effects inside cell with32px margins. FIXED ORIGIN local x100,y256 in EVERY frame: vertical oval magic circle width60 height180 at this origin. Circle NEVER travels. Explosion grows OUT OF circle into screen right at same anchored origin, never a detached traveling crescent, ring, ball or projectile. Frame1 thin partial rune oval, tiny sparks gathering inward. Frame2 complete bright rune oval, compressed bright point at its center. Frame3 compact white flash bursts through center, short jagged cone of energy extending70px right. Frame4 blast unfolds into thicker branching jagged plume extending140px right and150px high, irregular lobed outline. Frame5 peak broad fan shaped explosion extending230px right and250px high with bright white core near origin and cyan jagged outward tongues; circle still behind at same location. Frame6 same large footprint but hollowing core and breaking plume into jagged fragments; no growth into sphere. Frame7 separated outward shards mostly on right with large black gaps, rune circle thin and fading. Frame8 very few small dim cyan motes scattered in same footprint, circle almost gone. Successive frames reshape the blast organically, NOT copies simply scaled up. Clear anticipation, fast ignition, expansion, disintegration and fade. Never create a traveling wavefront. Preserve palette and pixel size across all8frames, generous empty separation.
