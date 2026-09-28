# 일반 고블린 생성 기록

방식: built-in image_gen. 이동을 먼저 생성하고 공격의 고정 시각 기준으로 사용.

## 이동

Create a pixel art game sprite sheet: NORMAL GOBLIN WALK CYCLE, 4 frames in one horizontal row, 2172x724 canvas, equal 543x724 cells. Genuine transparent RGBA background with alpha 0 outside the sprite; no coloured background, haze, glow, shadow, checkerboard, labels, grids, or other objects. Create a new unarmoured normal goblin holding one crude brown wooden club in its right hand; its left hand remains visible, with exactly two arms and two legs. Use a bald olive-green head, large pointed ears, tiny tusks, bare feet, and nonsexual featureless anatomy.

The goblin must have a compact, chunky mini-player-like pixel silhouette: clearly SHORT upper/lower arms and SHORT legs relative to its wide head and torso, sturdy limbs, crisp large square pixels, dark outline, muted olive-green and brown palette. Do not use a tall, wiry, long-limbed body. Face three-quarter right in all frames. Make a four-frame walk cycle: left-foot contact, passing, right-foot contact, opposite passing. Keep the same head/body ratio, shoulder width, limb thickness and lengths, palette, club hand, and pixel block size across all cells. Keep opaque core pixels away from every cell edge. Redraw the art; do not copy pixels from player sprites. This walk sheet is the visual master for the goblin attack sheet.

## 공격

Create a pixel art game sprite sheet: NORMAL GOBLIN CLUB OVERHEAD ATTACK, 4 frames in one horizontal row, 2172x724 canvas, equal 543x724 cells. Use a genuine transparent RGBA background with alpha 0 outside the sprite; no coloured background, haze, glow, shadow, checkerboard, labels, grids, or other objects.

Use only the provided current normal-goblin walk sheet as the visual reference for body proportions, limb lengths and thickness, pixel block size, palette, and club grip. Redraw every attack pixel from scratch; do not copy or reuse source pixels. The character must retain the compact chunky mini-player-like silhouette: wide head/torso, clearly SHORT arms and legs, two complete connected arms, two legs, and one club held in the right hand. The free left hand may overlap the torso during a pose but must remain structurally readable. Do not create a tall, wiry, long-limbed body, extra or missing limbs, detached arms, or a different palette.

Face three-quarter right in all frames. Animate: wind-up with club behind the body; lift the club overhead; drive it down toward the front ground; recover with club lowered. Keep opaque core pixels within their 543px cell. No armour, clothing, or sexual anatomy.
