#pragma once
#include <vector>
#include <string>
#include "Dependencies/glew.h"
#include "PostProcessing.h"

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
    void FinishWorld();
    PostSettings& Effects() { return m_Post.settings; }
    std::string EffectStatus() const { return m_Post.Status(); }
    void Flush();
    void Triangle(Point a, Point b, Point c, Color color);
    void Quad(Point a, Point b, Point c, Point d, Color color);
    void Rect(float x, float y, float width, float height, Color color);
    void Ellipse(float x, float y, float rx, float ry, Color color);
    void Text(float x, float y, const std::string& text, Color color);
    bool LoadCharacterSprites();
    // Mini movement is active; attack clips are prepared but not called by gameplay.
    bool CombatVisual(const char* clipId, float milliseconds, float footX, float footY,
        bool faceLeft = false, float scale = 1.f, bool effects = true);
    void Character(int profession, float footX, float footY, bool faceLeft = false, int scale = 2,
        int direction = -1, int frame = 0);
    void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);
private:
    struct Vertex { float x, y, r, g, b, a, u, v; };
    GLuint Compile(GLenum type, const char* source);
    bool UploadSprite(GLuint& texture, int width, int height, const unsigned char* rgba);
    GLuint m_Program = 0, m_Buffer = 0, m_Array = 0;
    GLint m_Viewport = -1;
    GLint m_LinearOutput = -1;
    bool m_InLinearScene = false;
    PostProcessing m_Post;
    std::vector<GLuint> m_CombatTextures;
    GLuint m_ActiveTexture = 0;
    int m_Width = 1, m_Height = 1;
    bool m_Initialized = false;
    std::vector<Vertex> m_Vertices;
};
