# C02 SPRITE PROMPTS

## ENEMY-ORC-01 — Common Orc
Use the actual SmallKnight sheets as the strict pixel-production reference: same chunky pixel clusters, compact proportions, face simplicity, dark outline grouping, limited stepped shading, game-camera presentation and foot anchor. Create a compact common orc soldier with muted green skin, short tusks, simple dark leather/iron equipment and a broad readable melee weapon. Transparent RGBA only. No painterly gradients, no tiny texture noise, no giant anatomy.

Required: 8-frame walk compatible with the existing walk-sheet philosophy; simple melee attack set; hit/death only if needed by current enemy runtime.

## ENEMY-ORC-02 / 03 / 04 — Role Variants
Derive from the same Common Orc body. Signaler adds only a flag/horn/beacon tool silhouette. Siege Engineer adds only a tool/explosive satchel silhouette. Elite adds heavier shoulder/torso armor and a larger weapon, but keeps the same species scale and pixel density.

## BOSS-002 — Barkan
Use Common Orc as species master and SmallKnight as rendering master. He is an orc general, not a monster-beast. Slightly larger/heavier than Common Orc, dark iron armor, one asymmetrical heavy shoulder plate, deep red command cloth, brass/iron command seal, heavy cleaver or poleblade. Required: idle, walk, heavy attack, command signal, hit, armor/stagger break, retreat attempt, death. Transparent RGBA.

## Reuse Rules
Do not regenerate Hero or Generic NPC for Chapter 2. Reuse existing assets after style/alpha QA.
# 2026-09-24 사용자 확정 공격 동작 보충

바르칸: 기본 무기 휘두르기, 플레이어가 멀어지면 점프하여 플레이어 위치에 내려찍기, 주변 적을 밀쳐내고 공격속도가 증가하는 전투의 함성. 밀침/공속 효과의 구체적 대상 및 수치는 미정이다. 해당 공격 목록이 아래의 일반적인 Heavy Attack / Command Signal 표현보다 우선한다. 생성 시안과 정확한 프롬프트는 `output/chapter2-orc-combat-sprites/`에 있으며 런타임 승인본이 아니다.
