#pragma once

#include "Actor.h"
#include "SceneCamera.h"
#include <map>
#include <utility>
#include <vector>

class LevelMap;

struct SceneItem
{
    const Actor* actor;
    Point screen;
};

// Spatial hierarchy: scene root -> streamed chunk nodes -> static actors.
// Dynamic actors form a separate branch and are merged only after culling.
class SceneGraph
{
  public:
    SceneGraph() = default;
    SceneGraph(const SceneGraph&) = delete;
    SceneGraph& operator=(const SceneGraph&) = delete;
    SceneGraph(SceneGraph&&) = delete;
    SceneGraph& operator=(SceneGraph&&) = delete;

    void Clear();
    void Stream(const LevelMap& map, float x, float y, int width, int height);
    void BeginDynamicActors();
    void AddDynamicActor(const WorldProp& description);
    // Queue items borrow actors until the next scene mutation. Build after all additions.
    void BuildRenderQueue(const SceneCamera& camera);
    const std::vector<SceneItem>& RenderQueue() const;
    const std::vector<SceneItem>& StaticVisible() const;

  private:
    struct ChunkNode
    {
        std::vector<Actor> actors;
    };

    void RebuildStaticOrder();
    std::map<std::pair<int, int>, ChunkNode> m_Chunks;
    std::vector<const Actor*> m_StaticOrder;
    std::vector<Actor> m_DynamicActors;
    std::vector<SceneItem> m_StaticVisible;
    std::vector<SceneItem> m_DynamicVisible;
    std::vector<SceneItem> m_RenderQueue;
    int m_CenterX = 0;
    int m_CenterY = 0;
    int m_RadiusX = -1;
    int m_RadiusY = -1;
};
