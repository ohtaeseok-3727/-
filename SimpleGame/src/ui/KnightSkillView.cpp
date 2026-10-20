#include "stdafx.h"
#include "KnightSkillView.h"
#include "CheondanView.h"
#include "KnightSkills.h"
#include "KnightSkillData.h"
#include "BasicAttack.h"
#include "CombatVisualData.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>

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
}

void KnightSkillView::DrawBody(Renderer& r, const KnightSkills& skills, Point feet)
{
    if (skills.Kind() == KnightSkillKind::Ultimate)
    {
        CheondanView::DrawCast(r, skills, feet);
        return;
    }
    if (skills.Kind() == KnightSkillKind::Thrust)
    {
        Frame(r,
              L"Skills/Knight/thrust.png",
              KnightSkillData::ThrustBody[ThrustPose(skills.CastTime())],
              2172,
              724,
              feet,
              .44f * KnightSkillData::PreviewScale,
              skills.FaceLeft());
    }
    else
    {
        Frame(r,
              L"Skills/Knight/wave.png",
              KnightSkillData::WaveBody[WavePose(skills.CastTime())],
              1536,
              1024,
              feet,
              KnightSkillData::PlayerScale,
              skills.FaceLeft());
    }
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
    Point hilt{-43.f * KnightSkillData::PlayerScale, -79.f * KnightSkillData::PlayerScale};
    float angle = .733038f;
    if (skills.Casting())
    {
        faceLeft = skills.FaceLeft();
        if (skills.Kind() == KnightSkillKind::Thrust)
        {
            const int pose = ThrustPose(skills.CastTime());
            const Point hilts[] = {{105, 464}, {151, 445}, {327, 390}, {122, 464}};
            const auto& frame = KnightSkillData::ThrustBody[pose];
            const float scale = .44f * KnightSkillData::PreviewScale;
            hilt = {(hilts[pose].x - frame.pivotX) * scale, (hilts[pose].y - frame.pivotY) * scale};
            angle = pose == 2 ? 0.f : .733038f;
        }
        else
        {
            const int pose = WavePose(skills.CastTime());
            const Point hilts[] = {{146, 374}, {155, 335}, {213, 331}, {70, 374}};
            const auto& frame = KnightSkillData::WaveBody[pose];
            hilt = {(hilts[pose].x - frame.pivotX) * KnightSkillData::PlayerScale,
                    (hilts[pose].y - frame.pivotY) * KnightSkillData::PlayerScale};
            angle = pose == 2 ? 0.f : .733038f;
        }
    }
    else
    {
        const char* id = attack.IsAttacking() ? attack.ClipId() : "knight-walk";
        const float ms = attack.IsAttacking() ? attack.ElapsedMilliseconds() : walkFrame * 90.f;
        for (const auto& clip : CombatVisuals::clips)
        {
            if (std::strcmp(clip.id, id) != 0)
            {
                continue;
            }
            CombatVisuals::Sprite sprites[2];
            if (CombatVisuals::Sample(clip, ms, sprites) > 0)
            {
                const auto& body = sprites[0];
                // Cell-local sword roots measured in the eight-frame walk sheet.
                // The second row sits about 65 source pixels higher within its cells.
                // Reusing frame 0's root there adds an artificial 9.36px downward jump.
                static constexpr std::array<Point, 8> walkHilts{{{149, 374},
                                                                 {149, 379},
                                                                 {141, 373},
                                                                 {116, 373},
                                                                 {157, 307},
                                                                 {154, 311},
                                                                 {147, 311},
                                                                 {115, 308}}};
                const int walkPose = (body.y / 512) * 4 + body.x / 384;
                Point source = walkHilts[std::clamp(walkPose, 0, 7)];
                if (attack.IsAttacking())
                {
                    const int pose = (std::min)(5, body.x / 256);
                    const Point slash[] = {
                        {80, 239}, {64, 152}, {139, 195}, {177, 191}, {139, 207}, {80, 239}};
                    const Point chop[] = {
                        {83, 241}, {96, 94}, {81, 120}, {122, 257}, {119, 260}, {83, 241}};
                    const Point thrust[] = {
                        {80, 272}, {102, 219}, {149, 207}, {174, 207}, {149, 213}, {80, 272}};
                    source = attack.Step() == 2   ? slash[pose]
                             : attack.Step() == 3 ? chop[pose]
                                                  : thrust[pose];
                    const float angles[] = {.733038f, -2.35f, 0.f, 0.f, .25f, .733038f};
                    angle = angles[pose];
                    if (attack.Step() == 3 && (pose == 1 || pose == 2))
                    {
                        angle = -1.57f;
                    }
                }
                // Use the exact sprite pivot/scale for each movement or attack frame.
                hilt = {(body.dx + source.x * body.scale) * .32f,
                        (body.dy + source.y * body.scale) * .32f};
            }
            break;
        }
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
