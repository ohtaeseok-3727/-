#include "stdafx.h"
#include "DamageNumbers.h"
#include "SceneCamera.h"
#include <algorithm>
#include <string>

namespace
{
    constexpr float Lifetime = 1.1f;
    constexpr float FadeDuration = .35f;
}

void DamageNumbers::Add(std::uint64_t target,
                        WorldPosition position,
                        float height,
                        int damage,
                        const DamageCast& cast)
{
    if (damage <= 0)
    {
        return;
    }
    if (cast)
    {
        for (auto& entry : m_Entries)
        {
            if (entry.target == target && !entry.cast.owner_before(cast) &&
                !cast.owner_before(entry.cast))
            {
                entry.total += damage;
                entry.position = position;
                entry.age = 0.f;
                return;
            }
        }
    }

    std::size_t lane = 0;
    while (std::any_of(m_Entries.begin(),
                       m_Entries.end(),
                       [target, lane](const Entry& entry)
                       {
                           return entry.target == target && entry.lane == lane;
                       }))
    {
        ++lane;
    }
    m_Entries.push_back({target, position, height, damage, 0.f, lane, cast});
}

void DamageNumbers::Update(float dt)
{
    for (auto& entry : m_Entries)
    {
        entry.age += dt;
    }
    // Keep totals available even if a cast has a gap between hits.
    m_Entries.erase(std::remove_if(m_Entries.begin(),
                                   m_Entries.end(),
                                   [](const Entry& entry)
                                   {
                                       return entry.age >= Lifetime && entry.cast.expired();
                                   }),
                    m_Entries.end());
}

void DamageNumbers::Draw(Renderer& renderer, const SceneCamera& camera, WorldPosition origin) const
{
    constexpr float Scale = 1.25f;
    for (const auto& entry : m_Entries)
    {
        if (entry.age >= Lifetime)
        {
            continue;
        }
        auto point = camera.Project(origin.x + entry.position.x, origin.y + entry.position.y);
        point.y -= entry.height + 30.f * static_cast<float>(entry.lane) + 15.f * entry.age;
        if (!camera.IsVisible(point))
        {
            continue;
        }
        const auto label = std::to_string(entry.total);
        const float alpha = (std::min)(1.f, (Lifetime - entry.age) / FadeDuration);
        const float left = point.x - renderer.TextWidth(label) * Scale * .5f;
        renderer.Text(left + 1.f, point.y + 2.f, label, Color(.04f, .02f, .01f, alpha), Scale);
        renderer.Text(left, point.y, label, Color(1.f, .89f, .56f, alpha), Scale);
    }
}

void DamageNumbers::Clear()
{
    m_Entries.clear();
}
