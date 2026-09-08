#pragma once
#include <vector>
#include <string>
#include "Dependencies/glew.h"

struct Point { float x, y; };
struct Color { float r, g, b, a; Color(float R, float G, float B, float A = 1.f) : r(R), g(G), b(B), a(A) {} };

// Screen-space, top-left origin. Triangles retain submission order for 2.5D layering.
class Renderer
{
public:
    Renderer(int width, int height);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    bool IsInitialized() const { return m_Initialized; }
    void Resize(int width, int height);
    void Begin();
    void Flush();
    void Triangle(Point a, Point b, Point c, Color color);
    void Quad(Point a, Point b, Point c, Point d, Color color);
    void Rect(float x, float y, float width, float height, Color color);
    void Ellipse(float x, float y, float rx, float ry, Color color);
    void Text(float x, float y, const std::string& text, Color color);
    bool LoadCharacterSprites();
    void Character(int profession, float footX, float footY, bool faceLeft = false, int scale = 2);
    void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
private:
    struct Vertex { float x, y, r, g, b, a, u, v; };
    GLuint Compile(GLenum type, const char* source);
    GLuint m_Program = 0, m_Buffer = 0, m_Array = 0;
    GLint m_Viewport = -1;
    GLuint m_CharacterTextures[2] = {};
    GLuint m_ActiveTexture = 0;
    int m_Width = 1, m_Height = 1;
    bool m_Initialized = false;
    std::vector<Vertex> m_Vertices;
};
