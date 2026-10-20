#include "stdafx.h"
#include "Renderer.h"
#include "FrameProfiler.h"
#include "SpriteImage.h"
#include "CombatVisualData.h"
#include "WindowsTextRenderer.h"
#include <cstring>
#include <cmath>
#include <cstddef>
#include <iostream>
#pragma comment(lib, "opengl32.lib")

GLuint Renderer::Compile(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    if (!shader)
        return 0;
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[2048] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "셰이더 컴파일 실패: " << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

Renderer::Renderer(int width, int height, FrameProfiler& profiler)
    : m_Post(profiler), m_Profiler(profiler)
{
    // Embedded prototype shaders avoid dependence on the process working directory.
    const char* vs =
        "#version 330 core\nlayout(location=0) in vec2 pos;\n"
        "layout(location=1) in vec4 color; layout(location=2) in vec2 uv;\n"
        "uniform vec2 viewport; out vec4 tint; out vec2 texCoord;\n"
        "void main(){gl_Position=vec4(pos.x*2.0/viewport.x-1.0,1.0-pos.y*2.0/viewport.y,0,1);tint=color;texCoord=uv;}";
    const char* fs =
        "#version 330 core\nin vec4 tint; in vec2 texCoord; uniform sampler2D sprite;\n"
        "uniform bool linearOutput; out vec4 frag;\n"
        "void main(){frag=tint;if(texCoord.x>=0.0) frag*=texture(sprite,texCoord);if(frag.a<0.01)discard;"
        "if(linearOutput){vec3 c=max(frag.rgb,vec3(0));"
        "frag.rgb=mix(c/12.92,pow((c+0.055)/1.055,vec3(2.4)),step(vec3(0.04045),c));}}";
    GLuint v = Compile(GL_VERTEX_SHADER, vs), f = Compile(GL_FRAGMENT_SHADER, fs);
    if (!v || !f)
    {
        if (v)
            glDeleteShader(v);
        if (f)
            glDeleteShader(f);
        return;
    }
    m_Program = glCreateProgram();
    if (!m_Program)
    {
        glDeleteShader(v);
        glDeleteShader(f);
        return;
    }
    glAttachShader(m_Program, v);
    glAttachShader(m_Program, f);
    glLinkProgram(m_Program);
    glDeleteShader(v);
    glDeleteShader(f);
    GLint ok = 0;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[2048] = {};
        glGetProgramInfoLog(m_Program, sizeof(log), nullptr, log);
        std::cerr << "셰이더 연결 실패: " << log << '\n';
        return;
    }
    m_Viewport = glGetUniformLocation(m_Program, "viewport");
    m_LinearOutput = glGetUniformLocation(m_Program, "linearOutput");
    glUseProgram(m_Program);
    glUniform1i(glGetUniformLocation(m_Program, "sprite"), 0);
    glUseProgram(0);
    glGenVertexArrays(1, &m_Array);
    glGenBuffers(1, &m_Buffer);
    glBindVertexArray(m_Array);
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glVertexAttribPointer(
        1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, r)));
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, u)));
    glBindVertexArray(0);
    Resize(width, height);
    m_Initialized = m_Array != 0 && m_Buffer != 0 && m_Viewport >= 0 && m_LinearOutput >= 0;
    if (m_Initialized)
    {
        m_Initialized = m_Post.Initialize(m_Width, m_Height);
    }
    if (m_Initialized)
    {
        m_TextRenderer.reset(new WindowsTextRenderer());
        m_Initialized = m_TextRenderer->Initialize();
        // Optional effect program: failure leaves the geometric fallback available.
        if (m_Initialized)
            m_Arcane.Initialize();
    }
}

Renderer::~Renderer()
{
    for (const auto& entry : m_ImageTextures)
    {
        glDeleteTextures(1, &entry.second);
    }
    if (!m_CombatTextures.empty())
        glDeleteTextures(static_cast<GLsizei>(m_CombatTextures.size()), m_CombatTextures.data());
    if (m_VeteranMercenaryTexture)
        glDeleteTextures(1, &m_VeteranMercenaryTexture);
    if (m_Buffer)
        glDeleteBuffers(1, &m_Buffer);
    if (m_Array)
        glDeleteVertexArrays(1, &m_Array);
    if (m_Program)
        glDeleteProgram(m_Program);
}

void Renderer::Resize(int width, int height)
{
    m_Width = width > 0 ? width : 1;
    m_Height = height > 0 ? height : 1;
    glViewport(0, 0, m_Width, m_Height);
    if (m_Initialized)
        m_Post.Resize(m_Width, m_Height);
}

void Renderer::Begin()
{
    m_Post.screenCut = {};
    m_Post.arcaneShock = {};
    m_Vertices.clear();
    m_InLinearScene = m_Post.BeginScene();
    glViewport(0, 0, m_Width, m_Height);
    glDisable(
        GL_FRAMEBUFFER_SRGB); // Composite shader encodes SDR once; UI already uses display colors.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (m_InLinearScene)
        glClearColor(.0044f, .0058f, .0066f, 1.f);
    else
        glClearColor(.055f, .068f, .075f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::FinishWorld()
{
    Flush();
    m_Profiler.SetPhase(FrameProfiler::Phase::Post);
    m_Post.Composite();
    m_InLinearScene = false;
    m_Profiler.SetPhase(FrameProfiler::Phase::Ui);
}

void Renderer::Triangle(Point a, Point b, Point c, Color t)
{
    for (Point p : {a, b, c})
        m_Vertices.push_back({p.x, p.y, t.r, t.g, t.b, t.a, -1.f, -1.f});
}

void Renderer::Quad(Point a, Point b, Point c, Point d, Color t)
{
    Triangle(a, b, c, t);
    Triangle(a, c, d, t);
}

void Renderer::Rect(float x, float y, float w, float h, Color t)
{
    Quad({x, y}, {x + w, y}, {x + w, y + h}, {x, y + h}, t);
}

void Renderer::Ellipse(float x, float y, float rx, float ry, Color t)
{
    const auto& circle = GeometryCache::Circle();
    for (int i = 0; i < 20; ++i)
    {
        Triangle({x, y},
                 {x + circle[i][0] * rx, y + circle[i][1] * ry},
                 {x + circle[i + 1][0] * rx, y + circle[i + 1][1] * ry},
                 t);
    }
}

bool Renderer::CachedGeometry(int kind, float x, float y, float scale)
{
    if (kind < 0 || kind >= 3)
    {
        return false;
    }
    const bool cached = m_GeometryCache.Append(kind, x, y, scale, m_Vertices);
    m_Profiler.RecordGeometry(cached);
    return cached;
}

void Renderer::DrawGeometry(const GeometryCache::Mesh& mesh, Point origin)
{
    if (mesh.empty())
    {
        return;
    }
    const auto first = m_Vertices.size();
    m_Vertices.insert(m_Vertices.end(), mesh.begin(), mesh.end());
    auto* vertices = m_Vertices.data() + first;
    for (std::size_t i = 0; i < mesh.size(); ++i)
    {
        vertices[i].x += origin.x;
        vertices[i].y += origin.y;
    }
}

void Renderer::BeginGeometry()
{
    m_GeometryStart = m_Vertices.size();
}

void Renderer::EndGeometry(int kind, float x, float y, float scale)
{
    m_GeometryCache.Store(kind, m_Vertices, m_GeometryStart);
    m_Vertices.resize(m_GeometryStart);
    m_GeometryCache.Append(kind, x, y, scale, m_Vertices);
}

void Renderer::Flush()
{
    if (!m_Initialized || m_Vertices.empty())
        return;
    glUseProgram(m_Program);
    glUniform2f(m_Viewport, float(m_Width), float(m_Height));
    glUniform1i(m_LinearOutput, m_InLinearScene ? 1 : 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ActiveTexture);
    glBindVertexArray(m_Array);
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
    glBufferData(
        GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STREAM_DRAW);
    m_Profiler.RecordDrawCall();
    m_Profiler.RecordBatch(m_Vertices.size(), m_Vertices.size() * sizeof(Vertex));
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_Vertices.size()));
    glBindVertexArray(0);
    glUseProgram(0);
    m_Vertices.clear();
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Renderer::LoadCharacterSprites()
{
    if (m_CombatTextures.empty())
        m_CombatTextures.resize(CombatVisuals::assetCount, 0);
    // Preload movement, attack bodies and effects to avoid loading during a combo.
    for (int index = 0; index < CombatVisuals::assetCount; ++index)
    {
        if (m_CombatTextures[index])
            continue;
        SpriteImage image;
        const auto& asset = CombatVisuals::assets[index];
        if (!LoadRawSpriteSheet(asset.path, image) || image.width != asset.width ||
            image.height != asset.height)
            return false;
        GLuint texture = 0;
        if (!UploadSprite(texture, image.width, image.height, image.rgba.data()))
        {
            if (texture)
                glDeleteTextures(1, &texture);
            return false;
        }
        m_CombatTextures[index] = texture;
    }
    return true;
}

bool Renderer::UploadSprite(GLuint& texture, int width, int height, const unsigned char* rgba)
{
    glGenTextures(1, &texture);
    if (!texture)
        return false;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    GLint uploadedWidth = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &uploadedWidth);
    glBindTexture(GL_TEXTURE_2D, 0);
    return uploadedWidth == width;
}

void Renderer::Character(
    int profession, float footX, float footY, bool faceLeft, int scale, int direction, int frame)
{
    if (profession < 0 || profession >= 2 || scale < 1)
        return;
    (void)direction; // Mini sheets contain one eight-frame side-facing cycle.
    const char* id = profession == 0 ? "mage-walk" : "knight-walk";
    const float worldScale = CharacterScale(profession);
    const int selected = frame >= 0 && frame < 8 ? frame : 0;
    for (const auto& clip : CombatVisuals::clips)
    {
        if (std::strcmp(clip.id, id) != 0)
            continue;
        float ms = 0;
        for (int i = 0; i < selected; ++i)
            ms += clip.body[i].ms;
        CombatVisual(
            id, ms, std::round(footX), std::round(footY), faceLeft, worldScale * scale, false);
        break;
    }
}

void Renderer::BeginFrameProfiling()
{
    m_Profiler.BeginFrame();
}

void Renderer::EndFrameProfiling()
{
    m_Profiler.EndFrame();
}

void Renderer::VeteranMercenary(float footX, float footY, int frame)
{
    constexpr int frameCount = 4;
    constexpr int frameWidth = 543;
    constexpr int frameHeight = 724;
    constexpr float opaqueHeight = 585.f;
    constexpr float footPivotY = 651.f;
    constexpr float footPivotX = 268.5f;
    constexpr float visibleHeight = 50.f;

    if (!m_Initialized)
    {
        return;
    }
    if (!m_VeteranMercenaryTexture)
    {
        SpriteImage image;
        if (!LoadRawSpriteSheet(L"NPC\\VeteranMercenary\\idle-transparent-aligned-v2.png", image) ||
            image.width != frameWidth * frameCount || image.height != frameHeight ||
            !UploadSprite(m_VeteranMercenaryTexture, image.width, image.height, image.rgba.data()))
        {
            if (m_VeteranMercenaryTexture)
            {
                glDeleteTextures(1, &m_VeteranMercenaryTexture);
                m_VeteranMercenaryTexture = 0;
            }
            return;
        }
    }

    const int selectedFrame = frame >= 0 && frame < frameCount ? frame : 0;
    const float scale = visibleHeight / opaqueHeight;
    const float width = static_cast<float>(frameWidth) * scale;
    const float height = static_cast<float>(frameHeight) * scale;
    const float x = std::round(footX - footPivotX * scale);
    const float y = std::round(footY - footPivotY * scale);
    const float u0 = static_cast<float>(selectedFrame) / frameCount;
    const float u1 = static_cast<float>(selectedFrame + 1) / frameCount;

    Sprite(m_VeteranMercenaryTexture, x, y, width, height, u0, 0, u1, 1);
}

float Renderer::CharacterScale(int profession)
{
    // Use the same world scale for movement and attacks.
    return profession == 0 ? .35f : .32f;
}

bool Renderer::CharacterAttack(int profession,
                               const char* clipId,
                               float milliseconds,
                               float footX,
                               float footY,
                               bool faceLeft,
                               float effectScale)
{
    if (profession < 0 || profession >= 2)
    {
        return false;
    }

    return CombatVisual(clipId,
                        milliseconds,
                        std::round(footX),
                        std::round(footY),
                        faceLeft,
                        CharacterScale(profession),
                        true,
                        effectScale);
}

bool Renderer::CombatVisual(const char* clipId,
                            float milliseconds,
                            float footX,
                            float footY,
                            bool faceLeft,
                            float scale,
                            bool effects,
                            float effectScale)
{
    if (!m_Initialized || !clipId || scale <= 0 || !std::isfinite(milliseconds))
        return false;
    const CombatVisuals::Clip* clip = nullptr;
    for (int i = 0; i < CombatVisuals::clipCount; ++i)
        if (std::strcmp(clipId, CombatVisuals::clips[i].id) == 0)
        {
            clip = &CombatVisuals::clips[i];
            break;
        }
    if (!clip)
        return false;
    CombatVisuals::Sprite commands[2];
    int count = CombatVisuals::Sample(*clip, milliseconds, commands);
    if (!count)
        return false;
    if (!effects)
        count = 1;
    if (m_CombatTextures.empty())
        m_CombatTextures.resize(CombatVisuals::assetCount, 0);
    // Load before submitting either layer, so a missing effect never leaves a partial draw.
    for (int i = 0; i < count; ++i)
    {
        const int index = commands[i].asset;
        if (!m_CombatTextures[index])
        {
            SpriteImage image;
            const auto& asset = CombatVisuals::assets[index];
            if (!LoadRawSpriteSheet(asset.path, image) || image.width != asset.width ||
                image.height != asset.height)
                return false;
            GLuint texture = 0;
            if (!UploadSprite(texture, image.width, image.height, image.rgba.data()))
            {
                if (texture)
                    glDeleteTextures(1, &texture);
                return false;
            }
            m_CombatTextures[index] = texture;
        }
    }
    for (int i = 0; i < count; ++i)
    {
        const auto& s = commands[i];
        const auto& asset = CombatVisuals::assets[s.asset];
        const float layerScale = scale * (i == 0 ? 1.f : effectScale);
        const float w = s.w * s.scale * layerScale, h = s.h * s.scale * layerScale;
        const float x = footX + (faceLeft ? -s.dx * layerScale - w : s.dx * layerScale),
                    y = footY + s.dy * layerScale;
        float u0 = float(s.x) / asset.width, u1 = float(s.x + s.w) / asset.width;
        if (faceLeft)
        {
            float temp = u0;
            u0 = u1;
            u1 = temp;
        }
        const float v0 = float(s.y) / asset.height, v1 = float(s.y + s.h) / asset.height;
        if (s.blend == 1)
        {
            Flush();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }
        Sprite(m_CombatTextures[s.asset], x, y, w, h, u0, v0, u1, v1);
        if (s.blend == 1)
        {
            Flush();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }
    return true;
}

void Renderer::Text(float x, float y, const std::string& text, Color t)
{
    Flush();
    if (!m_TextRenderer)
    {
        return;
    }

    WindowsTextRenderer::TextSprite sprite;
    bool cached = false;
    if (!m_TextRenderer->Prepare(text, sprite, &cached))
    {
        return;
    }
    m_Profiler.RecordText(cached);

    const float top = y - static_cast<float>(sprite.baseline);
    const float right = x + static_cast<float>(sprite.width);
    const float bottom = top + static_cast<float>(sprite.height);
    Vertex a = {x, top, t.r, t.g, t.b, t.a, 0.f, 0.f};
    Vertex b = {right, top, t.r, t.g, t.b, t.a, 1.f, 0.f};
    Vertex c = {right, bottom, t.r, t.g, t.b, t.a, 1.f, 1.f};
    Vertex d = {x, bottom, t.r, t.g, t.b, t.a, 0.f, 1.f};
    m_ActiveTexture = sprite.texture;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_Vertices.insert(m_Vertices.end(), {a, b, c, a, c, d});
}

float Renderer::TextWidth(const std::string& text) const
{
    if (!m_TextRenderer)
    {
        return 0.f;
    }

    return m_TextRenderer->TextWidth(text);
}

void Renderer::DrawSolidRect(
    float x, float y, float z, float size, float r, float g, float b, float a)
{
    (void)z;
    Rect(m_Width * .5f + x - size * .5f,
         m_Height * .5f - y - size * .5f,
         size,
         size,
         Color(r, g, b, a));
}

void Renderer::Sprite(GLuint texture,
                      float x,
                      float y,
                      float width,
                      float height,
                      float u0,
                      float v0,
                      float u1,
                      float v1)
{
    if (!m_Initialized || !texture)
    {
        return;
    }
    const bool reused = !m_Vertices.empty() && m_ActiveTexture == texture;
    m_Profiler.RecordSprite(reused);
    if (m_ActiveTexture != texture)
    {
        Flush();
    }
    m_ActiveTexture = texture;
    const Vertex a{x, y, 1, 1, 1, 1, u0, v0}, b{x + width, y, 1, 1, 1, 1, u1, v0};
    const Vertex c{x + width, y + height, 1, 1, 1, 1, u1, v1}, d{x, y + height, 1, 1, 1, 1, u0, v1};
    m_Vertices.insert(m_Vertices.end(), {a, b, c, a, c, d});
}

void Renderer::ImageQuad(
    const std::wstring& path, Point a, Point b, Point c, Point d, float alpha, Point uv0, Point uv1)
{
    if (!m_Initialized)
    {
        return;
    }
    auto found = m_ImageTextures.find(path);
    if (found == m_ImageTextures.end())
    {
        Flush();
        SpriteImage image;
        GLuint texture = 0;
        if (!LoadRawSpriteSheet(path, image) ||
            !UploadSprite(texture, image.width, image.height, image.rgba.data()))
        {
            if (texture)
            {
                glDeleteTextures(1, &texture);
            }
            m_ImageTextures.emplace(path, 0);
            std::wcerr << L"Skill image load failed: " << path << L"\n";
            return;
        }
        found = m_ImageTextures.emplace(path, texture).first;
    }
    const GLuint texture = found->second;
    if (!texture)
    {
        return;
    }
    m_Profiler.RecordSprite(!m_Vertices.empty() && m_ActiveTexture == texture);
    if (m_ActiveTexture != texture)
    {
        Flush();
    }
    m_ActiveTexture = texture;
    const Vertex va{a.x, a.y, 1, 1, 1, alpha, uv0.x, uv0.y},
        vb{b.x, b.y, 1, 1, 1, alpha, uv1.x, uv0.y};
    const Vertex vc{c.x, c.y, 1, 1, 1, alpha, uv1.x, uv1.y},
        vd{d.x, d.y, 1, 1, 1, alpha, uv0.x, uv1.y};
    m_Vertices.insert(m_Vertices.end(), {va, vb, vc, va, vc, vd});
}
