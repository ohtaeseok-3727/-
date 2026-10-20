#include "stdafx.h"
#include "MonitorSnapshot.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>

namespace
{
    void Text(std::ostream& out, const std::string& value)
    {
        constexpr char hex[] = "0123456789abcdef";
        out << '"';
        for (unsigned char c : value)
        {
            if (c == '"' || c == '\\')
            {
                out << '\\' << static_cast<char>(c);
            }
            else if (c < 0x20)
            {
                out << "\\u00" << hex[c >> 4] << hex[c & 15];
            }
            else
            {
                out << static_cast<char>(c);
            }
        }
        out << '"';
    }

    void Position(std::ostream& out, std::optional<WorldPosition> position)
    {
        if (!position || !std::isfinite(position->x) || !std::isfinite(position->y))
        {
            out << "null";
            return;
        }
        out << "{\"x\":" << position->x << ",\"y\":" << position->y << '}';
    }

    void Number(std::ostream& out, std::optional<int> value)
    {
        if (value)
        {
            out << *value;
        }
        else
        {
            out << "null";
        }
    }

    void Npc(std::ostream& out, const MonitorNpc& npc)
    {
        out << "{\"id\":";
        Text(out, npc.id);
        out << ",\"name\":";
        Text(out, npc.name);
        out << ",\"role\":";
        Text(out, npc.role);
        out << ",\"state\":";
        Text(out, npc.state);
        out << ",\"detail\":";
        Text(out, npc.detail);
        out << ",\"present\":" << (npc.present ? "true" : "false") << ",\"position\":";
        Position(out, npc.position);
        out << ",\"health\":";
        Number(out, npc.health);
        out << ",\"maxHealth\":";
        Number(out, npc.maxHealth);
        out << '}';
    }
}

std::string MonitorJson::Serialize(const MonitorSnapshot& snapshot,
                                   const std::string& sessionId,
                                   std::uint64_t sequence)
{
    std::ostringstream out;
    out.imbue(std::locale::classic());
    out << std::fixed << std::setprecision(2);
    const auto sent = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();
    out << "{\"schemaVersion\":1,\"type\":\"world.snapshot\",\"sessionId\":";
    Text(out, sessionId);
    out << ",\"sequence\":" << sequence << ",\"sentAtUnixMs\":" << sent << ",\"game\":{\"status\":";
    Text(out, snapshot.status);
    out << ",\"chapter\":";
    Text(out, snapshot.chapter);
    out << ",\"playerPosition\":";
    Position(out, snapshot.playerPosition);
    out << "},\"regions\":[";
    bool firstRegion = true;
    for (const auto& region : snapshot.regions)
    {
        if (!firstRegion)
        {
            out << ',';
        }
        firstRegion = false;
        out << "{\"id\":";
        Text(out, region.id);
        out << ",\"name\":";
        Text(out, region.name);
        out << ",\"kind\":";
        Text(out, region.kind);
        out << ",\"active\":" << (region.active ? "true" : "false")
            << ",\"discovered\":" << (region.discovered ? "true" : "false") << ",\"status\":";
        Text(out, region.observation.status);
        out << ",\"detail\":";
        Text(out, region.observation.detail);
        out << ",\"npcs\":[";
        bool first = true;
        for (const auto& npc : region.observation.npcs)
        {
            if (!first)
            {
                out << ',';
            }
            first = false;
            Npc(out, npc);
        }
        out << "],\"monsters\":[";
        first = true;
        for (const auto& monster : region.monsters)
        {
            if (!first)
            {
                out << ',';
            }
            first = false;
            out << "{\"id\":";
            Text(out, monster.id);
            out << ",\"role\":";
            Text(out, monster.role);
            out << ",\"state\":";
            Text(out, monster.state);
            out << ",\"position\":";
            Position(out, monster.position);
            out << ",\"health\":" << monster.health << ",\"maxHealth\":" << monster.maxHealth
                << ",\"scripted\":" << (monster.scripted ? "true" : "false") << '}';
        }
        out << "]}";
    }
    out << "]}";
    return out.str();
}
