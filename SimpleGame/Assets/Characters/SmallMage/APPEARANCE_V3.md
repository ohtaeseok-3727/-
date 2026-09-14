# 공격 외형 v3

attacks-transparent-v3.png는 walk-transparent-v2.png를 외형 기준으로 내장 image_gen에서 편집했다. 1536×1024 RGBA, 모서리 알파 0 확인. 기존 v2는 보존했다.
preview.html의 공격 시트, 분리 경계, 발 기준점과 표시 배율을 갱신했다. 공격 표시 높이는 이동과 비슷하도록 약 145px로 맞췄다. 효과 속도 설정은 유지했다.
생성 편집이므로 원본 동작의 픽셀 단위 동일성은 보장하지 않는다. 빌드 및 실행 검증은 하지 않았다.

## 프롬프트

Edit image2 attack sprite sheet to match EXACT character model of image1 walk sheet. Image1 is absolute reference for coarse big square pixel clusters, broad brown hair silhouette, large head half body height, pale face rectangular black eyes, dark muted navy tunic, olive scarf cape, brown gloves with cyan squares, short brown boots. Current attack looks orange-haired smooth outlined different character; replace appearance with walk character. Preserve all18 attack poses and sequence and foot positions from image2, two arms each. In mana sweep row2 columns2/3 keep left attacking forearm across chest in FRONT and right resting arm bent at waist BEHIND, no duplicate arms. No new VFX. 1536x1024 six columns three rows, keep feet baselines334,647,961 and x centers128+256n. Clear transparent gutters, each sprite within own cell. Actual transparent PNG RGBA alpha0 background, no checkerboard no glow. Crucial all18 characters must look like the SAME exact walk sprite character in different poses, same palette and pixel coarseness.
