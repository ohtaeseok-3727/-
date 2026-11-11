#pragma once

#include "LevelTypes.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

class Renderer;
class SceneCamera;

// All hits from one skill use share the lifetime of this token.
struct DamageNumberCast
{
};

using DamageCast = std::shared_ptr<const DamageNumberCast>;

class DamageNumbers
{
  public:
    void Add(std::uint64_t target,
             WorldPosition position,
             float height,
             int damage,
             const DamageCast& cast);
    void Update(float dt);
    void Draw(Renderer& renderer, const SceneCamera& camera, WorldPosition origin) const;
    void Clear();

  private:
    struct Entry
    {
        std::uint64_t target;
        WorldPosition position;
        float height;
        std::int64_t total;
        float age;
        std::size_t lane;
        std::weak_ptr<const DamageNumberCast> cast;
    };

    std::vector<Entry> m_Entries;
};
