#include "stdafx.h"
#include "KnightSkillView.h"
#include "CheondanView.h"
#include "KnightSkills.h"
#include "KnightSkillData.h"
#include "BasicAttack.h"
#include "CombatVisualData.h"
#include "PixelRestyleHeroData.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cwchar>

namespace
{
    void Frame(Renderer& r,
               const wchar_t* path,
               const KnightSkillData::Frame& f,
               float sheetWidth,
               float sheetHeight,
               Point pivot,
               float scale,
               bool left,
               float alpha = 1.f,
               float angle = 0.f)
    {
        const float c = std::cos(angle), s = std::sin(angle), sign = left ? -1.f : 1.f;
        const auto point = [&](float x, float y)
        {
            x = (x - f.pivotX) * scale;
            y = (y - f.pivotY) * scale;
            return Point{pivot.x + sign * (x * c - y * s), pivot.y + x * s + y * c};
        };
        r.ImageQuad(path,
                    point(0, 0),
                    point(float(f.w), 0),
                    point(float(f.w), float(f.h)),
                    point(0, float(f.h)),
                    alpha,
                    {f.x / sheetWidth, f.y / sheetHeight},
                    {(f.x + f.w) / sheetWidth, (f.y + f.h) / sheetHeight});
    }

    int ThrustPose(float time)
    {
        return time < .135f ? 0 : time < .180f ? 1 : time < .475f ? 2 : 3;
    }

    int WavePose(float time)
    {
        return time < .130f ? 0 : time < .270f ? 1 : time < .480f ? 2 : 3;
    }

    float SkillBodyTime(const KnightSkills& skills)
    {
        const int pose = skills.Kind() == KnightSkillKind::Thrust
                             ? ThrustPose(skills.CastTime())
                             : WavePose(skills.CastTime());
        constexpr float times[] = {0.f, 280.f, 350.f, 520.f};
        return times[pose];
    }

    void SpriteHilt(const CombatVisuals::Sprite& body, Point& hilt, float& angle)
    {
        for (const auto& anchor : PixelRestyleKnight::hilts)
        {
            if (body.x != anchor.x || body.y != anchor.y ||
                std::wcscmp(CombatVisuals::assets[body.asset].path, anchor.path) != 0)
            {
                continue;
            }
            hilt = {(body.dx + anchor.rootX * body.scale) * .32f,
                    (body.dy + anchor.rootY * body.scale) * .32f};
            angle = anchor.angle;
            return;
        }
    }
}

void KnightSkillView::DrawBody(Renderer& r, const KnightSkills& skills, Point feet)
{
    if (skills.Kind() == KnightSkillKind::Ultimate)
    {
        CheondanView::DrawCast(r, skills, feet);
        return;
    }
    r.CombatVisual("knight-2",
                   SkillBodyTime(skills),
                   feet.x,
                   feet.y,
                   skills.FaceLeft(),
                   .32f,
                   false);
}

void KnightSkillView::DrawShot(Renderer& r, const KnightSkillShot& shot, Point ground)
{
    if (!shot.Visible())
    {
        return;
    }
    if (shot.kind == KnightSkillKind::Ultimate)
    {
        CheondanView::DrawWave(r, shot, ground);
        return;
    }
    const Point pivot{ground.x, ground.y - shot.height};
    const float ratio = KnightSkillData::PreviewScale * shot.scale;
    if (shot.kind == KnightSkillKind::Thrust)
    {
        Frame(r,
              L"Skills/Knight/thrust-effect.png",
              KnightSkillData::ThrustEffect[shot.Phase()],
              1536,
              1024,
              pivot,
              .43f * ratio,
              shot.faceLeft);
    }
    else
    {
        const float age = shot.age - shot.delay;
        const float alpha = age < .310f ? 1.f : (std::max)(0.f, 1.f - (age - .310f) / .110f);
        Frame(r,
              L"Skills/Knight/wave.png",
              KnightSkillData::WaveEffect[shot.Phase()],
              1536,
              1024,
              pivot,
              .65f * ratio,
              shot.faceLeft,
              alpha);
    }
}

void KnightSkillView::DrawAura(Renderer& r,
                               const KnightSkills& skills,
                               const BasicAttack& attack,
                               Point feet,
                               bool faceLeft,
                               int walkFrame,
                               float time)
{
    if (skills.Casting() && skills.Kind() == KnightSkillKind::Ultimate)
    {
        return; // The growing blade is the passive aura during the ultimate.
    }
    Point hilt{-6.f, -15.f};
    float angle = .733038f;
    const char* id = "knight-walk";
    float ms = walkFrame * 90.f;
    if (skills.Casting())
    {
        faceLeft = skills.FaceLeft();
        id = "knight-2";
        ms = SkillBodyTime(skills);
    }
    else if (attack.IsAttacking())
    {
        id = attack.ClipId();
        ms = attack.ElapsedMilliseconds();
    }
    for (const auto& clip : CombatVisuals::clips)
    {
        if (std::strcmp(clip.id, id) != 0)
        {
            continue;
        }
        CombatVisuals::Sprite sprites[2];
        if (CombatVisuals::Sample(clip, ms, sprites) > 0)
        {
            SpriteHilt(sprites[0], hilt, angle);
        }
        break;
    }
    const Point pivot{feet.x + (faceLeft ? -hilt.x : hilt.x), feet.y + hilt.y};
    const int frame = static_cast<int>(time * 10.f) % 4;
    const KnightSkillData::Frame aura{frame * 543, 0, 543, 724, 110, 365};
    Frame(r,
          L"Skills/Knight/aura.png",
          aura,
          2172,
          724,
          pivot,
          .18f * KnightSkillData::PreviewScale,
          faceLeft,
          .8f,
          angle);
}
