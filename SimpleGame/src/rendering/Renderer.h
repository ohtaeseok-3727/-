#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>
#include "Dependencies/glew.h"
#include "PostProcessing.h"
#include "GeometryCache.h"
#include "ArcaneEffect.h"

class WindowsTextRenderer;
class FrameProfiler;

struct Point
{
    float x, y;
};

struct Color
{
    float r, g, b, a;

    Color(float R, float G, float B, float A = 1.f) : r(R), g(G), b(B), a(A)
    {
    }
};

// Screen-space, top-left origin. Triangles retain submission order for 2.5D layering.
class Renderer
{
  public:
    Renderer(int width, int height, FrameProfiler& profiler);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool IsInitialized() const
    {
        return m_Initialized;
    }

    void Resize(int width, int height);
    void Begin();
    void FinishWorld();
    void BeginFrameProfiling();
    void EndFrameProfiling();

    PostSettings& Effects()
    {
        return m_Post.settings;
    }

    ScreenCut& ScreenFracture()
    {
        return m_Post.screenCut;
    }

    ArcaneShock& ArcaneShockwave()
    {
        return m_Post.arcaneShock;
    }

    std::string EffectStatus() const
    {
        return m_Post.Status();
    }

    void Flush();
    bool DrawArcane(ArcaneEffect::Kind kind,
                    Point center,
                    float radius,
                    float depth,
                    float rotation,
                    float age,
                    Color color);
    bool CachedGeometry(int kind, float x, float y, float scale);
    void DrawGeometry(const GeometryCache::Mesh& mesh, Point origin);
    void BeginGeometry();
    void EndGeometry(int kind, float x, float y, float scale);
    void Sprite(GLuint texture,
                float x,
                float y,
                float width,
                float height,
                float u0,
                float v0,
                float u1,
                float v1);
    void Triangle(Point a, Point b, Point c, Color color);
    void ImageQuad(const std::wstring& path,
                   Point a,
                   Point b,
                   Point c,
                   Point d,
                   float alpha = 1.f,
                   Point uv0 = {0.f, 0.f},
                   Point uv1 = {1.f, 1.f});
    void Quad(Point a, Point b, Point c, Point d, Color color);
    void Rect(float x, float y, float width, float height, Color color);
    void Ellipse(float x, float y, float rx, float ry, Color color);
    void Text(float x, float y, const std::string& text, Color color);
    float TextWidth(const std::string& text) const;
    bool LoadCharacterSprites();
    bool CharacterAttack(int profession,
                         const char* clipId,
                         float milliseconds,
                         float footX,
                         float footY,
                         bool faceLeft,
                         float effectScale = 1.f);
    bool CombatVisual(const char* clipId,
                      float milliseconds,
                      float footX,
                      float footY,
                      bool faceLeft = false,
                      float scale = 1.f,
                      bool effects = true,
                      float effectScale = 1.f);
    void Character(int profession,
                   float footX,
                   float footY,
                   bool faceLeft = false,
                   int scale = 2,
                   int direction = -1,
                   int frame = 0);
    void VeteranMercenary(float footX, float footY, int frame = 0);
    void DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a);

  private:
    using Vertex = RenderVertex;
    GeometryCache m_GeometryCache;
    std::size_t m_GeometryStart = 0;

    GLuint Compile(GLenum type, const char* source);
    static float CharacterScale(int profession);
    bool UploadSprite(GLuint& texture, int width, int height, const unsigned char* rgba);
    GLuint m_Program = 0, m_Buffer = 0, m_Array = 0;
    GLint m_Viewport = -1;
    GLint m_LinearOutput = -1;
    bool m_InLinearScene = false;
    PostProcessing m_Post;
    ArcaneEffect m_Arcane;
    FrameProfiler& m_Profiler;
    std::unique_ptr<WindowsTextRenderer> m_TextRenderer;
    std::vector<GLuint> m_CombatTextures;
    std::map<std::wstring, GLuint> m_ImageTextures;
    GLuint m_VeteranMercenaryTexture = 0;
    GLuint m_ActiveTexture = 0;
    int m_Width = 1, m_Height = 1;
    bool m_Initialized = false;
    std::vector<Vertex> m_Vertices;
};
