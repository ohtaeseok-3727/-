#include "stdafx.h"
#include "SceneGraph.h"
#include "LevelMap.h"
#include <algorithm>
#include <cmath>
#include <iterator>

void SceneGraph::Clear()
{
    m_RenderQueue.clear();
    m_StaticVisible.clear();
    m_DynamicVisible.clear();
    m_StaticOrder.clear();
    m_DynamicActors.clear();
    m_Chunks.clear();
    m_RadiusX = m_RadiusY = -1;
}

void SceneGraph::Stream(const LevelMap& map, float x, float y, int width, int height)
{
    constexpr float chunkSize = 512.f;
    const int cx = static_cast<int>(std::floor(x / chunkSize));
    const int cy = static_cast<int>(std::floor(y / chunkSize));
    const int rx = static_cast<int>(std::ceil(width * .5f / chunkSize)) + 2;
    const int ry =
        static_cast<int>(std::ceil(height * .6f / SceneCamera::GroundDepthScale() / chunkSize)) + 2;
    if (cx == m_CenterX && cy == m_CenterY && rx == m_RadiusX && ry == m_RadiusY)
    {
        return;
    }

    // Invalidate borrowed pointers before removing their owning chunk nodes.
    m_RenderQueue.clear();
    m_StaticVisible.clear();
    m_StaticOrder.clear();
    for (auto it = m_Chunks.begin(); it != m_Chunks.end();)
    {
        if (std::abs(it->first.first - cx) > rx || std::abs(it->first.second - cy) > ry)
        {
            it = m_Chunks.erase(it);
        }
        else
        {
            ++it;
        }
    }
    for (int row = cy - ry; row <= cy + ry; ++row)
    {
        for (int column = cx - rx; column <= cx + rx; ++column)
        {
            const auto key = std::make_pair(column, row);
            if (m_Chunks.find(key) != m_Chunks.end())
            {
                continue;
            }
            ChunkNode node;
            const auto props = map.ChunkObjects(column, row);
            node.actors.reserve(props.size());
            for (const auto& prop : props)
            {
                node.actors.emplace_back(prop);
            }
            m_Chunks.emplace(key, std::move(node));
        }
    }
    RebuildStaticOrder();
    m_CenterX = cx;
    m_CenterY = cy;
    m_RadiusX = rx;
    m_RadiusY = ry;
}

void SceneGraph::RebuildStaticOrder()
{
    for (const auto& entry : m_Chunks)
    {
        for (const auto& actor : entry.second.actors)
        {
            m_StaticOrder.push_back(&actor);
        }
    }
    std::stable_sort(m_StaticOrder.begin(),
                     m_StaticOrder.end(),
                     [](const Actor* left, const Actor* right)
                     {
                         return left->Depth() < right->Depth();
                     });
}

void SceneGraph::BeginDynamicActors()
{
    m_DynamicOrigin = {};
    m_RenderQueue.clear();
    m_DynamicVisible.clear();
    m_DynamicActors.clear();
}

void SceneGraph::AddDynamicActor(const WorldProp& description)
{
    m_RenderQueue.clear();
    m_DynamicVisible.clear();
    auto world = description;
    world.x += m_DynamicOrigin.x;
    world.y += m_DynamicOrigin.y;
    m_DynamicActors.emplace_back(world);
}

void SceneGraph::BuildRenderQueue(const SceneCamera& camera)
{
    m_StaticVisible.clear();
    m_DynamicVisible.clear();
    m_RenderQueue.clear();
    const auto appendVisible = [&camera](const Actor& actor, std::vector<SceneItem>& output)
    {
        const auto& prop = actor.Description();
        const Point screen = camera.Project(prop.x, prop.y);
        // The ultimate blade/wave extends well above the ordinary actor bounds.
        const bool tallEffect =
            (prop.kind == 5 || prop.kind == 12) && camera.IsVisible({screen.x, screen.y - 180.f});
        if (camera.IsVisible(screen, prop.kind == 13 ? prop.size * 2.f : 0.f) || tallEffect)
        {
            output.push_back({&actor, screen});
        }
    };
    for (const Actor* actor : m_StaticOrder)
    {
        appendVisible(*actor, m_StaticVisible);
    }
    for (const auto& actor : m_DynamicActors)
    {
        appendVisible(actor, m_DynamicVisible);
    }
    const auto byDepth = [](const SceneItem& left, const SceneItem& right)
    {
        return left.actor->Depth() < right.actor->Depth();
    };
    std::stable_sort(m_DynamicVisible.begin(), m_DynamicVisible.end(), byDepth);
    // Equal-depth static actors precede dynamic actors, matching the original stable sort.
    m_RenderQueue.reserve(m_StaticVisible.size() + m_DynamicVisible.size());
    std::merge(m_StaticVisible.begin(),
               m_StaticVisible.end(),
               m_DynamicVisible.begin(),
               m_DynamicVisible.end(),
               std::back_inserter(m_RenderQueue),
               byDepth);
}

const std::vector<SceneItem>& SceneGraph::RenderQueue() const
{
    return m_RenderQueue;
}

const std::vector<SceneItem>& SceneGraph::StaticVisible() const
{
    return m_StaticVisible;
}
