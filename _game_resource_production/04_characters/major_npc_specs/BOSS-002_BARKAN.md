# BOSS-002 — Barkan

## Classification
MAJOR NPC / BOSS

## Story Role
Orc general who uses Graywall Fortress as a lure while targeting King's Canyon bridges, supplies, refugees and military movement.

## Gameplay Role
First true cooperative boss with Hero AI. Barkan controls the field with heavy armor, command signals, reinforcements and demolition pressure.

## Sprite Masters
- SmallKnight: body/pixel-density/shading/outline master.
- New Common Orc C02 sheet: species/body-family master.

## Silhouette
Compact but significantly heavier than Common Orc. Broad armored torso, thick boots, one oversized asymmetric shoulder plate, readable weapon profile.

## Head
Green orc skin, short tusks, minimal face detail, no realistic anatomy.

## Outfit
Dark iron segmented armor with restrained red command cloth. Limited highlights and large pixel clusters.

## Equipment
Heavy cleaver/poleblade; command seal or signal emblem integrated at belt/chest; reinforced gauntlet.

## Signature Feature
Red iron command mark plus single heavy shoulder plate.

## Color Role
Primary: dark iron gray.
Secondary: muted orc green.
Accent: deep command red / small brass seal.

## Required Animations
Latest user-confirmed attacks (2026-09-24): basic weapon swing; when the player is far away, jump and slam at the player's position; battle cry that repels surrounding enemies and increases attack speed. Exact affected factions, buff recipients, distances, duration, values and target snapshot timing are not confirmed. These attacks supersede the earlier generic Heavy Attack / Command Signal labels as the combat production requirement; story-specific animations remain separate.

Idle, Walk, Heavy Attack, Command Signal, Hit, Armor/Stance Break, Retreat Attempt, Death.
Reuse SmallKnight timing philosophy where motion roles match.

## Asset Destination
`SimpleGame/Assets/Monsters/Orc/Barkan/`

## Do Not
Do not turn Barkan into a giant beast, horned demon, wolf, or highly rendered concept-art character. Do not exceed SmallKnight detail density.
