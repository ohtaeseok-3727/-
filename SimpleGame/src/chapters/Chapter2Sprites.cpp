#include "stdafx.h"
#include "CombatCueView.h"
#include "Chapter2Sprites.h"
#include "EnemySystem.h"
#include "SpriteImage.h"
#include <algorithm>
#include <cmath>

namespace
{
    int OnceFrame(float seconds, float duration)
    {
        return std::clamp(static_cast<int>(seconds * 4.f / (std::max)(duration, .001f)), 0, 3);
    }
}

Chapter2Sprites::~Chapter2Sprites()
{
    for (auto& atlas : m_Atlases)
    {
        if (atlas.texture)
        {
            glDeleteTextures(1, &atlas.texture);
        }
    }
}

bool Chapter2Sprites::Load(Sheet sheet, Atlas& atlas) const
{
    atlas.attempted = true;
    const wchar_t* paths[] = {L"..\\Monsters\\Orc\\Common\\common-combat-v2.png",
                              L"..\\Monsters\\Orc\\Elite\\elite-combat-v2.png",
                              L"..\\Monsters\\Orc\\Barkan\\barkan-attacks-v2.png",
                              L"..\\Monsters\\Orc\\Barkan\\barkan-support-v2.png",
                              L"..\\Monsters\\Orc\\Common\\walk-v1.png",
                              L"..\\Monsters\\Orc\\Elite\\walk-v1.png",
                              L"..\\Monsters\\Orc\\Barkan\\walk-v1.png",
                              L"NPC\\Hero\\hero-v1.png"};
    SpriteImage image;
    if (!LoadRawSpriteSheet(paths[static_cast<int>(sheet)], image))
    {
        return false;
    }
    atlas.width = image.width;
    atlas.height = image.height;
    const bool walk = sheet >= Sheet::CommonWalk && sheet <= Sheet::BarkanWalk;
    if (sheet == Sheet::Hero)
    {
        if (image.width != 1122 || image.height != 1402)
        {
            return false;
        }
        atlas.frames = {{0, 0, 220, 330}, {750, 570, 935, 850}, {750, 850, 935, 1120}};
    }
    else
    {
        const int rows = walk ? 2 : (sheet <= Sheet::Elite ? 4 : 3);
        const int cellWidth = walk ? 384 : 640;
        const int cellHeight = walk ? 512 : 640;
        if (image.width != cellWidth * 4 || image.height != cellHeight * rows)
        {
            return false;
        }
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < 4; ++col)
            {
                atlas.frames.push_back({col * cellWidth,
                                        row * cellHeight,
                                        (col + 1) * cellWidth,
                                        (row + 1) * cellHeight,
                                        320.f,
                                        512.f});
            }
        }
    }
    // Combat sheets retain the authored fixed cell and pivot, including airborne pose lift.
    // Walk/Hero sheets predate that format: derive their alpha bounds without editing pixels.
    if (walk || sheet == Sheet::Hero)
    {
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
            bounds.pivotX = (bounds.right - bounds.left) * .5f;
            bounds.pivotY = float(bounds.bottom - bounds.top);
            frame = bounds;
        }
        atlas.referenceHeight = float(atlas.frames.front().bottom - atlas.frames.front().top);
    }
    else
    {
        // First standing-pose heights measured from the v2 metadata. One scale per atlas,
        // never per pose: weapons, crouching, death and jump must not resize the body.
        atlas.referenceHeight = sheet == Sheet::Common         ? 265.f
                                : sheet == Sheet::Elite        ? 291.f
                                : sheet == Sheet::BarkanAttack ? 259.f
                                                               : 323.f;
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

bool Chapter2Sprites::Draw(
    Renderer& renderer, Sheet sheet, int frame, Point foot, float height, bool flip) const
{
    auto& atlas = m_Atlases[static_cast<std::size_t>(sheet)];
    if (!atlas.attempted)
    {
        renderer.Flush();
        Load(sheet, atlas);
    }
    if (!atlas.texture || atlas.frames.empty())
    {
        return false;
    }
    const auto& source =
        atlas.frames[static_cast<std::size_t>((std::max)(0, frame)) % atlas.frames.size()];
    const float scale = height / atlas.referenceHeight;
    const float width = float(source.right - source.left);
    renderer.Sprite(atlas.texture,
                    foot.x - (flip ? width - source.pivotX : source.pivotX) * scale,
                    foot.y - source.pivotY * scale,
                    width * scale,
                    (source.bottom - source.top) * scale,
                    float(flip ? source.right : source.left) / atlas.width,
                    float(source.top) / atlas.height,
                    float(flip ? source.left : source.right) / atlas.width,
                    float(source.bottom) / atlas.height);
    return true;
}

void Chapter2Sprites::DrawEnemy(Renderer& renderer, const FieldEnemy& enemy, Point foot) const
{
    const int appearance = std::clamp(enemy.stats.appearance, 0, 2);
    const bool boss = appearance == 2;
    // Provisional display tuning: players are roughly 50 pixels tall, all orcs larger.
    const float height = boss ? 94.f : appearance == 1 ? 76.f : 66.f;
    Sheet sheet = boss ? Sheet::BarkanSupport : appearance == 1 ? Sheet::Elite : Sheet::Common;
    int frame = static_cast<int>(enemy.animationTime * 4.f) % 4;
    if (enemy.health <= 0)
    {
        frame = (boss ? 8 : 12) + OnceFrame(enemy.stateTime, FieldEnemy::DeathAnimationDuration);
    }
    else if (boss && enemy.action != EnemyAction::None)
    {
        sheet = Sheet::BarkanAttack;
        const int row = enemy.action == EnemyAction::JumpSlam    ? 1
                        : enemy.action == EnemyAction::BattleCry ? 2
                                                                 : 0;
        frame = row * 4 + OnceFrame(enemy.actionTime, enemy.actionDuration);
    }
    else if (enemy.hitFlash > 0.f)
    {
        frame = (boss ? 4 : 8) + OnceFrame(enemy.stateTime, .3f);
    }
    else if (enemy.ai.state == EnemyState::Windup)
    {
        sheet = boss ? Sheet::BarkanAttack : sheet;
        frame = (boss ? 0 : 4) + OnceFrame(enemy.stateTime, enemy.AttackWindupDuration());
    }
    else if (enemy.ai.state == EnemyState::Chase || enemy.ai.state == EnemyState::Patrol ||
             enemy.ai.state == EnemyState::Return)
    {
        sheet = boss ? Sheet::BarkanWalk : appearance == 1 ? Sheet::EliteWalk : Sheet::CommonWalk;
        frame = static_cast<int>(enemy.animationTime * 8.f) % 8;
    }
    CombatCueView::DrawEnemy(renderer, enemy, foot, height);
    renderer.Ellipse(foot.x, foot.y, height * .29f, 7.f, Color(.02f, .03f, .02f, .5f));
    if (!Draw(renderer, sheet, frame, foot, height, enemy.faceLeft))
    {
        renderer.Ellipse(foot.x,
                         foot.y - height * .5f,
                         height * .3f,
                         height * .5f,
                         boss ? Color(.35f, .42f, .2f) : Color(.45f, .55f, .27f));
        renderer.Text(foot.x - 22.f, foot.y - height - 12.f, "오크(임시 외형)", Color(1, .8f, .4f));
    }
    if (enemy.health <= 0)
    {
        return;
    }
    renderer.Rect(foot.x - 23.f, foot.y - height - 7.f, 46.f, 4.f, Color(.14f, .1f, .1f));
    renderer.Rect(foot.x - 23.f,
                  foot.y - height - 7.f,
                  46.f * std::clamp(float(enemy.health) / (std::max)(1, enemy.maxHealth), 0.f, 1.f),
                  4.f,
                  Color(.8f, .23f, .14f));
    if (boss)
    {
        renderer.Text(
            foot.x - 58.f, foot.y - height - 22.f, "바르칸 · 오크 군단장", Color(.95f, .72f, .4f));
    }
    if (enemy.attackSpeedTime > 0.f)
    {
        renderer.Ellipse(foot.x, foot.y, 28.f, 10.f, Color(.9f, .16f, .05f, .3f));
        renderer.Text(foot.x - 26.f, foot.y - height - 36.f, "전투의 함성", Color(1.f, .4f, .2f));
    }
}

void Chapter2Sprites::DrawHero(
    Renderer& renderer, Point foot, float time, bool attacking, bool faceLeft) const
{
    const int frame = attacking ? 1 + static_cast<int>(time * 5.f) % 2 : 0;
    if (!Draw(renderer, Sheet::Hero, frame, foot, 52.f, faceLeft))
    {
        renderer.Rect(foot.x - 12.f, foot.y - 40.f, 24.f, 40.f, Color(.5f, .42f, .35f));
        renderer.Text(foot.x - 24.f, foot.y - 54.f, "용사(임시)", Color(1, .8f, .4f));
    }
}
