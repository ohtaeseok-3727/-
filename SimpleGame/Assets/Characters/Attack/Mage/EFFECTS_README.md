# 마법사 근접공격 효과 시트

- mage-melee-effects-v1.png: 1536×1024, 6열×3행. 위부터 작은 마법진 폭발, 마력칼날의 곡선 궤적, 세로 마법진에서 전방으로 나가는 충격파.
- 검정 배경 가산 합성용이다. 투명 알파 이미지로 간주하지 않는다. 넓은 배경 광채가 있던 최초 출력을 수정하여 효과 외곽을 분리했다.
- preview-effects-v1.html: 캐릭터 v3와 효과를 내장한 합성 미리보기. 공격 선택, 속도, 효과만 보기, 12단계 이동을 지원한다. 효과6프레임이 재생되는 동안 타격 자세를 유지한다.
- 1타 효과 배율0.72, 2타1.30, 3타0.76. 사용자 요청에 따라 최초 시안 대비 가로·세로 각각 2배로 확대했고 손과 마법진의 기준 위치는 유지한다. 3타 원형 높이는 약160px이다. 프레임별 원점과 타이밍은 HTML에 기록되어 있으며 게임 밸런스 확정값이 아니다.
- 기존 캐릭터 시트의 작은 효과는 그대로 남아 있다. 최종 게임 적용 시 캐릭터의 효과를 제거한 원본 및 HDR 발광값과 Bloom 연결이 필요하다.
- 생성 이미지를 육안 확인하고 파일·내장 이미지 연결을 정적으로 확인했다. 빌드·게임 실행·HTML 재생 확인은 하지 않았다.

## 생성 도구와 프롬프트

내장 image_gen 사용. 최초 생성 후 해당 이미지를 편집했다. 최종 출력만 프로젝트에 복사했다.

### 최초 생성

undefined

### 배경 정리

Edit this VFX atlas into clean ADDITIVE BLEND sprite sheet. Keep EXACT 1536x1024,6columns3rows, same magic explosion, blade arc and portal shockwave animation designs and frame order. REPLACE ALL backdrop and diffuse blue haze with PURE FLAT BLACK RGB0,0,0. Remove ALL broad glow halos; ONLY crisp cyan/blue/white pixel effect shapes remain on black. No gradients, no fog, no ambient light. Shrink each effect within its own256x341cell to80percent around its cell center to leave at least24px pure black margins on every edge; never cross into adjacent frame. Three rows of6 coherent frames, no characters or text. The bottom row portal is fixed on left side of each cell emitting wave to the right. Black must be completely black everywhere outside the actual crisp magical glyphs and trails, suitable for additive sprite rendering.



## 마력칼날 속도 조정

마력칼날 궤적만 최초 재생속도의 0.8배로 조정했다. 캐릭터 타임라인은 원래 속도를 유지한다. 효과 시작 시점 이후의 경과 시간에만 0.8을 적용하며 효과 재생 시간은 255ms에서 318.75ms로 늘어난다. 다른 공격은 기존 속도를 유지한다. 전체 속도 선택은 기존처럼 양쪽에 적용된다. 코드 정적 확인만 수행했다.


## 3타 충격파 확대·전진

마법진은 기존 배율과 손 위치에 유지하고, 효과 시트 3행 4열의 오른쪽 파면을 별도로 그린다. 충격파 배율은 0.95에서 1.35로 커지며 전방으로 250px 이동하고 마지막 35% 구간에서 사라진다. 기존 통합 시트의 파면은 잘라 중복 표시를 피한다. 미리보기 폭은 760px로 확장했다. 수치는 미리보기용이며 게임 판정은 변경하지 않았다. 코드 정적 검토만 수행했다.

## 3타 A/B 비교

compare-shockwave.html은 동일한 타임라인으로 A 전진 파동과 B 고정된 원점에서 커지는 폭발을 나란히 표시한다. B는 효과 시트 3행 3열의 전방 폭발 부분을 확대하고 점차 사라지게 한다. 선택 전 비교 시안이며 기존 효과 미리보기는 유지한다. 원본 PNG를 변경하지 않고 Canvas 합성 방식만 추가했다. 파일 구조를 정적으로 확인했으며 빌드·HTML 실행은 하지 않았다.

A/B 표시 오류 수정: compare-shockwave.html의 미선언 variant와 const ctx 재할당 오류를 수정했다. 두 화면을 순차적으로 그릴 수 있도록 ctx와 variant를 let으로 선언했다. 코드 정적 확인만 수행했다.
