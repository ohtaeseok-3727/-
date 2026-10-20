#pragma once
#include <array>
#include <cmath>
#include <vector>

struct RenderVertex
{
    float x, y, r, g, b, a, u, v;
};

// Immutable local-space prototypes. Instances keep only position and scale.
class GeometryCache
{
  public:
    using Mesh = std::vector<RenderVertex>;

    bool Append(int kind, float x, float y, float scale, Mesh& destination) const
    {
        const auto& mesh = m_Props[static_cast<std::size_t>(kind)];
        if (mesh.empty())
        {
            return false;
        }
        const auto first = destination.size();
        destination.insert(destination.end(), mesh.begin(), mesh.end());
        auto* vertices = destination.data() + first;
        for (std::size_t i = 0; i < mesh.size(); ++i)
        {
            vertices[i].x = x + vertices[i].x * scale;
            vertices[i].y = y + vertices[i].y * scale;
        }
        return !mesh.empty();
    }

    void Store(int kind, const Mesh& source, std::size_t first)
    {
        m_Props[static_cast<std::size_t>(kind)].assign(source.begin() + first, source.end());
    }

    static const std::array<std::array<float, 2>, 21>& Circle()
    {
        static const auto points = []
        {
            std::array<std::array<float, 2>, 21> result{};
            for (std::size_t i = 0; i < result.size(); ++i)
            {
                const float angle = static_cast<float>(i) * 6.2831853f / 20.f;
                result[i] = {std::cos(angle), std::sin(angle)};
            }
            return result;
        }();
        return points;
    }

  private:
    std::array<Mesh, 3> m_Props;
};
