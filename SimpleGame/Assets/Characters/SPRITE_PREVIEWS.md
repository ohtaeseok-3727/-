# 저해상도 스타일 시안 v2

내장 image_gen 도구로 생성한 확대 시안입니다. 원본은 투명 배경의 저해상도 파일이 아닙니다. 현재 게임에서는 이 두 v2 원본을 읽어 캐릭터 부분을 32×52 메모리 텍스처로 추출하고 배경을 투명 처리하여 표시합니다. 젊은 외형으로 수정한 후속 시안과 v1 이미지는 사용하지 않습니다.

## 게임 연결

- `SpriteImage.cpp`: 두 원본의 공통 영역 `(288, 400, 432, 704)`을 샘플링.
- 회색 배경과 연결된 픽셀을 메모리에서 투명 처리. 원본 PNG는 보존.
- `Renderer::Character`: 2배 정수 확대, 최근접 필터링, 발 위치 기준 배치.
- MSBuild의 `CopyCharacterSprites`가 사용자의 빌드 시 두 v2 PNG를 복사.
- WIC 디코더는 1024×1536 원본 크기를 검사. 로딩 실패 시 콘솔 오류 후 종료.
- 빌드 및 게임 실행 결과는 에이전트가 확인하지 않음.

- knight-coarse-preview-v2.png: 기사, 검·방패와 회색 갑옷.
- mage-coarse-preview-v2.png: 마법사, 무기 없이 양손 장갑의 청록색 마법진.

## 기사 생성 프롬프트

Create ONE extremely LOW RESOLUTION pixel art KNIGHT idle sprite. Strict aesthetic target: character designed on a 32 pixels wide by 48 pixels tall logical grid, enlarged 16x with nearest-neighbor so individual pixels are BIG visibly square blocks. Character only about 36 pixels tall, 3 heads tall, face only 4x5 pixels, eyes single pixels, hands 2x3 pixels. Maximum 16 flat colors total, 2 shades per material. No tiny details, no texture, no gradients, no antialiasing, NO high resolution illustration disguised with pixel texture. This should look like a small 1990s 16-bit RPG overworld sprite, not concept art. Single centered full-body adult mercenary knight, idle front three-quarter facing slightly right. Short brown hair, muted olive cape, simple grey breastplate and shoulder armor, brown belt and boots. Small simple straight sword held downward, compact shield. Strong readable compact silhouette. Plain uniform dark slate background. No text, no labels, no grid, no scenery, no shadow, no extra views or enlarged inset. Entire image must use coarse regular pixel blocks. Same design vocabulary for a matching mage later.

## 마법사 생성 프롬프트

Use the latest coarse low-resolution knight as exact STYLE, PIXEL SIZE, body proportion, canvas framing reference. Create its matching MAGE sprite, ONE figure only. Same giant clearly visible square pixel clusters, approximately 32x48 logical sprite grid enlarged, face only 4x5 pixels, about 36 pixels body height, maximum 16 flat colors, no subtle textures, no gradients or smooth illustration. Same short brown hair, adult mercenary face, olive short cape, brown belt, brown boots and front three-quarter idle view facing right. Replace all metal armor with simple dark navy robe split above boots and cloth sleeves. Remove sword and shield completely. ABSOLUTELY NO WEAPON, staff, wand, book, orb or held item. BOTH hands empty with dark brown gloves, held slightly away from body so backs visible. Back of EACH glove has a tiny cyan MAGIC CIRCLE drawn on the leather: a coarse 3x3 or 5x5 pixel cyan ring with dark central pixel, not a floating effect. Make these two cyan glove markings readable even with very few pixels. Preserve same uniform dark slate backdrop, same figure scale and placement as knight. No text, no inset, no floor, no extras. Output a deliberately chunky low-pixel-count RPG sprite enlarged for review.
