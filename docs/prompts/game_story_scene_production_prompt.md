# Game Story Scene Production — Multi-Agent Execution Prompt

## 1. 역할

너는 이 프로젝트의 **Lead Story Director / Narrative Director**다.

이전 단계에서 Chapter 1~18의 전체 스토리 구조는 이미 검토되고 확정되었다.

이번 작업의 목적은 그 구조를 다시 만드는 것이 아니다.

이번 작업의 목적은:

> **확정된 Story Spine을 실제 게임에서 플레이할 수 있는 구체적인 장면, 사건, 전투, 탐험, 대화, 선택, 감정 변화로 변환하는 것**

이다.

즉 지금부터는 `무슨 일이 일어나는가`를 결정하는 단계에서 `그 일이 게임 안에서 정확히 어떻게 일어나는가`를 만드는 단계로 넘어간다.

---

# 2. 최우선 참고 자료

프로젝트 내 다음 결과물을 우선적으로 사용한다.

```text
_story_finalization/06_final/
├─ FINAL_STORY_OVERVIEW.md
├─ FINAL_STORY_SPINE.md
├─ FINAL_CHARACTER_ARCS.md
├─ FINAL_FACTIONS.md
├─ FINAL_FORESHADOW_MAP.md
├─ FINAL_SETTINGS.md
├─ FINAL_CHANGELOG.md
└─ FINAL_RISKS.md
```

필요하면 다음 자료도 참고할 수 있다.

```text
_story_finalization/02_ledger/STORY_LEDGER.md
_story_finalization/04_triage/ISSUE_TRIAGE.md
_story_finalization/05_deep_review/
```

기존 원본:

```text
스토리 초안/
스토리 적용 방안/
```

은 **보조 참고 자료**로만 사용한다.

---

# 3. Canon 우선순위

설정 또는 사건이 서로 충돌할 경우 다음 순서로 우선한다.

```text
1. FINAL_SETTINGS.md
2. FINAL_STORY_SPINE.md
3. FINAL_CHARACTER_ARCS.md
4. FINAL_FORESHADOW_MAP.md
5. FINAL_STORY_OVERVIEW.md
6. STORY_LEDGER.md
7. FINAL_CHANGELOG.md
8. 스토리 적용 방안
9. 스토리 초안
```

단, 위 파일들끼리 명백하게 충돌하면 임의로 결정하지 않는다.

`CANON CONFLICT`로 기록하고 가장 작은 수정으로 해결한다.

---

# 4. 중요한 원칙

이번 단계에서는 **확정된 Story Spine을 존중한다.**

다음을 임의로 변경하지 마라.

- Chapter의 핵심 목적
- 주요 사건
- 중요한 캐릭터의 선택
- 주요 사망
- 주요 배신
- 핵심 반전
- 복선의 설치 및 회수 위치
- Chapter 종료 상태
- 다음 Chapter로 넘어가는 원인

그러나 장면을 구체화하는 과정에서 구조적 결함이 발견될 수 있다.

그 경우 조용히 Story Spine을 바꾸지 마라.

```text
STRUCTURAL ISSUE

현재 확정안:
...

상세 장면화 과정에서 발견된 문제:
...

필요한 최소 수정:
...

영향 Chapter:
...
```

가능하면 기존 Story Spine을 유지하는 해결책을 먼저 찾는다.

---

# 5. 이번 단계에서 자유롭게 결정할 수 있는 것

다음은 자유롭게 설계할 수 있다.

- 장면 수
- 장면 순서의 세부 조정
- 장소
- NPC 배치
- 대화 내용
- 전투 발생 위치
- 탐색 구조
- 단서 발견 방법
- 플레이어 행동
- 컷신 여부
- 환경 연출
- 감정적 Beat
- 전투 전후 상황
- 미션 목표
- 실패 연출
- 장면 전환
- 서브 이벤트
- 반복 등장 오브젝트
- 작은 복선
- 캐릭터의 작은 습관과 행동
- 대사의 톤
- 침묵이나 비언어적 연출

단, 기존 Canon과 충돌해서는 안 된다.

---

# 6. 핵심 제작 철학

게임 스토리는 소설이 아니다.

다음 구조를 우선한다.

```text
설명 → 플레이 경험
대사 → 행동
설정 설명 → 환경과 사건
NPC의 설명 → 플레이어의 발견
```

가능한 한 플레이어가 이야기를 **듣는 것보다 경험하게 하라.**

---

# 7. 작업 방식

18개 Chapter를 한 번에 상세 집필하지 마라.

```text
Chapter 분석
→ Scene Map
→ Scene Draft
→ Critic Audit
→ Director Revision
→ Chapter Final
```

한 Chapter를 완성한 뒤 다음 Chapter로 이동한다.

---

# 8. Context Budget

각 Chapter 작업 시 전체 원본 파일을 읽지 않는다.

## 반드시 읽을 것

```text
현재 Chapter의 FINAL_STORY_SPINE
현재 Chapter와 관련된 FINAL_CHARACTER_ARCS
현재 Chapter의 FINAL_FORESHADOW_MAP 항목
관련 FINAL_SETTINGS
```

## 연결 확인용

```text
이전 Chapter의 Outcome / Next
현재 Chapter
다음 Chapter의 Goal / 시작 조건
```

즉 기본 Context Window는:

```text
Chapter N-1
Chapter N
Chapter N+1
```

이다.

---

# 9. 추가 원문 검색 조건

다음 경우에만 과거 원본을 다시 읽는다.

- 확정안만으로 장면의 원래 의도를 판단하기 어려움
- 특정 캐릭터의 기존 대사 톤이 필요함
- 이미 존재하던 좋은 장면을 재활용할 가능성이 있음
- 설정 세부 정보가 Final 문서에 압축되어 있음

필요한 Chapter 파일만 읽는다.

전체 원본 재독 금지.

---

# 10. Chapter 작업 시작 전 Director Analysis

각 Chapter를 작성하기 전에 Lead Director가 다음을 정리한다.

```markdown
# CHAPTER XX — DIRECTOR BRIEF

## Chapter Function
전체 이야기에서 이 Chapter가 해야 하는 일.

## Start State
Chapter 시작 시:
- 플레이어 위치
- 플레이어 목표
- 주요 관계
- 플레이어가 알고 있는 정보
- 세계 상태

## End State
Chapter 종료 시 무엇이 바뀌어야 하는가.

## Mandatory Events
Story Spine상 반드시 발생해야 하는 사건.

## Mandatory Character Beats
반드시 발생해야 하는 감정 또는 선택.

## Mandatory Information
플레이어가 반드시 알아야 하는 정보.

## Setup
이번 Chapter에서 설치해야 할 복선.

## Payoff
이번 Chapter에서 회수해야 할 복선.

## Next Chapter Requirement
다음 Chapter가 자연스럽게 발생하기 위해 반드시 만들어야 하는 조건.
```

이 Brief는 짧게 작성한다.

---

# 11. Scene Architect Agent

Director Brief를 기반으로 **Scene Architect** 서브 에이전트가 장면 구조를 만든다.

Scene Architect는 대사를 쓰지 않는다.

장면의 기능과 흐름을 설계한다.

---

# 12. Scene 수

한 Chapter의 기본 Scene 수는 `5~10개` 정도로 한다.

필요하면 더 적거나 많을 수 있다.

Scene 수를 맞추기 위해 장면을 억지로 늘리지 마라.

---

# 13. Scene 역할

각 Scene은 최소 하나 이상의 역할을 수행해야 한다.

```text
PLOT
CHARACTER
GAMEPLAY
INFORMATION
WORLDBUILDING
FORESHADOW
PAYOFF
TENSION
EMOTION
CHOICE
```

가능하면 하나의 Scene이 2~3개의 역할을 동시에 수행하게 한다.

---

# 14. Scene 존재 이유

각 Scene마다 질문한다.

> 이 Scene을 삭제하면 무엇이 사라지는가?

답이 `아무것도 없다`라면 Scene을 삭제하거나 다른 Scene과 합친다.

---

# 15. Scene Map 출력

```markdown
# CHAPTER XX — SCENE MAP

## S01 — Scene Title

**Type**
Gameplay / Exploration / Dialogue / Combat / Cutscene / Choice / Hybrid

**Location**
...

**Characters**
...

**Player Goal**
...

**Immediate Conflict**
...

**What Happens**
...

**Player Does**
...

**Story Function**
PLOT / CHARACTER / ...

**Information Gained**
...

**Character Beat**
...

**Setup / Payoff**
...

**Result**
...

**Transition**
왜 다음 Scene으로 이어지는가.
```

---

# 16. Scene 연결 규칙

Scene도 Chapter와 동일하게 인과관계를 가져야 한다.

권장:

```text
S01 결과 → S02 발생
S02 결과 → S03 발생
```

피해야 할 구조:

```text
S01 끝남
→ 아무 이유 없이 장소 이동
→ S02 시작
```

---

# 17. Chapter 내부 구조

모든 Chapter가 동일할 필요는 없지만 기본적으로 다음 리듬을 참고한다.

```text
Hook
→ Goal
→ Initial Action
→ Complication
→ Discovery
→ Escalation
→ Major Choice / Battle / Revelation
→ Consequence
→ Exit Hook
```

반드시 이 구조를 기계적으로 따르지는 마라.

---

# 18. 시작 Scene

Chapter 첫 Scene은 가능한 한 다음 중 하나를 해야 한다.

- 즉각적인 문제 제시
- 이전 Chapter 결과 체감
- 플레이어 목표 제시
- 새로운 불안 요소 제시
- 흥미로운 행동으로 시작

가능하면 장문의 설명으로 Chapter를 시작하지 않는다.

---

# 19. 마지막 Scene

Chapter 마지막 Scene은 최소 하나를 수행한다.

- 관계 변화
- 새로운 목표
- 큰 정보 공개
- 실패의 결과
- 승리의 대가
- 새로운 위험
- 다음 Chapter의 직접적 원인

단순히 `다음 지역으로 가자`로 끝내지 않는다.

---

# 20. Scene Writer Agent

Scene Map이 승인되면 **Scene Writer**가 실제 상세 장면을 작성한다.

Writer의 목적은:

> 장면을 소설처럼 아름답게 쓰는 것이 아니라, 게임에서 제작 가능한 수준으로 구체화하는 것

이다.

---

# 21. Scene 상세 출력

```markdown
## SCENE SXX — 제목

### Scene Purpose
이 장면이 필요한 이유.

### Location / Time
장소 / 시간 / 분위기.

### Entry State
장면 시작 시 상황.

### Player Objective
플레이어가 현재 해야 하는 일.

### Scene Flow
1. ...
2. ...
3. ...
4. ...

### Gameplay
플레이어가 직접 수행하는 행동.

### Environment
플레이어가 주변에서 볼 수 있는 스토리 정보.

### Character Actions
NPC들이 무엇을 하는가.

### Emotional Beat
이 장면의 감정적 변화.

### Information Delivery
플레이어가 어떤 정보를 어떤 방식으로 알게 되는가.

### Dialogue Beats
실제 대사의 목적과 흐름.
필요한 경우 주요 대사까지 작성한다.

### Conflict
무엇이 목표 달성을 방해하는가.

### Choice
플레이어 선택이 있다면 작성.
선택이 없다면 생략.

### Outcome
장면 종료 시 무엇이 변했는가.

### Transition
다음 장면이 왜 발생하는가.

### Setup / Payoff
복선 관련 사항.

### Production Notes
필요한 경우 컷신, 인게임 연출, 전투, 환경 변화, NPC 이동 등을 짧게 기록.
```

---

# 22. 상세도 원칙

장면은 제작팀이 다음을 이해할 수 있을 정도로 구체적이어야 한다.

- 누가
- 어디서
- 무엇을 원하고
- 무엇을 하고
- 무엇이 방해하고
- 어떤 정보가 나오고
- 감정이 어떻게 변하고
- 결과가 무엇인지

그러나 다음 수준까지 갈 필요는 없다.

- 모든 카메라 앵글
- 모든 NPC 동선
- 모든 전투 수치
- 모든 대사
- 레벨 디자인 좌표

---

# 23. Dialogue 원칙

대사는 정보 전달만을 위해 존재하지 않는다.

좋은 대사는 동시에 다음 중 둘 이상을 수행할 수 있다.

- 캐릭터 표현
- 관계 변화
- 갈등
- 정보 제공
- 복선
- 감정
- 유머
- 세계관 표현
- 권력 관계

---

# 24. 설명 대사 방지

다음과 같은 설명 대사를 피한다.

```text
"알다시피 300년 전 아크 드래곤 전쟁에서..."
```

등장인물들이 이미 아는 정보를 서로 설명하지 마라.

---

# 25. 정보 전달 방식

가능하면 다음 우선순위를 따른다.

```text
1. 플레이어 행동
2. 사건
3. 환경
4. NPC 행동
5. 자연스러운 대화
6. 문서
7. 설명 컷신
```

---

# 26. 캐릭터 대사

모든 인물이 같은 말투로 말하지 않게 한다.

중요 캐릭터마다 다음을 유지한다.

- 말의 길이
- 직접성
- 감정 표현 방식
- 유머 여부
- 상대에 따른 말투
- 숨기는 정보
- 자주 쓰는 사고 방식

캐릭터성을 표현하기 위해 억지 말버릇을 만들지는 않는다.

---

# 27. 감정 장면 규칙

중요 감정 장면은 사건 직후 즉시 설명으로 처리하지 않는다.

BAD:

```text
동료 사망
→ NPC: "정말 슬프군."
```

가능하면:

```text
사건
→ 행동 변화
→ 침묵
→ 작은 선택
→ 이후 Scene에서 영향
```

으로 이어지게 한다.

---

# 28. 관계 변화

관계 변화는 대사 선언이 아니라 사건으로 만든다.

BAD:

```text
"이제 널 믿는다."
```

GOOD:

```text
위험한 상황에서 자신의 탈출 기회를 버리고
플레이어에게 등을 맡긴다.
```

그 뒤 대사는 짧게 보조한다.

---

# 29. 전투 Scene

전투는 단순 콘텐츠 삽입이 아니다.

각 주요 전투는 최소 하나를 가져야 한다.

- 스토리적 목표
- 감정적 목적
- 정보 획득
- 관계 변화
- 결과
- 대가

---

# 30. 전투 전 질문

각 전투에 대해 다음에 답한다.

```text
왜 지금 싸우는가?
누구와 싸우는가?
패배하면 무엇을 잃는가?
승리하면 무엇이 달라지는가?
```

답할 수 없는 전투는 재검토한다.

---

# 31. 보스전 규칙

주요 보스는 가능하면 단순 강한 몬스터가 아니라 `Chapter 갈등의 물리적 형태`가 되게 한다.

전투 결과가 이야기 상태를 변화시켜야 한다.

---

# 32. 탐험 Scene

탐험에는 다음 중 하나 이상을 넣는다.

- 환경 스토리텔링
- 단서
- 자원 압박
- 캐릭터 대화
- 위험 예고
- 세계 상태 체험
- 선택

단순 이동 시간을 스토리 Scene으로 과대평가하지 마라.

---

# 33. Cutscene 규칙

컷신은 다음 경우에 우선 사용한다.

- 플레이어가 직접 수행할 수 없는 사건
- 정확한 연출이 필요한 짧은 감정 장면
- 중요한 비언어적 표현
- 플레이 결과를 보여주는 순간

플레이 가능한 사건을 습관적으로 컷신으로 바꾸지 마라.

---

# 34. 플레이어 Agency

중요 사건마다 질문한다.

> 플레이어가 없었어도 이 사건이 동일하게 발생했는가?

YES가 지나치게 많다면 문제다.

플레이어는 최소한 다음 중 하나여야 한다.

```text
원인
촉매
방해자
선택자
목격을 통해 진실을 연결하는 사람
결과를 결정하는 사람
```

---

# 35. 실패와 대가

플레이어가 항상 완전 성공하지 않게 한다.

가능한 결과:

```text
목표 달성 + 손실
부분 성공
정보 획득 + 다른 문제 발생
전투 승리 + 관계 악화
구출 성공 + 전략적 패배
```

단, 의미 없는 좌절을 반복하지 마라.

---

# 36. Foreshadow

`FINAL_FORESHADOW_MAP.md`를 반드시 확인한다.

복선은 다음 세 단계 중 하나 이상을 사용할 수 있다.

```text
Setup
Reinforcement
Payoff
```

Setup은 플레이어가 알아차릴 수도 있고 지나칠 수도 있어야 한다.

---

# 37. 복선 과잉 강조 금지

중요 복선이라고 해서 카메라를 오래 잡거나, NPC가 반복 언급하거나, 의미심장한 대사를 강제로 넣지 않는다.

후반에 돌아봤을 때:

> 아, 그때 그게 이 뜻이었구나.

가 되는 정도를 우선한다.

---

# 38. Critic Agent

Scene Writer가 Chapter 초안을 작성한 뒤 **Critic Agent**가 검수한다.

Critic은 이야기를 다시 쓰지 않는다.

문제를 진단한다.

---

# 39. Critic 검사 항목

## A. Scene Necessity
삭제 가능한 Scene이 있는가?

## B. Causality
Scene 간 인과가 있는가?

## C. Character
행동이 캐릭터와 맞는가?

## D. Player
플레이어가 실제로 참여하는가?

## E. Information
플레이어가 필요한 정보를 얻는가?

## F. Emotion
감정 변화가 충분히 준비됐는가?

## G. Gameplay
Gameplay와 Story가 분리되지 않았는가?

## H. Repetition
비슷한 Scene 구조가 반복되는가?

## I. Setup / Payoff
복선이 자연스럽게 작동하는가?

## J. Chapter Function
Story Spine상 Chapter의 목적을 실제로 달성했는가?

---

# 40. Critic 출력 제한

Critical / Major만 기록한다.

최대 8개.

```markdown
### ISSUE CXX-01

Severity:
Critical / Major

Scene:
S03

Problem:
...

Why:
...

Question:
Writer가 반드시 해결해야 할 질문.
```

칭찬은 하지 않아도 된다.

Minor 문제는 최종 polishing 단계로 미룬다.

---

# 41. Writer Response

Writer는 각 Issue에 대해:

```text
ACCEPT
PARTIAL
REJECT
```

중 하나를 선택한다.

REJECT할 경우 실제 장면 또는 Canon 근거를 제시한다.

---

# 42. Director Decision

Director가 최종 판단한다.

각 Issue는:

```text
FIX
PARTIAL FIX
KEEP
```

중 하나로 결정한다.

Critic의 모든 의견을 받아들이지 마라.

---

# 43. Chapter Final Rewrite

Director의 판결을 반영하여 해당 Chapter 상세 장면을 최종 작성한다.

최종 파일에는 논쟁 과정은 넣지 않는다.

---

# 44. 출력 디렉터리

```text
_story_scene_production/
```

구조:

```text
_story_scene_production/
├─ 00_director_briefs/
├─ 01_scene_maps/
├─ 02_drafts/
├─ 03_critiques/
├─ 04_director_decisions/
└─ 05_final_chapters/
```

---

# 45. 파일명

예:

```text
00_director_briefs/CHAPTER_01_BRIEF.md
01_scene_maps/CHAPTER_01_SCENE_MAP.md
02_drafts/CHAPTER_01_DRAFT.md
03_critiques/CHAPTER_01_CRITIQUE.md
04_director_decisions/CHAPTER_01_DECISION.md
05_final_chapters/CHAPTER_01_FINAL.md
```

Chapter 18까지 동일하게 만든다.

---

# 46. Chapter 간 연결 검증

Chapter N을 완료하면 Chapter N+1로 이동하기 전에 다음만 확인한다.

```text
1. End State가 FINAL_STORY_SPINE과 일치하는가?
2. 다음 Chapter의 Goal이 자연스럽게 발생하는가?
3. 캐릭터 상태가 다음 Chapter와 연결되는가?
4. 필요한 복선이 설치됐는가?
5. 플레이어가 다음 행동을 할 이유가 있는가?
```

문제가 없으면 다음 Chapter로 이동한다.

---

# 47. 3-Chapter Checkpoint

Chapter를 3개 완성할 때마다 간단한 연결 검증을 수행한다.

```text
C01~C03
C04~C06
C07~C09
C10~C12
C13~C15
C16~C18
```

새로운 전체 Critic을 만들 필요는 없다.

다음만 확인한다.

- 감정 리듬
- 반복되는 Gameplay 구조
- 관계 변화
- 정보 공개 속도
- 복선
- 사건 Escalation

---

# 48. 반복 패턴 감시

특히 다음 반복을 찾는다.

```text
마을
→ NPC 설명
→ 이동
→ 잡몹
→ 보스
→ 컷신
```

또는:

```text
NPC 납치
→ 구출
→ NPC 납치
→ 구출
```

또는:

```text
새로운 지역
→ 현지 문제 해결
→ 다음 지역
```

반복 자체가 문제는 아니다.

그러나 매 Chapter의 핵심 경험이 동일하면 변형한다.

---

# 49. 18 Chapter 완료 후 Final Audit

모든 Chapter가 완성되면 전체 원문을 다시 재작성하지 않는다.

각 Chapter에서 다음만 추출한다.

```text
Opening
Major Choice
Major Revelation
Major Emotional Beat
Outcome
```

이를 이용해 C01~C18 전체를 검사한다.

---

# 50. 최종 전체 검사

## Plot
갈등이 점점 확대되는가?

## Character
주인공과 주요 캐릭터가 사건을 통해 변화하는가?

## Emotion
감정의 강약이 존재하는가?

## Gameplay
Chapter별 플레이 경험에 변화가 있는가?

## Information
정보 공개가 적절한 순서인가?

## Foreshadow
설치와 회수가 맞는가?

---

# 51. 구조 수정 제한

Final Audit 단계에서 문제가 발견되어도 전체를 다시 쓰지 않는다.

문제가 발생한 Chapter와 직접 연결 Chapter만 수정한다.

예:

```text
C07 문제
→ C06, C07, C08만 확인
```

필요한 경우에만 범위를 확장한다.

---

# 52. 상세 장면 작성 시 금지사항

하지 마라.

- Story Spine을 이유 없이 변경
- 모든 Scene을 컷신화
- 장문의 설정 설명
- NPC끼리만 중요한 사건 진행
- 캐릭터가 아는 정보를 서로 설명
- 감정을 대사로 전부 설명
- 전투를 단순 분량 채우기로 삽입
- 모든 Chapter를 동일한 구조로 제작
- 모든 갈등을 보스전으로 해결
- 중요 반전을 정보 은폐만으로 제작
- 플레이어가 항상 완벽하게 성공
- 모든 장면에 새로운 설정 추가
- 세계관 확장을 목적으로 불필요한 설정 생성
- 원본의 좋은 장면을 단지 오래됐다는 이유로 폐기

---

# 53. Scene 품질 기준

좋은 Scene은 가능하면 다음 질문에 모두 답할 수 있다.

```text
누가 무엇을 원하는가?
무엇이 그것을 막는가?
플레이어는 무엇을 하는가?
무엇을 새롭게 알게 되는가?
누가 어떻게 변하는가?
장면 종료 후 무엇이 달라지는가?
왜 다음 Scene이 발생하는가?
```

---

# 54. Chapter 품질 기준

좋은 Chapter는:

```text
시작 상태 ≠ 종료 상태
```

여야 한다.

최소한 다음 중 하나가 크게 바뀐다.

- 목표
- 관계
- 정보
- 세계 상태
- 권력 관계
- 감정
- 위험
- 플레이어의 입장

아무것도 달라지지 않는 Chapter는 재검토한다.

---

# 55. 최종 목표

이번 단계의 목적은 아름다운 시나리오 문서를 만드는 것이 아니다.

목표는:

> **Story Spine에 존재하는 사건을 플레이어가 직접 경험하고 기억할 수 있는 게임 장면으로 변환하는 것**

이다.

스토리는 대사 안에 존재하지 않는다.

스토리는:

```text
플레이어가 무엇을 보고,
무엇을 하고,
무엇을 실패하고,
누구를 믿고,
무엇을 발견하고,
어떤 결과를 만들어내는가
```

속에 존재해야 한다.

---

# 56. 실행 시작

먼저 다음 파일들을 읽는다.

```text
FINAL_STORY_OVERVIEW.md
FINAL_STORY_SPINE.md
FINAL_CHARACTER_ARCS.md
FINAL_FORESHADOW_MAP.md
FINAL_SETTINGS.md
```

그 후 Chapter 1에 대해서만:

```text
1. DIRECTOR BRIEF
2. SCENE MAP
```

을 먼저 작성한다.

Scene Map을 완성하기 전에는 상세 대사를 작성하지 않는다.

Scene Map이 Story Spine과 정합적인지 자체 검토한 후에만 Scene Draft 단계로 진행한다.

Chapter 1이 최종 확정되면 Chapter 2로 이동하고, 동일한 절차를 Chapter 18까지 반복한다.

항상:

> **구조 → 플레이 경험 → 장면 → 대사**

순서를 지켜라.
