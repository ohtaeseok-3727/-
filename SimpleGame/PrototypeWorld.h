#pragma once
#include "Renderer.h"
#include "BasicAttack.h"
#include "LevelMap.h"
#include "LevelOneSession.h"
#include "LevelOneView.h"
#include "TutorialMapView.h"
#include <map>
#include <utility>
#include <vector>

class PrototypeWorld
{
  public:
    void Update(float dt, float dx, float dy, bool sprint, bool attackPressed);
    void Draw(Renderer& renderer, int width, int height);

    void Select(int value)
    {
        if (value < 0 || value >= 2)
            return;
        m_Class = value;
        m_Attack.Reset();
        m_Level.Reset(m_Class, m_Map.Seed(), m_Map);
        m_Started = true;
    }

    void Reset();
    void ToggleInventory();
    void ChangeInventoryPage(int direction);
    void ToggleMap();
    void InspectLandmark();

    void SetPaused(bool value)
    {
        if (value && !m_Paused)
        {
            m_Attack.Reset();
        }

        m_Paused = value;
    }

    bool Started() const
    {
        return m_Started;
    }

  private:
    using Object = WorldProp;

    struct Chunk
    {
        std::vector<Object> objects;
    };

    void Stream();
    bool Blocked(float x, float y) const;
    Point Project(float x, float y) const;
    void DrawObject(Renderer& r, const Object& object);
    float m_X = 0, m_Y = 0, m_CameraX = 0, m_CameraY = 0;
    float m_Time = 0, m_Walk = 0;
    int m_Width = 1280, m_Height = 720, m_Class = 1;
    bool m_Started = false, m_Paused = false;
    bool m_FaceLeft = false; // Vertical movement retains the last horizontal facing.
    bool m_Moving = false;
    std::map<std::pair<int, int>, Chunk> m_Chunks;
    BasicAttack m_Attack;
    LevelMap m_Map;
    LevelOneSession m_Level;
    LevelOneView m_LevelView;
    TutorialMapView m_MapView;
    bool m_MapGenerated = false;
};
