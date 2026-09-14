#pragma once
#include <string>
#include <vector>

struct SpriteImage
{
    int width = 32;
    int height = 52;
    std::vector<unsigned char> rgba;
};

// Adapts the accepted coarse previews in memory; source PNGs remain unchanged.
bool LoadMercenarySprite(const std::wstring& filename, SpriteImage& image);
// 4 directions x 4 frames; output atlas uses uniform 32x52 cells.
bool LoadMercenaryWalkSheet(const std::wstring& filename, SpriteImage& atlas);
// Decode prepared atlases without cropping, recolouring or resampling.
bool LoadRawSpriteSheet(const std::wstring& filename, SpriteImage& image);
