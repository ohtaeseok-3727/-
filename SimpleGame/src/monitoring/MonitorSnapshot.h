#pragma once

#include "../world/LevelTypes.h"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

struct MonitorNpc
{
    std::string id;
    std::string name;
    std::string role;
    std::string state;
    std::string detail;
    bool present = false;
    std::optional<WorldPosition> position;
    std::optional<int> health;
    std::optional<int> maxHealth;
};

struct MonitorObservation
{
    std::string status = "unknown";
    std::string detail;
    std::vector<MonitorNpc> npcs;
};

struct MonitorMonster
{
    std::string id;
    std::string role;
    std::string state;
    WorldPosition position;
    int health = 0;
    int maxHealth = 0;
    bool scripted = false;
};

struct MonitorRegion
{
    std::string id;
    std::string name;
    std::string kind;
    bool active = false;
    bool discovered = false;
    MonitorObservation observation;
    std::vector<MonitorMonster> monsters;
};

struct MonitorSnapshot
{
    std::string status = "selecting";
    std::string chapter;
    std::optional<WorldPosition> playerPosition;
    std::vector<MonitorRegion> regions;
};

class MonitorJson
{
  public:
    static std::string Serialize(const MonitorSnapshot& snapshot,
                                 const std::string& sessionId,
                                 std::uint64_t sequence);
};
