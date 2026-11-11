#include "stdafx.h"
#include "Chapter3Sprites.h"
#include "Chapter3SpriteData.h"
#include "SpriteImage.h"
#include <algorithm>
#include <cmath>

Chapter3Sprites::~Chapter3Sprites()
{
    for (auto& atlas : m_Atlases)
    {
        if (atlas.texture)
        {
            glDeleteTextures(1, &atlas.texture);
        }
    }
}

int Chapter3Sprites::Frame(Chapter3Motion motion, float seconds)
{
    seconds = (std::max)(0.f, seconds);
    if (motion == Chapter3Motion::Idle)
    {
        return static_cast<int>(std::fmod(seconds * 4.f, 4.f));
    }
    if (motion == Chapter3Motion::Walk)
    {
        return 4 + static_cast<int>(std::fmod(seconds * 10.f, 8.f));
    }
    return (motion == Chapter3Motion::Ranged ? 16 : 12) +
           (std::min)(3, static_cast<int>(seconds * 4.f));
}

bool Chapter3Sprites::Load(Chapter3Person person, Atlas& atlas)
{
    atlas.attempted = true;
    const auto& data = Chapter3SpriteData::atlases[static_cast<std::size_t>(person)];
    SpriteImage image;
    if (!LoadRawSpriteSheet(data.path, image) || image.width != data.width ||
        image.height != data.height)
    {
        return false;
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

bool Chapter3Sprites::Draw(Renderer& renderer,
                           Chapter3Person person,
                           Chapter3Motion motion,
                           float seconds,
                           Point foot,
                           bool flip,
                           float height)
{
    const auto index = static_cast<std::size_t>(person);
    auto& atlas = m_Atlases[index];
    if (!atlas.attempted)
    {
        renderer.Flush();
        Load(person, atlas);
    }
    if (!atlas.texture)
    {
        return false;
    }
    const auto& data = Chapter3SpriteData::atlases[index];
    const auto frame = Frame(motion, seconds);
    if (frame >= data.count)
    {
        return false;
    }
    const auto& source = data.frames[frame];
    // One fixed scale for every frame of this NPC. Only its authored foot pivot changes.
    const float scale = height / data.referenceHeight;
    const float width = (source.right - source.left) * scale;
    const float drawnHeight = (source.bottom - source.top) * scale;
    const float anchor = (source.pivotX - source.left) * scale;
    renderer.Sprite(atlas.texture,
                    foot.x - (flip ? width - anchor : anchor),
                    foot.y - (source.pivotY - source.top) * scale,
                    width,
                    drawnHeight,
                    float(flip ? source.right : source.left) / data.width,
                    float(source.top) / data.height,
                    float(flip ? source.left : source.right) / data.width,
                    float(source.bottom) / data.height);
    return true;
}
