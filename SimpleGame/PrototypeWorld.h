#pragma once
#include "Renderer.h"
#include <map>
#include <utility>
#include <vector>

class PrototypeWorld
{
public:
    void Update(float dt, float dx, float dy, bool sprint);
    void Draw(Renderer& renderer, int width, int height);
    void Select(int value) { if (value < 0 || value >= 2) return; m_Class = value; m_Started = true; }
    void Reset();
    void SetPaused(bool value) { m_Paused = value; }
    bool Started() const { return m_Started; }
private:
    struct Object { float x, y; int kind; float size; };
    struct Chunk { std::vector<Object> objects; };
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
    std::map<std::pair<int,int>, Chunk> m_Chunks;
};
