#pragma once
#include "GameSession.h"
#include "SkillSystem.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include "Chapter3.h"
#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <type_traits>

// Versioned field archive. Loading always targets a temporary world.
class SaveArchive
{
  public:
    SaveArchive(std::iostream& stream, bool writing) : m_Stream(stream), m_Writing(writing)
    {
    }

    template <class... T> void operator()(T&... values)
    {
        (Field(values), ...);
    }

    bool Writing() const
    {
        return m_Writing;
    }

  private:
    void Bytes(char* data, std::size_t size);
    void Field(std::string& value);
    void Field(std::mt19937& value);

    template <class T> void Field(T& value)
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            std::uint8_t encoded = value ? 1 : 0;
            Bytes(reinterpret_cast<char*>(&encoded), sizeof(encoded));
            if (encoded > 1)
            {
                throw std::runtime_error("Invalid save boolean");
            }
            value = encoded != 0;
        }
        else if constexpr (std::is_enum_v<T>)
        {
            auto encoded = static_cast<std::underlying_type_t<T>>(value);
            Field(encoded);
            value = static_cast<T>(encoded);
        }
        else if constexpr (std::is_arithmetic_v<T>)
        {
            Bytes(reinterpret_cast<char*>(&value), sizeof(value));
            if constexpr (std::is_floating_point_v<T>)
            {
                if (!std::isfinite(value))
                {
                    throw std::runtime_error("Non-finite save value");
                }
            }
        }
        else
        {
            Fields(value);
        }
    }

    template <class T, std::size_t N> void Field(std::array<T, N>& value)
    {
        for (auto& item : value)
        {
            Field(item);
        }
    }

    template <std::size_t N> void Field(std::bitset<N>& value)
    {
        std::string bits = value.to_string();
        Field(bits);
        if (bits.size() != N || bits.find_first_not_of("01") != std::string::npos)
        {
            throw std::runtime_error("Invalid save flags");
        }
        value = std::bitset<N>(bits);
    }

    template <class T> void Field(std::optional<T>& value)
    {
        bool present = value.has_value();
        Field(present);
        if (!present)
        {
            value.reset();
            return;
        }
        if (!m_Writing)
        {
            value.emplace();
        }
        Field(*value);
    }

    template <class T> void Field(std::vector<T>& value)
    {
        std::uint32_t size = static_cast<std::uint32_t>(value.size());
        Field(size);
        if (size > 20000)
        {
            throw std::runtime_error("Save collection too large");
        }
        if (!m_Writing)
        {
            value.resize(size);
        }
        for (auto& item : value)
        {
            Field(item);
        }
    }

    template <class K, class V> void Field(std::unordered_map<K, V>& value)
    {
        std::uint32_t size = static_cast<std::uint32_t>(value.size());
        Field(size);
        if (size > 128)
        {
            throw std::runtime_error("Save map too large");
        }
        if (m_Writing)
        {
            for (const auto& pair : value)
            {
                K key = pair.first;
                V item = pair.second;
                (*this)(key, item);
            }
        }
        else
        {
            value.clear();
            for (std::uint32_t i = 0; i < size; ++i)
            {
                K key{};
                V item{};
                (*this)(key, item);
                if (!value.emplace(std::move(key), std::move(item)).second)
                {
                    throw std::runtime_error("Duplicate save key");
                }
            }
        }
    }

    void Fields(WorldPosition& value);
    void Fields(EnemyAIState& value);
    void Fields(EnemyStats& value);
    void Fields(FieldEnemy& value);
    void Fields(LootDrop& value);
    void Fields(ChapterTravel& value);
    void Fields(ChapterImpact& value);
    void Fields(Equipment& value);
    void Fields(Chapter1State& value);
    void Fields(Chapter2Outcome& value);
    void Fields(Chapter2State& value);
    void Fields(Chapter2Combat& value);
    void Fields(Chapter3State& value);
    void Fields(PlayerProgression& value);
    void Fields(PlayerInventory& value);
    void Fields(SkillSystem& value);
    void Fields(SkillQuickSlots& value);
    void Fields(GameSession& value);
    void Fields(EnemySystem& value);
    void Fields(FieldPopulation& value);
    void Fields(LootSystem& value);
    void Fields(Chapter& value);
    void Fields(Chapter1& value);
    void Fields(Chapter2& value);
    void Fields(Chapter3& value);
    std::iostream& m_Stream;
    bool m_Writing;
};
