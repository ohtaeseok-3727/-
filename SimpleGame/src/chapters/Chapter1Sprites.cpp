#include "stdafx.h"
#include "Chapter1Sprites.h"
#include "PixelRestyleHeroData.h"
#include "SpriteImage.h"
#include <algorithm>

Chapter1Sprites::~Chapter1Sprites()
{
    for (auto& atlas : m_Atlases)
    {
        if (atlas.texture)
        {
            glDeleteTextures(1, &atlas.texture);
        }
    }
}

bool Chapter1Sprites::Load(ChapterSprite sprite, Atlas& atlas)
{
    atlas.attempted = true;
    const wchar_t* paths[] = {L"..\\Monsters\\Goblin\\Common\\walk-v2.png",
                              L"..\\Monsters\\Goblin\\Common\\attack-v2.png",
                              L"NPC\\Villager\\villager-v1.png",
                              L"..\\Monsters\\Goblin\\Healer\\healer-v1.png",
                              L"..\\Monsters\\Goblin\\LegionCommander\\legion-commander-v1.png",
                              PixelRestyleHero::path};
    SpriteImage image;
    if (!LoadRawSpriteSheet(paths[static_cast<int>(sprite)], image))
    {
        return false;
    }
    atlas.width = image.width;
    atlas.height = image.height;
    const int rows =
        sprite == ChapterSprite::GoblinWalk || sprite == ChapterSprite::GoblinAttack ? 1 : 4;
    if (sprite == ChapterSprite::Hero)
    {
        if (image.width != PixelRestyleHero::width || image.height != PixelRestyleHero::height)
        {
            return false;
        }
        for (const auto& region : PixelRestyleHero::regions)
        {
            atlas.frames.push_back({region[0], region[1], region[2], region[3]});
        }
    }
    else
    {
        if ((rows == 1 && (image.width != 2172 || image.height != 724)) ||
            (rows == 4 && (image.width != 1254 || image.height != 1254)))
        {
            return false;
        }
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                atlas.frames.push_back({col * image.width / 4,
                                        row * image.height / rows,
                                        (col + 1) * image.width / 4,
                                        (row + 1) * image.height / rows});
            }
        }
    }
    // Read alpha bounds only; never rewrite, recolor, or resize source assets.
    for (auto& frame : atlas.frames)
    {
        Frame bounds{frame.right, frame.bottom, frame.left, frame.top};
        for (int y = frame.top; y < frame.bottom; ++y)
        {
            for (int x = frame.left; x < frame.right; ++x)
            {
                if (image.rgba[(static_cast<std::size_t>(y) * image.width + x) * 4 + 3] > 128)
                {
                    bounds.left = (std::min)(bounds.left, x);
                    bounds.top = (std::min)(bounds.top, y);
                    bounds.right = (std::max)(bounds.right, x + 1);
                    bounds.bottom = (std::max)(bounds.bottom, y + 1);
                }
            }
        }
        if (bounds.right <= bounds.left || bounds.bottom <= bounds.top)
        {
            return false;
        }
        frame = bounds;
    }
    glGenTextures(1, &atlas.texture);
    glBindTexture(GL_TEXTURE_2D, atlas.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 image.width,
                 image.height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image.rgba.data());
    GLint width = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (width != image.width)
    {
        glDeleteTextures(1, &atlas.texture);
        atlas.texture = 0;
        return false;
    }
    return true;
}

bool Chapter1Sprites::Draw(
    Renderer& renderer, ChapterSprite sprite, int frame, Point foot, float height, bool flip)
{
    auto& atlas = m_Atlases[static_cast<std::size_t>(sprite)];
    if (!atlas.attempted)
    {
        renderer.Flush();
        Load(sprite, atlas);
    }
    if (!atlas.texture)
    {
        return false;
    }
    const auto& source =
        atlas.frames[static_cast<std::size_t>((std::max)(0, frame)) % atlas.frames.size()];
    const float width = height * (source.right - source.left) / (source.bottom - source.top);
    renderer.Sprite(atlas.texture,
                    foot.x - width * .5f,
                    foot.y - height,
                    width,
                    height,
                    float(flip ? source.right : source.left) / atlas.width,
                    float(source.top) / atlas.height,
                    float(flip ? source.left : source.right) / atlas.width,
                    float(source.bottom) / atlas.height);
    return true;
}
