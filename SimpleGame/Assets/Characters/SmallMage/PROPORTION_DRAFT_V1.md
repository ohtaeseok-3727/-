# 비율 보정 시안 — 미채택

attacks-proportion-draft-v1.png는 이동 시트를 비율 기준, attacks-selected-v1.png를 동작 기준으로 내장 image_gen에서 편집한 시안이다.
1536×1024 RGBA 및 모서리 알파 0을 확인했다. 원본 동작의 완전한 동일성은 검증되지 않았으므로 preview.html은 교체하지 않았다. 빌드·실행하지 않았다.

## 프롬프트

Precise proportion edit. IMAGE2 is immutable motion reference: preserve exactly all18 poses, limb directions, elbow knee angles, torso lean, palm orientation, cape motion and frame order. Do NOT invent new attacks or straighten crouches or change hand gestures. IMAGE1 is anatomy reference ONLY: match head-to-body ratio (head approx half standing height), broad head, compact torso, short limbs and same coarse pixel style. Adjust segment sizes of image2 to image1 proportions while retaining every original joint angle and pose silhouette direction. Uniform anatomy across frames. No extra limbs, no pose repair, no added hands beyond original visible ones. Six columns three rows1536x1024. Feet remain at y334,647,961. Increase head relative to torso, shorten torso and legs proportionally; keep original animation exactly recognizable. Actual transparent RGBA PNG alpha0 outside subjects, no checkerboard no glow. No VFX. Only proportion correction.
