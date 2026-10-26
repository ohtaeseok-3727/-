# 마법과 검의 노래

## 사용과 확정 수치

마법사 Lv.20에서 `K` → **마법과 검의 노래** 아이콘을 퀵슬롯으로 드래그한다. MP 50, 쿨타임 60초이며 반경은 월드 좌표 300이다. 새 코드를 빌드해야 게임에 적용된다.

- 검 세례: 4초간 총 40회, 0.1초 간격. 타격마다 원 안에 있는 살아 있는 적 모두에게 `공격력 × 0.1 + INT × 0.2` 피해.
- 최종 합체 폭발: 같은 원 안의 적 모두에게 `공격력 × 4 + INT × 6` 피해 1회.
- 검 그림과 적의 개별 충돌 여부는 피해 조건이 아니다. 상·하 동시 검 연출을 한 타격으로 합산한다. 40회를 각각 2배 적용하지 않는다.
- 범위는 적의 지면 위치와 원 중심의 유클리드 거리 `dx² + dy² <= 300²`다. 매회 재판정하므로 나간 적은 이후 타격을 피하고, 새로 들어온 적은 이후 타격을 받는다. 전후 방향·시야·장애물로 원 내부 타격을 거절하지 않는다.

이전의 공격력 0.3, 3초 15회 수치는 폐기하고 2026-10-13 최신 확정값을 적용한다.

## 구현 선택

아래는 사용자 밸런스 지정과 별개인 연출·운영 선택이다.

- 마법진은 발동 순간의 플레이어 위치에 고정한다. 소환부터 폭발까지 이동·다른 공격 입력을 제한하는 기존 시전 규칙을 따른다. 피해 면역은 추가하지 않는다. 외부 밀치기에 의한 위치 변화가 마법진을 옮기지는 않는다.
- 0–0.70초: 바닥·상공 마법진 동시 생성. 회전 방향과 색을 구별한다.
- 0.70–4.70초: 검 세례. 첫 타격은 0.80초, 마지막 40번째는 4.70초다.
- 4.70–5.25초: 두 마법진이 중앙으로 이동하며 축소·합체한다. 추가 타격 없음.
- 5.25초: 폭발 피해 1회, 이동·공격 제한 해제. 5.80초까지 폭발 잔광을 표시한다.
- 공격력과 INT는 발동 시 스냅샷이며 장비 공격력을 포함한다. 각 타격·폭발 피해를 기존 방식처럼 정수 반올림한다. 시전 중 레벨 상승이 해당 시전의 피해를 바꾸지 않는다.
- 메뉴·일시정지는 기존 게임 시간 정지를 따른다. 사망·귀환·초기화·맵 이동·이벤트 제어 잠금은 궁극기를 제거한다. MP·쿨타임은 되돌리지 않는다.
- 적의 기존 이야기 보호용 최소 체력 규칙을 유지한다. NPC·종·장치 등 적 목록 외의 오브젝트는 이 범위 공격의 대상이 아니다.

## 직접 그리기

마법진·합체·폭발은 **OpenGL 3.3 절차적 셰이더**로 그린다. 판정 경계는 Renderer의 도형 배치를 사용한다. 검은 아래 Blender 제작 시트로 교체했다. 기존 `mage-3` 캐릭터 몸 모션을 재사용하고 기존 근접 효과 레이어는 끈다.

### Blender 검 개선 — 2026-10-20

검날의 다이아몬드 단면·밝은 날·발광 중심선·청색 마나 가드·손잡이 감개·마나 결정 폼멜을 Blender에서 제작했다. 장축 회전 16프레임을 96×192 RGBA로 렌더링한 뒤 32×64 셀로 축소하고, 마법사 이동 시트에서 선택한 청색·청록 팔레트로 디더링 없이 마감한다. 검 전체(날·가드·손잡이·폼멜)는 응축된 파란 마나로 표현하며 금색·갈색·은색 재질은 사용하지 않는다. 실제 알파는 0/255이며 게임의 최근접 필터로 표시한다. 캐릭터 자체는 수정하지 않는다.

- [Blender 원본](../../SimpleGame/Assets/Effects/MageSwordSong/sword-song.blend), [시트](../../SimpleGame/Assets/Effects/MageSwordSong/sword-turntable.png), [변환 설정](../../SimpleGame/Assets/Effects/MageSwordSong/settings.json).
- [Blender 제작 도구](../../tools/effects/blender/build_sword_song.py)를 Blender의 `--background --python`으로 실행한 뒤 [시트 변환 도구](../../tools/effects/blender/pack_sword_song.py)를 Pillow가 있는 Python으로 실행한다. 원시 프레임은 Git 제외 경로 `output/mage-sword-song-blender-v3/raw/`에 생성한다.
- [MageUltimateSwordView.h](../../SimpleGame/src/ui/MageUltimateSwordView.h)의 전용 클래스가 시트 프레임·검끝 피벗·상하 반전·작은 기울기·계단형 잔상을 담당한다. `MageUltimateView`는 기존 타격 순번으로 회전 위상을 분산하고 이동 진행률의 제곱으로 가속한다. 32×64 셀의 1.5배 표시는 구현 선택이다.
- `std::sin`으로 검별 기울기를 분산하고 `std::round`로 화면 좌표를 정리한다. 기존 `ImageQuad` 텍스처 캐시와 PNG 복사 타깃을 사용한다.
- 40회 타격 시점·피해·상하 각각 2개·마법진·합체·폭발은 유지한다. Blender 렌더링과 시트 비교는 수행했으며 게임 빌드·실행·성능 검증은 수행하지 않았다. 아래 기존 WebGL 미리보기는 이전 도형 검을 표시하므로 이번 검의 확인 자료로 사용하지 않는다.

- 마법진: 거리 함수와 픽셀 미분으로 가장자리를 부드럽게 처리한다. 6겹 동심원, 64개 눈금, 28개 기하 룬, 이중 삼각형, 6개 보석 문양, 내부 꽃잎과 반대 방향 궤도·회전 광채를 겹친다.
- 합체: 두 마법진 사이의 나선형 마력 흐름이 빨라지며 수렴 고리와 작은 고휘도 중심으로 응축된다.
- 폭발: 절차적 3D 노이즈를 쓰는 발광 구체의 껍질을 깊이 방향으로 18회 누적해 두께와 흐름을 표현한다. 중심 섬광, 두 번의 압력파, 연속된 방사형 빛줄기와 잔광을 겹친다. 장면 깊이 버퍼를 사용하는 3D 지형 충돌은 아니다.
- 발광: 1.0을 넘는 선형 밝기를 기존 RGBA16F 장면 버퍼에 더하고 기존 블룸·노출·톤 매핑을 사용한다. HDR10 디스플레이 출력이 아니라 SDR 화면용 HDR 중간 합성이다. 게임 F1/F2 후처리·블룸 설정에 따라 결과가 달라진다.
- 굴절: 폭발 압력파 부근에서 장면 샘플 위치를 최대 7px 이동한다. UI를 그리기 전에 합성하며 매 프레임 충격파 상태를 초기화한다. 피해 범위에는 영향이 없다.

효과 프로그램은 초기화 시 한 번 준비한다. 실패하면 기존 도형 마법진·폭발로 되돌아간다. 마법진·폭발용 추가 프레임버퍼는 만들지 않는다. 검 시트는 기존 이미지 캐시에 텍스처 한 장으로 적재한다. 효과 영역과 화면의 교집합만 그리며, 기존 잘라내기 영역·블렌딩을 복원한다. 후처리 타깃을 사용할 수 없는 직접 렌더링 경로에서는 자체 톤 매핑으로 발광을 표시하고 배경 굴절은 생략한다.

바닥 경계는 실제 반경 300을 `SceneCamera::GroundDepthScale`로 투영한다. 상공 마법진은 문양이 보이도록 화면 쪽으로 기울인 시안이다. 검은 각각 해당 원 내부에서 출발한다. 한 타격에 상·하 각각 2개의 장식 검을 그리지만 추가 피해 객체는 생성하지 않는다. 합체 중에도 지면에는 실제 판정 범위 선이 남는다. 폭발 잔광의 크기는 추가 공격 범위가 아니다.

## 코드 책임과 흐름

| 파일 | 역할 |
| --- | --- |
| [MageUltimate.h](../../SimpleGame/src/combat/MageUltimate.h), [구현](../../SimpleGame/src/combat/MageUltimate.cpp) | 시전 시간, 고정 중심, 40회 타격 순번, 폭발·취소 상태 |
| [EnemySystem.cpp](../../SimpleGame/src/combat/EnemySystem.cpp) | 원 내부 살아 있는 적 전원에게 타격, 사망 위치 반환 |
| [GameSessionAreaAttack.cpp](../../SimpleGame/src/app/GameSessionAreaAttack.cpp) | 월드→지역 좌표 변환과 각 지역 적·보상·전리품 연결 |
| [MageUltimateView.cpp](../../SimpleGame/src/ui/MageUltimateView.cpp) | 지면/상공 레이어, 검·룬·합체 폭발, 기존 시전자 동작 |
| [ArcaneEffect.cpp](../../SimpleGame/src/rendering/ArcaneEffect.cpp), [셰이더](../../SimpleGame/src/rendering/ArcaneEffectShaders.h) | 효과 프로그램 수명·제한 영역·HDR 합성 및 마법진/응축/폭발 계산 |
| [RendererArcane.cpp](../../SimpleGame/src/rendering/RendererArcane.cpp), [PostProcessing.cpp](../../SimpleGame/src/rendering/PostProcessing.cpp) | 기존 도형 배치와 효과 제출 순서, 장면 충격파 굴절 |
| [SkillCatalog.cpp](../../SimpleGame/src/player/SkillCatalog.cpp), [SkillSystem.cpp](../../SimpleGame/src/player/SkillSystem.cpp) | 스킬 수치·MP·쿨타임과 검/폭발 피해 스냅샷 |
| [PrototypeWorld.cpp](../../SimpleGame/src/app/PrototypeWorld.cpp), [SceneRenderer.cpp](../../SimpleGame/src/rendering/SceneRenderer.cpp) | 입력→시전→시간 갱신, 초기화와 렌더 순서 |
| [SkillLayout.h](../../SimpleGame/src/player/SkillLayout.h), [SkillView.cpp](../../SimpleGame/src/ui/SkillView.cpp) | 7개 마법사 스킬을 4열·2행으로 표시하며 동일 배치로 드래그 판정 |
| [MageUltimateVerification.cpp](../../SimpleGame/src/verification/MageUltimateVerification.cpp) | 수치·자원·원형 판정·40회·타격 경계·폭발·취소 회귀 소스 |

`SkillUseRequest`는 검 세례 피해와 최종 폭발 피해를 나누어 전달한다. `MageUltimate`는 `double` 시간과 정수 이벤트 순번으로 여러 타격 시점을 한 프레임에 넘겨도 누락·중복을 막는다. 늦은 프레임에서는 지나간 각 타격을 그 갱신 시점의 적 위치로 판정한다. 과거 적 위치를 재구성하지는 않는다.

기존 `std::vector` 적 저장소와 보상 목록을 재사용한다. 이미 죽은 적은 다음 검사에서 제외하므로 처치 보상이 중복되지 않는다. 피해용 검 160개를 별도로 보관하지 않고 고정 루프에서 장식 위치를 계산한다. 수명·진행률은 `std::min`, `std::clamp`로 제한한다.

`ArcaneEffect`는 GPU 자원을 소유하고 소멸 시 반환한다. 복사를 금지해 중복 해제를 방지하며 `enum class Kind`로 세 효과를 구분한다. 렌더 순서는 바닥 경계·마법진 → 월드 액터·시전자 → 마력 흐름·상공 마법진·검 또는 폭발 → 후처리 → UI다.

## 미리보기와 검증

[WebGL2 미리보기](../../output/mage-sword-song-v1/preview.html)는 정지·재생·배속·시간 탐색·단계 선택을 제공한다. 게임 캡처가 아니며 기존 캐릭터의 별도 시안 모션과 확인용 배경을 사용한다. 게임과 같은 효과 셰이더를 [내보내기 도구](../../tools/effects/export_arcane_preview.py)로 복사하므로 셰이더 수정 후 `python tools/effects/export_arcane_preview.py`로 갱신한다. 캐릭터 원본 PNG는 변경 없이 데이터 URL로 포함해 로컬 파일에서도 Canvas→WebGL 업로드가 가능하도록 한다.

미리보기는 WebGL2와 부동소수점 색상 버퍼 지원 여부를 표시한다. HDR 미지원이면 일반 정밀도로 표시하며, WebGL2·셰이더·타깃 준비 실패 시 Canvas 도형으로 대체한다. 컨텍스트를 잃으면 안내를 표시한다. 미리보기의 배경·캐릭터·후처리 설정은 실제 게임과 다르므로 동일한 최종 화면이나 성능을 보장하지 않는다.

정적 서식·구문·프로젝트 등록·참조를 검토한다. **이번 요청에서는 게임 빌드, 실행, 회귀 검사 실행, 미리보기 재생 및 화면·성능 확인을 하지 않았다.** 수동 회귀 명령은 빌드 후 `SimpleGame.exe --verify-mage-ultimate`다. 회귀 소스 추가가 통과를 뜻하지 않는다.

실제 게임 확인 항목: Lv.19/20, MP49/50, 같은 스킬 복수 슬롯 쿨타임 공유, 원 경계·범위 출입, 작은/큰 프레임 간격, 막힌 지형 안의 적, 발동 중 사망·귀환·메뉴·맵 전환, 활성/비활성 지역 경계, 7번째 카드 등록, 좌우 캐릭터 동작. 시각적 완성도와 프레임 비용은 실제 실행에서 추가 확인해야 한다.
