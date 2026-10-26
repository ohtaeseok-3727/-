#include "stdafx.h"
#include "HeroWalkView.h"
#include "SelectedWalkData.h"
#include "SpriteImage.h"
#include <algorithm>

HeroWalkView::~HeroWalkView()
{
    if (m_Texture)
    {
        glDeleteTextures(1, &m_Texture);
    }
}

bool HeroWalkView::Load()
{
    m_Attempted = true;
    SpriteImage image;
    if (!LoadRawSpriteSheet(SelectedWalk::path, image) || image.width != SelectedWalk::width ||
        image.height != SelectedWalk::height)
    {
        return false;
    }
    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image.rgba.data());
    GLint width = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (width != image.width)
    {
        glDeleteTextures(1, &m_Texture);
        m_Texture = 0;
    }
    return m_Texture != 0;
}

bool HeroWalkView::Draw(
    Renderer& renderer, Point foot, float height, bool flip, float time, bool moving)
{
    if (!m_Attempted)
    {
        renderer.Flush();
        Load();
    }
    if (!m_Texture)
    {
        return false;
    }
    const int index = moving ? static_cast<int>((std::max)(0.f, time) / .18f) % 4 : 0;
    const auto& frame = SelectedWalk::frames[2][index];
    const float scale = height / SelectedWalk::referenceHeights[2];
    renderer.Sprite(m_Texture,
                    foot.x - (flip ? frame.w - frame.pivotX : frame.pivotX) * scale,
                    foot.y - frame.pivotY * scale,
                    frame.w * scale,
                    frame.h * scale,
                    float(flip ? frame.x + frame.w : frame.x) / SelectedWalk::width,
                    float(frame.y) / SelectedWalk::height,
                    float(flip ? frame.x : frame.x + frame.w) / SelectedWalk::width,
                    float(frame.y + frame.h) / SelectedWalk::height);
    return true;
}
