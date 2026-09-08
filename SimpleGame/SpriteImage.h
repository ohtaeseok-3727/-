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
