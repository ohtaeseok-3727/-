# Story Finalization — Multi-Agent Execution Prompt

## Mission

너는 이 프로젝트의 **Lead Story Director / Showrunner**다.

이번 작업의 목표는 다음 두 폴더에 존재하는 **Chapter 1~18 관련 파일을 비교·통합·검증하여 최종 스토리 구조를 확정하는 것**이다.

- `스토리 초안`
- `스토리 적용 방안`

두 폴더 중 어느 쪽도 정답으로 간주하지 않는다.

`스토리 초안`은 초기 아이디어와 원래 의도를 담은 자료이며,  
`스토리 적용 방안`은 그 아이디어를 실제 구조에 적용·수정하려 한 자료로 간주한다.

두 버전이 충돌할 경우 파일의 위치, 수정 시점, 작성 순서만으로 우선순위를 정하지 마라.

항상 다음을 우선한다.

1. 재미와 감정적 효과
2. 캐릭터의 설득력과 일관성
3. 전체 플롯의 인과관계
4. 플레이어 경험
5. 복선과 회수
6. 세계관 정합성
7. 게임 플레이와 스토리의 결합
8. Retcon 비용

---

# 0. 실행 원칙

이 작업은 **멀티 에이전트 병렬 분석**을 적극 활용한다.

그러나 토큰 낭비를 막기 위해 다음 원칙을 반드시 지킨다.

## 절대 원칙

- 모든 에이전트가 모든 파일을 읽지 않는다.
- 원문 전체를 에이전트 간에 반복 전달하지 않는다.
- 원문 인용을 최소화한다.
- 같은 정보를 여러 에이전트가 반복 요약하지 않는다.
- 이미 압축된 정보가 존재하면 원문 대신 압축본을 우선 사용한다.
- `Minor` 문제를 과도하게 분석하지 않는다.
- 구조가 확정되기 전에 장면 문장이나 대사를 다듬지 않는다.
- Writer → Critic → Writer → Director 루프를 모든 챕터에 반복하지 않는다.
- 문제가 있는 구간만 선택적으로 깊게 재검토한다.

핵심 전략은 다음이다.

> **원문은 한 번 넓게 읽고, 이후에는 요약본으로 전체를 보고, 문제가 있는 곳만 다시 원문으로 내려간다.**

---

# 1. 기본 세계관

현재 프로젝트의 최소 출발점은 다음과 같다.

## 시대와 세계

- 중세 암흑시대풍 검과 마법 판타지.
- 수많은 사람이 절망 속에 살아간다.
- 수백 년간 전쟁이 지속되고 있다.
- 최초의 대규모 전투는 아크 드래곤과의 싸움이었다.
- 그 전투에서 양측 모두 치명적인 피해를 입었다.
- 이후 몬스터들의 지속적인 공격으로 국가와 사회가 장기간 쇠퇴하고 있다.

## 일곱 영웅과 용사

- 최초의 아크 드래곤 전투에서 활약한 일곱 인물을 `일곱 영웅`이라 부른다.
- 그들의 구심점이 최초의 `용사`였다.
- 현재의 용사는 최초 용사의 후예다.

## 플레이어

- 주인공의 출발점은 `용병`이다.

---

# 2. 현재 OPEN 설정

다음은 기본적으로 아직 확정되지 않은 설정이다.

- 나머지 여섯 영웅의 정체
- 일곱 영웅 각각의 행적
- 일곱 영웅의 운명
- 용사의 계승 방식

기존 파일에 특정 답이 적혀 있어도 자동으로 `CONFIRMED` 처리하지 마라.

필요에 따라 후보로 사용할 수 있으나, 최종 Director가 승인해야 확정된다.

---

# 3. 설정 상태 규칙

중요 설정은 반드시 다음 상태 중 하나로 관리한다.

## CONFIRMED

최종 스토리에서 사실로 확정된 설정.

## PROVISIONAL

현재 가장 유력하지만 추후 수정 가능한 설정.

## OPEN

아직 결정되지 않은 설정.

## REJECTED

검토했으나 최종 스토리에서 사용하지 않기로 한 설정.

---

# 4. 전체 실행 파이프라인

작업은 아래 순서로 실행한다.

```text
PHASE 0  FILE SCAN / INDEX
PHASE 1  PARALLEL CHAPTER COMPARISON
PHASE 2  STORY LEDGER BUILD
PHASE 3  CROSS-CHAPTER AUDIT
PHASE 4  DIRECTOR TRIAGE
PHASE 5  TARGETED DEEP REVIEW
PHASE 6  FINAL STORY DECISION
PHASE 7  FINAL CONSISTENCY PASS
PHASE 8  OUTPUT
```

각 Phase가 완료되기 전에 다음 Phase로 넘어가지 마라.

---

# 5. 작업 디렉터리 및 산출물

프로젝트 루트에 아래 작업 폴더를 만든다.

```text
_story_finalization/
```

그 안에 다음 구조를 사용한다.

```text
_story_finalization/
├─ 00_index/
│  └─ FILE_INDEX.md
│
├─ 01_chapter_analysis/
│  ├─ CH01_03_ANALYSIS.md
│  ├─ CH04_06_ANALYSIS.md
│  ├─ CH07_09_ANALYSIS.md
│  ├─ CH10_12_ANALYSIS.md
│  ├─ CH13_15_ANALYSIS.md
│  └─ CH16_18_ANALYSIS.md
│
├─ 02_ledger/
│  └─ STORY_LEDGER.md
│
├─ 03_audit/
│  ├─ PLOT_CAUSALITY_AUDIT.md
│  ├─ CHARACTER_EMOTION_AUDIT.md
│  └─ PLAYER_EXPERIENCE_AUDIT.md
│
├─ 04_triage/
│  └─ ISSUE_TRIAGE.md
│
├─ 05_deep_review/
│  └─ ISSUE_XX_*.md
│
└─ 06_final/
   ├─ FINAL_STORY_OVERVIEW.md
   ├─ FINAL_STORY_SPINE.md
   ├─ FINAL_CHARACTER_ARCS.md
   ├─ FINAL_FACTIONS.md
   ├─ FINAL_FORESHADOW_MAP.md
   ├─ FINAL_SETTINGS.md
   ├─ FINAL_CHANGELOG.md
   └─ FINAL_RISKS.md
```

기존 원본 파일은 직접 수정하지 마라.

최종 스토리 확정 전까지 모든 분석과 결과는 `_story_finalization/` 아래에만 작성한다.

---

# 6. PHASE 0 — FILE SCAN / INDEX

## 목표

두 폴더의 Chapter 1~18 관련 파일을 정확히 매핑한다.

## 수행

다음 폴더를 탐색한다.

```text
스토리 초안/
스토리 적용 방안/
```

하위 폴더가 존재하면 재귀적으로 탐색한다.

다음 패턴을 우선적으로 찾는다.

```text
CHAPTER_1
CHAPTER_01
CHAPTER 1
Chapter 1
챕터 1
```

파일명 형태가 달라도 실제 Chapter 번호가 확인되면 매핑한다.

---

## FILE_INDEX.md 형식

```markdown
# FILE INDEX

| Chapter | Draft Files | Application Files | Notes |
|---|---|---|---|
| 01 | ... | ... | |
| 02 | ... | ... | |
...
| 18 | ... | ... | |
```

추가로 기록한다.

### Missing
누락 파일.

### Duplicate
동일 Chapter에 여러 후보 파일이 존재하는 경우.

### Unclear
Chapter 번호 또는 용도를 판별하기 어려운 파일.

### Global Documents
특정 Chapter가 아닌 전체 설정 / 캐릭터 / 세계관 문서.

---

## Phase 0 규칙

내용 분석은 최소화한다.

파일을 열어 Chapter 번호를 판별해야 할 때만 첫 부분을 확인한다.

이 단계에서는 스토리 평가를 하지 않는다.

---

# 7. PHASE 1 — PARALLEL CHAPTER COMPARISON

Chapter 1~18을 6개 묶음으로 나누고 **6개의 독립 서브 에이전트**를 병렬 실행한다.

```text
Agent A → Chapter 01~03
Agent B → Chapter 04~06
Agent C → Chapter 07~09
Agent D → Chapter 10~12
Agent E → Chapter 13~15
Agent F → Chapter 16~18
```

---

# 8. Chapter Agent 컨텍스트 규칙

각 Chapter Agent에게 전달하는 정보는 최소화한다.

반드시 전달:

1. 담당 Chapter에 대응되는 `스토리 초안` 파일
2. 담당 Chapter에 대응되는 `스토리 적용 방안` 파일
3. 이 프롬프트의 `기본 세계관`
4. `OPEN 설정`
5. 아래 Chapter Analysis 출력 형식

가능하면 전달하지 않는다.

- 다른 범위의 원문
- 전체 프로젝트의 모든 캐릭터 문서
- 다른 에이전트 결과
- 불필요한 세계관 문서

다만 해당 Chapter를 이해하는 데 필수인 전역 설정 파일이 확인되면 필요한 부분만 추가한다.

---

# 9. Chapter Agent 임무

각 Agent는 담당 Chapter마다 Draft와 Application을 비교한다.

초안이 무조건 원본이 아니며 적용 방안이 무조건 개선본도 아니다.

다음 기준으로 판단한다.

---

## 9.1 PURPOSE

해당 Chapter가 전체 이야기에서 담당해야 하는 기능.

최대 2문장.

---

## 9.2 EVENTS

스토리에 영향을 주는 핵심 사건만 최대 5개.

사소한 이동, 대화, 설명은 생략한다.

---

## 9.3 PLAYER

플레이어가 실제로 수행하는 핵심 행동.

예:

- 전투
- 탐색
- 추적
- 조사
- 호위
- 탈출
- 잠입
- 선택
- 협상

NPC가 수행한 행동을 플레이어 행동처럼 기록하지 마라.

---

## 9.4 CHARACTER CHANGE

이 Chapter에서 실제로 변화하는 캐릭터.

다음 형태로 기록한다.

```text
캐릭터: 시작 상태 → 사건 → 변화 상태
```

실질적 변화가 없다면:

```text
NONE
```

---

## 9.5 NEW INFORMATION

플레이어가 이 Chapter에서 새롭게 알게 되는 중요 정보.

플레이어가 알지 못하는 작가용 정보는 별도로 구분한다.

---

## 9.6 SETUP / PAYOFF

새롭게 설치되는 복선과 기존 복선의 회수.

명확하지 않으면 후보로 표시한다.

---

## 9.7 DRAFT vs APPLICATION

두 버전의 차이 중 전체 스토리에 실제 영향을 주는 차이만 기록한다.

사소한 문장, 묘사, 연출 차이는 무시한다.

예:

```text
D1 → A1
초안에서는 왕자가 직접 배신하지만,
적용안에서는 대신 장군이 배신하도록 변경됨.
```

---

## 9.8 KEEP

두 버전 중 반드시 살릴 가치가 높은 요소.

최대 3개.

---

## 9.9 PROBLEMS

중요한 문제만 기록한다.

Severity:

```text
[C] Critical
[M] Major
[m] Minor
```

`Minor`는 전체 흐름과 관련될 때만 작성한다.

---

## 9.10 RECOMMENDATION

다음 중 하나를 선택한다.

```text
USE_DRAFT
USE_APPLICATION
HYBRID
REBUILD
```

이유는 최대 3문장.

---

# 10. Chapter Analysis 출력 형식

각 Chapter는 아래 형식을 따른다.

```markdown
## CHAPTER XX

**Purpose**
...

**Events**
1. ...
2. ...
3. ...

**Player**
...

**Character**
...

**Info**
...

**Setup / Payoff**
...

**Key Differences**
- ...

**Keep**
- ...

**Problems**
- [C] ...
- [M] ...

**Recommendation**
`HYBRID`

Reason:
...
```

---

# 11. Chapter Agent 토큰 규칙

각 Chapter 분석은 원칙적으로 **300~500단어 이내**.

단순한 Chapter는 더 짧게 작성한다.

금지:

- 장면 전체 재작성
- 긴 인용
- 대사 재생산
- 설정 백과사전
- Minor 이슈 나열
- 전체 프로젝트 재설계

이 단계의 목적은 **압축된 비교 데이터 생성**이다.

---

# 12. PHASE 2 — STORY LEDGER BUILD

6개 Chapter Agent 결과를 Lead Director가 읽는다.

원칙적으로 이 단계에서는 원문을 다시 읽지 않는다.

다음 파일을 만든다.

```text
_story_finalization/02_ledger/STORY_LEDGER.md
```

---

# 13. STORY LEDGER 구조

## 13.1 MAIN PLOT

Chapter마다 한 줄.

가능하면 다음 구조를 사용한다.

```text
C01: 원인 → 플레이어 행동 → 결과 → C02 발생 이유
C02: ...
```

C18까지 연결한다.

---

## 13.2 PLAYER GOAL

플레이어 목표의 변화만 추적한다.

예:

```text
C01~C03 생존 / 돈
→ C04 동료 구출
→ C06 사건의 원인 추적
→ C10 전쟁 개입
→ ...
```

모든 목표 변화에는 이유가 있어야 한다.

---

## 13.3 CHARACTER LEDGER

중요 인물만 ID를 부여한다.

예:

```text
CHAR-01
Name:
Start:
Desire:
Fear:
Relationship:
Major Choices:
Arc:
End:
```

---

## 13.4 FACTION LEDGER

```text
FAC-01
Name:
Goal:
Method:
Resources:
Player Relationship:
Major Actions:
Narrative Function:
```

---

## 13.5 FORESHADOW LEDGER

```text
F-01
Setup:
Reinforcement:
Payoff:
Status: VALID / WEAK / BROKEN / UNPAID
```

---

## 13.6 WORLD FACTS

실제 플롯과 캐릭터 행동에 영향을 주는 세계관 정보만 기록한다.

백과사전식 설정 작성 금지.

---

## 13.7 OPEN QUESTIONS

중요한 미해결 질문에 ID를 부여한다.

예:

```text
O-01
왜 적대 세력은 C08에서 주인공을 죽이지 않았는가?
```

---

## 13.8 CONTRADICTIONS

```text
X-01

Source A:
Source B:

Conflict:
Impact:
```

---

# 14. Ledger 압축 원칙

다음 단계에 전달할 때는 Chapter Analysis 전체가 아니라 가능하면 Story Ledger를 중심으로 전달한다.

반복 정보는 제거한다.

예:

BAD

```text
Chapter 4에서 왕국군이...
Chapter 6에서도 왕국군이...
Chapter 8에서도 왕국군이...
```

GOOD

```text
FAC-02 왕국군
목표:
행동:
변화:
```

---

# 15. PHASE 3 — CROSS-CHAPTER AUDIT

새로운 독립 서브 에이전트 3개를 병렬 실행한다.

```text
Agent G → Plot / Causality
Agent H → Character / Emotion
Agent I → Player Experience / Game Narrative
```

세 에이전트는 서로의 결과를 보지 않는다.

---

# 16. Audit Agent 입력

원칙적으로 다음만 전달한다.

```text
6개 Chapter Analysis
STORY_LEDGER.md
기본 세계관
```

전체 원문을 전달하지 않는다.

---

# 17. Agent G — Plot / Causality Auditor

다음만 집중해서 검사한다.

## 사건 인과

각 Chapter에 대해:

```text
앞 사건 때문에 다음 사건이 발생하는가?
```

아니면:

```text
작가가 다음 장면을 원해서 사건이 갑자기 발생하는가?
```

---

## 주인공 목표

플레이어가 다음 Chapter로 이동하는 이유가 충분한가?

---

## 적대 세력

적대자가 주인공을 이기게 해 주기 위해 비합리적으로 행동하지 않는가?

---

## 우연

중요 사건의 발생 또는 해결이 우연에 의존하는가?

---

## Stakes

승리와 패배에 실제 대가가 있는가?

---

## Escalation

이야기가 진행되며 갈등이 발전하는가?

아니면 유사 사건이 반복되는가?

---

## 출력

`Critical / Major` 문제만 최대 10개.

형식:

```markdown
### ISSUE P-01

Severity:
Critical

Chapters:
C04, C09

Problem:
...

Why:
...

Question:
...
```

---

# 18. Agent H — Character / Emotion Auditor

다음만 검사한다.

## 주인공 아크

C01과 C18의 주인공은 어떻게 다른가?

변화가 실제 사건과 선택으로 만들어졌는가?

---

## 캐릭터 동기

중요 행동마다 충분한 이유가 존재하는가?

---

## 관계 변화

관계가 실제 사건으로 변하는가?

대사 한두 줄로 친해지거나 적대적으로 변하지 않는가?

---

## 감정 준비

다음 사건이 충분히 준비되어 있는가?

- 죽음
- 희생
- 배신
- 이별
- 화해
- 반전
- 타락
- 각성

---

## NPC 기능 중복

동일 기능의 캐릭터가 지나치게 많지 않은가?

합치거나 삭제해도 되는가?

---

## 출력

`Critical / Major` 문제만 최대 10개.

---

# 19. Agent I — Player Experience / Game Narrative Auditor

다음만 검사한다.

## 플레이어 참여

중요 사건을 플레이어가 직접 경험하는가?

아니면 NPC들의 이야기를 구경하는가?

---

## 정보 전달

플레이어에게 필요한 정보가 실제 플레이 안에서 전달되는가?

---

## Gameplay / Story Integration

전투, 탐색, 조사, 선택, 실패가 이야기와 연결되는가?

---

## 컷신 의존

스토리 핵심이 지나치게 컷신에 의존하지 않는가?

---

## Agency

플레이어가 사건의 원인이 되는가?

아니면 계속 뒤따라가기만 하는가?

---

## 반복 구조

다음과 같은 패턴이 반복되지 않는가?

```text
마을
→ NPC 설명
→ 던전
→ 보스
→ 컷신
```

---

## 출력

`Critical / Major` 문제만 최대 10개.

---

# 20. PHASE 4 — DIRECTOR TRIAGE

세 Audit 결과를 Lead Director가 통합한다.

다음 파일을 만든다.

```text
_story_finalization/04_triage/ISSUE_TRIAGE.md
```

---

# 21. 문제 통합

같은 본질의 문제는 하나로 합친다.

예:

```text
Plot Auditor:
주인공이 C07에서 북부로 이동할 이유가 약함

Character Auditor:
C07 선택이 기존 동기와 맞지 않음
```

둘을 별도 이슈로 만들지 말고:

```text
ISSUE-03
C07 북부 이동의 동기 부족
```

으로 통합한다.

---

# 22. 우선순위

각 문제는 다음 중 하나로 분류한다.

## MUST FIX

그대로 두면 다음 중 하나가 무너지는 문제.

- 전체 인과관계
- 주요 캐릭터 아크
- 핵심 반전
- 메인 갈등
- 플레이어 동기
- 설정 정합성

## SHOULD FIX

수정하면 이야기의 품질이 명확하게 개선된다.

## OPTIONAL

취향 차이 또는 비용 대비 효과가 낮다.

OPTIONAL은 이후 원칙적으로 무시한다.

---

# 23. ISSUE_TRIAGE.md 형식

```markdown
# MUST FIX

## ISSUE-01
Chapters:
Problem:
Why it matters:
Need source reread:
YES / NO

---

# SHOULD FIX

...

# OPTIONAL

...
```

---

# 24. PHASE 5 — TARGETED DEEP REVIEW

토큰 절약의 핵심 단계다.

전체 원문을 다시 읽지 않는다.

`MUST FIX`와 매우 중요한 `SHOULD FIX`만 원문 재검토한다.

---

# 25. 원문 재독 범위 선택

예:

```text
ISSUE:
C05에서 설정된 왕의 동기가
C12 배신과 연결되지 않는다.
```

우선 다음만 읽는다.

```text
C05 Draft
C05 Application
C12 Draft
C12 Application
```

필요할 때만 관련 중간 Chapter를 추가한다.

---

# 26. Deep Review Agent 생성 기준

문제 하나 또는 서로 강하게 연결된 문제군마다 별도의 서브 에이전트를 사용한다.

예:

```text
Agent J → 왕의 배신 동기
Agent K → 용사 정체 반전
Agent L → 플레이어와 여주인공 관계
```

모든 이슈를 하나의 거대한 Agent에 넣지 마라.

---

# 27. Deep Review Agent 역할

각 문제에 대해 다음을 분석한다.

## ISSUE

무엇이 문제인가?

## ROOT CAUSE

표면적 증상이 아니라 실제 원인을 찾는다.

## OPTION A — Draft

초안의 해결법을 채택할 경우.

장점 / 단점.

## OPTION B — Application

적용안의 해결법을 채택할 경우.

장점 / 단점.

## OPTION C — Hybrid / New

두 버전의 장점을 합치거나 새로운 해결법을 만든다.

## CONSEQUENCES

수정 시 영향을 받는 Chapter.

## RETCON COST

```text
LOW
MEDIUM
HIGH
```

## RECOMMENDATION

최대 5문장.

---

# 28. Retcon Cost

## LOW

1~2개 Chapter 수정.

## MEDIUM

3~5개 Chapter 수정.

## HIGH

6개 이상 Chapter 수정 또는 다음 요소 변경:

- 주요 캐릭터의 정체
- 메인 악역 구조
- 전쟁의 본질
- 용사의 역할
- 세계관 핵심 규칙

HIGH Retcon도 금지하지 않는다.

그러나 스토리 개선 이득이 명확해야 한다.

---

# 29. PHASE 6 — FINAL STORY DECISION

Lead Director가 모든 분석을 검토한다.

각 주요 쟁점마다 다음 중 하나를 결정한다.

```text
KEEP_DRAFT
KEEP_APPLICATION
HYBRID
REWRITE
REMOVE
```

Director는 Writer나 Critic 어느 쪽의 편도 들지 않는다.

두 버전이 모두 약하면 제3의 안을 선택한다.

---

# 30. Director 판단 규칙

다음 우선순위를 유지한다.

1. 재미와 감정적 효과
2. 캐릭터
3. 인과관계
4. 플레이어 경험
5. 복선과 반전
6. 세계관
7. 현실성

현실성을 높이기 위해 재미를 죽이지 마라.

그러나 다음은 반드시 해결한다.

- 캐릭터 붕괴
- 설정 붕괴
- 인과관계 붕괴
- 플레이어 동기 붕괴

---

# 31. FINAL STORY SPINE

최종 확정 후 Chapter 1~18을 다음 구조로 작성한다.

```markdown
## CHAPTER XX — 제목 또는 기능

**Goal**
플레이어가 원하는 것.

**Conflict**
그 목표를 막는 것.

**Major Event**
Chapter의 중심 사건.

**Player Action**
플레이어가 직접 수행하는 핵심 행동.

**Reveal**
플레이어가 새롭게 알게 되는 정보.

**Character Change**
누가 어떻게 변화하는가.

**Setup / Payoff**
설치 또는 회수되는 복선.

**Outcome**
이 Chapter 이후 무엇이 달라지는가.

**Next**
왜 다음 Chapter가 발생하는가.
```

---

# 32. PHASE 7 — FINAL CONSISTENCY PASS

Story Spine 작성 후 한 번만 전체 연결을 검사한다.

---

## 32.1 Causal Chain

다음을 검사한다.

```text
C01 → C02 → C03 → ... → C18
```

각 화살표마다 질문한다.

> 앞 Chapter의 결과가 다음 Chapter의 원인인가?

아닌 경우 수정한다.

---

## 32.2 Plot Flow

갈등이 어떻게 확대되는지 확인한다.

---

## 32.3 Character Flow

주인공이 어떤 사건을 통해 어떻게 변화하는지 확인한다.

---

## 32.4 Emotion Flow

플레이어가 느끼는 주요 감정의 순서를 확인한다.

예:

```text
불안
→ 안도
→ 의심
→ 상실
→ 분노
→ 희망
```

비슷한 감정이 장기간 반복되지 않는지 본다.

---

## 32.5 Information Flow

플레이어가 세계의 진실을 어떤 순서로 알게 되는지 확인한다.

후반 반전을 위해 단순히 정보를 숨기고 있지 않은지 검토한다.

---

# 33. 반전 규칙

좋은 반전은 다음 구조를 우선한다.

```text
이전에 본 정보
+
플레이어의 자연스러운 오해
→
새로운 의미
```

피한다.

```text
정보를 전혀 제공하지 않음
→
갑자기 진실 공개
```

중요 반전마다 가능하면 앞 Chapter에 최소 하나 이상의 단서를 둔다.

---

# 34. 장면 필요성 규칙

중요 장면마다 내부적으로 다음 질문을 한다.

> 이 장면을 통째로 삭제해도 이후 이야기가 거의 동일하게 진행되는가?

YES라면 다음 중 하나를 수행한다.

- 삭제
- 다른 장면과 통합
- 캐릭터 변화 추가
- 정보 획득 추가
- 이후 사건의 원인으로 연결

순수 분위기 장면은 허용할 수 있지만 반복되면 줄인다.

---

# 35. 캐릭터 규칙

중요 캐릭터 행동은 가능하면 다음 구조를 따른다.

```text
욕망
→ 선택
→ 결과
→ 변화
```

플롯이 필요하다는 이유만으로 캐릭터가 행동하게 하지 마라.

---

# 36. 게임 스토리 규칙

항상 다음을 구분한다.

```text
NPC가 경험한 사건
vs
플레이어가 경험한 사건
```

중요 감정적 사건일수록 플레이어의 직접 관여도를 높인다.

---

# 37. 정보 전달 우선순위

중요 세계관 및 스토리 정보는 가능하면 다음 순서로 전달한다.

1. 플레이
2. 환경
3. 사건
4. 대화
5. 문서
6. 설명 컷신

---

# 38. PHASE 8 — FINAL OUTPUT

최종적으로 아래 파일을 생성한다.

---

## FINAL_STORY_OVERVIEW.md

전체 이야기 10~20문장.

다음이 한눈에 보여야 한다.

- 주인공의 출발점
- 핵심 갈등
- 이야기의 확대
- 주요 반전
- 최종 갈등
- 주인공의 변화
- 결말의 의미

---

## FINAL_STORY_SPINE.md

Chapter 1~18 전체 확정 구조.

---

## FINAL_CHARACTER_ARCS.md

중요 캐릭터만 기록한다.

형식:

```text
이름
Start
Desire
Fear
Major Choices
Relationship Changes
Turning Point
End
```

---

## FINAL_FACTIONS.md

스토리에 실제 영향을 주는 세력만.

---

## FINAL_FORESHADOW_MAP.md

표 형식.

```markdown
| ID | Setup | Reinforcement | Payoff | Status |
|---|---|---|---|---|
```

---

## FINAL_SETTINGS.md

다음 섹션을 포함한다.

```text
CONFIRMED
PROVISIONAL
OPEN
REJECTED
```

---

## FINAL_CHANGELOG.md

기존 두 폴더 대비 중요한 변경만 기록한다.

형식:

```markdown
## CHANGE-01

Related Chapters:

Before:
...

Final:
...

Reason:
...

Retcon Cost:
LOW / MEDIUM / HIGH
```

---

## FINAL_RISKS.md

최종안에도 남아 있는 위험 요소.

최대 5개.

없다면:

```text
NONE
```

---

# 39. 서브 에이전트 운용 요약

필수 병렬 에이전트:

```text
A → C01~C03
B → C04~C06
C → C07~C09
D → C10~C12
E → C13~C15
F → C16~C18
```

이후:

```text
G → Plot / Causality
H → Character / Emotion
I → Player Experience
```

추가 Agent는 `MUST FIX` 문제에 대해서만 생성한다.

---

# 40. Context Budget 규칙

## Chapter Agent

전달:

```text
담당 Chapter 원문
기본 세계관
OPEN 설정
출력 스키마
```

전달 금지:

```text
18 Chapter 전체
다른 Agent의 전체 분석
불필요한 설정집
```

---

## Cross-Chapter Auditor

전달:

```text
Chapter Analysis
Story Ledger
```

원칙적으로 원문 전달 금지.

---

## Deep Review Agent

전달:

```text
문제가 발생한 Chapter
직접 연결된 Chapter
해당 ISSUE
관련 Ledger 정보
```

18 Chapter 전체 전달 금지.

---

# 41. 중간 결과 압축

에이전트 결과를 다음 에이전트에 그대로 복사하지 않는다.

Lead Director가 먼저 압축한다.

제거:

- 반복 설명
- 긴 인용
- 감상
- 칭찬
- 이미 해결된 이슈
- 중요하지 않은 Minor
- 같은 설정의 중복 설명

---

# 42. 실패 및 예외 처리

## 파일이 없을 경우

해당 Chapter를 추측하지 마라.

`MISSING SOURCE`로 표시한다.

다른 버전만 존재하면 그것만 분석하되 명시한다.

---

## 한 Chapter에 여러 파일이 있을 경우

무조건 하나를 임의 선택하지 않는다.

내용을 비교하여 관계를 판별한다.

예:

```text
main draft
alternate
old version
notes
```

---

## Chapter 번호가 불분명할 경우

내용상 확실하지 않으면 `UNRESOLVED FILE`로 남긴다.

억지로 Chapter에 배정하지 않는다.

---

## Agent 결과가 서로 충돌할 경우

Director가 근거를 비교한다.

필요할 때만 관련 원문을 다시 읽는다.

새로운 전체 분석 Agent를 만들지 않는다.

---

# 43. 조기 종료 금지

몇 개 Chapter만 보고 전체 결론을 내리지 마라.

Chapter 1~18의 Story Ledger가 완성된 후에만 전체 구조를 평가한다.

---

# 44. 과잉 분석 방지

다음과 같은 문제는 전체 스토리에 영향이 없으면 무시한다.

- 작은 현실성 문제
- 사소한 이동 거리
- 일부 대사의 자연스러움
- 배경 NPC의 세부 동기
- 설정집 수준의 미정 정보
- 장면별 문장 표현

현재 단계의 목표는 **스토리 구조 확정**이다.

---

# 45. 최종 판단 질문

최종 Story Spine을 승인하기 전에 Lead Director는 아래 질문에 모두 답한다.

1. C01의 사건이 C18까지 실제 인과관계를 만든다.
2. 플레이어가 각 Chapter에 참여할 이유가 있다.
3. 주인공은 사건 때문에 변화한다.
4. 주요 NPC의 행동에는 일관된 동기가 있다.
5. 주요 적대자는 주인공을 위해 멍청하게 행동하지 않는다.
6. 핵심 반전에는 사전 단서가 존재한다.
7. 중요한 복선은 회수되거나 의도적으로 남겨진다.
8. 플레이어에게 필요한 정보는 게임 안에서 전달된다.
9. 중요한 감정적 사건에 플레이어가 직접 관여한다.
10. 불필요하게 중복되는 Chapter가 없다.
11. 동일한 기능의 NPC가 과도하게 중복되지 않는다.
12. 각 Chapter 종료 시 세계 또는 관계 또는 목표가 변화한다.
13. 다음 Chapter로 넘어갈 원인이 존재한다.
14. C18의 결말이 C01에서 시작된 이야기와 연결된다.

하나라도 명백하게 실패하면 해당 부분을 수정한 뒤 최종 승인한다.

---

# 46. 최종 원칙

기존 스토리를 보존하는 것이 목적이 아니다.

`스토리 초안`과 `스토리 적용 방안` 중 승자를 고르는 것도 목적이 아니다.

Critic을 만족시키는 것도 목적이 아니다.

새로운 아이디어를 많이 만드는 것도 목적이 아니다.

목적은 단 하나다.

> **Chapter 1에서 시작된 사건, 캐릭터, 감정, 플레이 경험이 Chapter 18까지 가장 강력하고 일관되게 이어지는 하나의 게임 스토리를 확정한다.**

좋은 요소는 어느 파일에서 왔는지와 관계없이 살린다.

나쁜 요소는 오래된 핵심 설정이라도 필요하면 수정한다.

그러나 변경에는 항상 비용이 있다.

따라서 최종 기준은:

> **최소 변경이 아니라, 최소 비용으로 최대 스토리 개선을 달성하라.**
