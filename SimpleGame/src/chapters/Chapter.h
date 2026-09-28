#pragma once

#include "MapLayout.h"
#include "EnemySystem.h"
#include "PlayerInventory.h"
#include "PlayerProgression.h"
#include <optional>
#include <string>
#include <unordered_map>

using ChapterFlags = std::unordered_map<std::string, int>;

class Renderer;
class SceneGraph;
struct Point;

// IDs of maps and objects are local to a chapter, not entries in a global enum.
struct ChapterTravel
{
    std::string chapter;                      // Empty means the current chapter.
    std::optional<MapId> map;                 // Missing means the chapter's initial map.
    std::optional<WorldPosition> destination; // Missing means the authored entry.
};

struct ChapterImpact
{
    int damage = 0;
    WorldPosition push{};
};

// One implementation owns a chapter's state, events, authored maps and presentation.
// Split long implementations into .cpp files, not additional coordinating classes.
class Chapter
{
  public:
    virtual ~Chapter() = default;
    virtual const char* Title() const = 0;
    virtual MapId InitialMap() const = 0;
    virtual MapDefinition BuildMap(MapId map) const = 0;
    virtual void Reset() = 0;
    virtual void EnterMap(MapId map) noexcept = 0;
    virtual void Update(float dt,
                        WorldPosition player,
                        bool interactionHeld,
                        const LevelMap& map,
                        EnemySystem& enemies,
                        PlayerProgression& progress) = 0;
    virtual std::string Interact(const MapLandmark& site,
                                 const LevelMap& map,
                                 EnemySystem& enemies,
                                 PlayerProgression& progress,
                                 PlayerInventory& inventory) = 0;
    virtual void StrikeObjects(WorldPosition player,
                               bool faceLeft,
                               int damage,
                               const LevelMap& map);
    virtual void Retry(EnemySystem& enemies) = 0;
    virtual ChapterImpact ConsumeImpact()
    {
        return {};
    }

    virtual bool BlocksMovement(WorldPosition) const
    {
        return false;
    }

    virtual void StrikeProjectileObjects(WorldPosition,
                                         WorldPosition,
                                         float,
                                         float,
                                         int,
                                         const LevelMap&)
    {
    }

    virtual bool ControlLocked() const = 0;
    virtual bool Complete() const = 0;
    virtual const std::string& Message() const = 0;
    virtual std::string Objective() const = 0;
    // Queried on demand by later chapters; no per-frame copying or concrete state casts.
    virtual ChapterFlags ExportFlags() const;

    virtual bool EnemyVisible(const FieldEnemy& enemy) const;
    virtual void AddActors(SceneGraph& scene, WorldPosition player) const;
    virtual bool DrawLandmark(Renderer& renderer,
                              const MapLandmark& site,
                              Point p,
                              float time) const;
    virtual void DrawActor(Renderer& renderer, int actor, Point p, float time) const;
    virtual void DrawEnemy(Renderer& renderer, const FieldEnemy& enemy, Point p) const = 0;
    virtual void DrawWeather(Renderer& renderer, float time, int width, int height) const;

    const std::optional<ChapterTravel>& PendingTravel() const;
    void ClearTravel() noexcept;

  protected:
    void RequestTravel(ChapterTravel travel);

  private:
    std::optional<ChapterTravel> m_Travel;
};
