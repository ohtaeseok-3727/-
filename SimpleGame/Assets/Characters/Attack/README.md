# 기사 공격 스프라이트 초안

## 최신 수정 v5 — 프레임 간 간섭 제거

- `preview-v5.html`: 방패 밀치기 2·3번과 가로 베기(찌르는 자세) 5번의 이웃 망토·발 노출을 해결하기 위해 추출 영역과 배치 코드를 수정했다.
- 단순한 384×512 사각형 분할 대신 프레임별 영역을 지정했다. 가로 베기 5번 검 끝과 다음 프레임 망토는 높이에 따라 경계를 나눠 분리한다.
- 브라우저에서 각 자세를 원본 크기 그대로 512×640 칸에 재배치한다. 기준 칸의 상하좌우에 64픽셀씩 여유를 추가한 2048×1280 시트로 재생한다.
- `간격을 늘린 시트 PNG 저장` 버튼으로 선택한 공격의 재배치 시트를 저장할 수 있다. 원본 PNG는 보존하며 새 시트는 브라우저에서 생성된다.
- 이미지 생성·자세 변경 없이 미리보기의 Canvas 추출·배치 코드를 수정했다. 게임 코드에는 연결하지 않았으며 빌드·게임 실행·HTML 실행은 수행하지 않았다.

## 최신 수정 v4 — 가로 베기 두 번째 프레임

- `knight-horizontal-slash-v4.png`: 첫 행 두 번째 자세에서 검을 잡는 오른팔과 방패를 잡는 왼팔을 분리한 수정본.
- `preview-v4.html`: 수정된 가로 베기 두 번째 프레임이 처음 표시된다. 재생 버튼으로 전체 동작을 확인할 수 있다. 방패 밀치기와 내려베기는 v3 시안을 사용한다.
- 원본 v3는 보존했다. 생성 이미지의 자세를 육안으로 살펴보고 미리보기의 이미지 교체를 정적으로 확인했다. 게임 빌드 및 HTML 실행은 수행하지 않았다.

## 역동적인 동작 수정 시안 v3 (2026-09-08)

- 사용자의 자연스러운 연결 및 더 역동적인 동작 요청을 반영한 시안이다.
- `preview-v3.html`을 브라우저로 열고 재생 버튼을 누르면 공격 선택, 0.25~1.5배 속도, 프레임 이동을 사용할 수 있다. PNG가 HTML에 포함되어 미리보기 파일만으로 재생 가능하다.
- `knight-shield-bash-v3.png`: 몸을 낮춘 준비, 앞발 디딤과 방패 전진, 회수.
- `knight-horizontal-slash-v3.png`: 넓은 발 디딤, 검의 좌우 이동, 망토의 후속 움직임.
- `knight-overhead-chop-v3.png`: 검을 높이 드는 준비와 몸을 낮춘 내려베기의 대비.
- 각 시트는 1536×1024, 4열×2행 8프레임의 배경 포함 이미지다. 이전 v1/v2는 비교용으로 보존했다.
- 프리뷰에서는 발 기준점을 맞추며 배율은 고정한다. 반복 마지막에는 원본 1번을 다시 사용하고, 원본 8번은 별도 비교 버튼으로 볼 수 있다. 프레임 간 자세·장비 형태 차이를 완전히 해결한 최종 게임용 아틀라스는 아니다.
- 가로 베기 v3의 검이 셀 경계를 넘는 부분은 프리뷰의 프레임별 수평 범위로 포함한다. 게임에서 단순히 4×2 UV로 분할하면 검이 잘릴 수 있다.
- 재생 시간과 기준점은 미리보기용 임시 설정이다. 공격 판정, 이동 거리, 콤보 규칙을 확정하지 않는다.
- 게임 코드에는 아직 연결하지 않았다. 빌드, 게임 실행, HTML 재생 확인은 수행하지 않았으며 파일 경로·소스만 정적으로 검토했다.
- 생성 도구·전체 프롬프트: `GENERATION_PROMPTS.md`.

## 최초 시안 v1

- 파일: knight-attacks-sheet-v1.png
- 3행 × 6열, 총 18프레임. 정면에 가까운 우측 3/4 시점.
- 첫 행: 방패 밀치기.
- 둘째 행: 화면 기준 좌 → 우 검 휘두르기.
- 셋째 행: 위 → 아래 내려베기.
- 각 행은 준비, 예비 동작, 가속, 타격, 후속 동작, 회수 순서로 제작 요청.
- 공격 순서는 확정 설정이며 6프레임 구성과 시점은 제작용 선택이다.
- 원본은 1536×1024 배경 포함 시트로, 투명 배경 정식 아틀라스가 아니다.
- 게임 연결 전 배경 제거, 발 기준점·무기 여백 정렬과 재생 시 동작 검토가 필요하다.
- 좌우 베기는 현재 시안의 화면 기준이다. 다른 방향의 공격 시트는 아직 없다.
- 공격 판정, 연계 입력, 피해량 및 게임 코드는 이번 작업에서 구현하지 않았다.
- 빌드와 게임 실행은 수행하지 않았다.

## 생성

내장 image_gen 도구 사용. 기존 knight-coarse-preview-v2.png를 참조했다.

Create ONE low-resolution pixel-art KNIGHT ATTACK ANIMATION sprite sheet using attached approved knight as exact costume, face and coarse pixel style reference. Exactly 3 rows x 6 columns = 18 full-body frames. Fixed equal cells, aligned foot baseline per row, same character scale in every frame, generous space for sword above head and on BOTH sides, no clipping, no overlap. Plain SOLID dark slate background (#2b3037), no texture, no checkerboard. No text or lines. Character: brown hair adult mercenary, olive cape, grey breastplate and shoulder armor, brown belt and boots, ONE sword ALWAYS held in right hand, ONE shield ALWAYS strapped to left forearm. Original coarse chunky pixel clusters, about 32x48 logical character detail, no high-resolution illustration, no smooth shading. ONE consistent front-three-quarter camera with character facing slightly screen-right in ALL frames; do not turn to back views. 6 columns per attack: ready, windup, acceleration, impact, follow-through, recovery. ROW 1 SHIELD BASH: keep sword low and inactive, bring shield close to chest, lean back to load, then drive LEFT ARM shield forward away from chest toward screen-right with a short lunge, maximal shield reach on impact, retract back to ready. Clearly shield hits, not sword. ROW 2 HORIZONTAL LEFT-TO-RIGHT SWORD SLASH: right hand draws sword across body to SCREEN LEFT in windup, then blade swings horizontally across at waist/chest height and finishes on SCREEN RIGHT in impact/follow-through. Use distinct poses that clearly show sword on left in column2, across front in column3, on right in column4, lower right in column5, recover in column6. Shield remains on left forearm and does NOT become sword. ROW 3 OVERHEAD DOWNWARD SWORD CHOP: windup raises RIGHT HAND and sword clearly ABOVE head vertically, column3 starts descent, column4 sword driven DOWN in front of body with bent knees, column5 blade low near ground, column6 recovery. Sword held ONE-HANDED since left arm carries shield. Use at most a tiny thin pale pixel slash trace in impact frames; do not obscure blade or body with giant arcs. No enemies, no gore, no scenery, no UI, no labels. Three unmistakably different attacks, coherent anatomy, modest attack posing and visible weapon trajectories.
