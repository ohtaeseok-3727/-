#include "stdafx.h"
#include "Renderer.h"
#include "SpriteImage.h"
#include "Dependencies/freeglut.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#pragma comment(lib, "opengl32.lib")

GLuint Renderer::Compile(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    if (!shader) return 0;
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compilation failed: " << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

Renderer::Renderer(int width, int height)
{
    // Embedded prototype shaders avoid dependence on the process working directory.
    const char* vs = "#version 330 core\nlayout(location=0) in vec2 pos;\n"
        "layout(location=1) in vec4 color; layout(location=2) in vec2 uv;\n"
        "uniform vec2 viewport; out vec4 tint; out vec2 texCoord;\n"
        "void main(){gl_Position=vec4(pos.x*2.0/viewport.x-1.0,1.0-pos.y*2.0/viewport.y,0,1);tint=color;texCoord=uv;}";
    const char* fs = "#version 330 core\nin vec4 tint; in vec2 texCoord; uniform sampler2D sprite;\n"
        "out vec4 frag; void main(){frag=tint;if(texCoord.x>=0.0) frag*=texture(sprite,texCoord);if(frag.a<0.01)discard;}";
    GLuint v = Compile(GL_VERTEX_SHADER, vs), f = Compile(GL_FRAGMENT_SHADER, fs);
    if (!v || !f) { if (v) glDeleteShader(v); if (f) glDeleteShader(f); return; }
    m_Program = glCreateProgram();
    if (!m_Program) { glDeleteShader(v); glDeleteShader(f); return; }
    glAttachShader(m_Program, v); glAttachShader(m_Program, f);
    glLinkProgram(m_Program);
    glDeleteShader(v); glDeleteShader(f);
    GLint ok = 0;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048] = {};
        glGetProgramInfoLog(m_Program, sizeof(log), nullptr, log);
        std::cerr << "Shader link failed: " << log << '\n';
        return;
    }
    m_Viewport = glGetUniformLocation(m_Program, "viewport");
    glUseProgram(m_Program);
    glUniform1i(glGetUniformLocation(m_Program, "sprite"), 0);
    glUseProgram(0);
    glGenVertexArrays(1, &m_Array); glGenBuffers(1, &m_Buffer);
    glBindVertexArray(m_Array); glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
    glEnableVertexAttribArray(0); glEnableVertexAttribArray(1); glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, r)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, u)));
    glBindVertexArray(0);
    Resize(width, height);
    m_Initialized = m_Array != 0 && m_Buffer != 0 && m_Viewport >= 0;
}
Renderer::~Renderer()
{
    glDeleteTextures(2, m_CharacterTextures);
    if (m_Buffer) glDeleteBuffers(1, &m_Buffer);
    if (m_Array) glDeleteVertexArrays(1, &m_Array);
    if (m_Program) glDeleteProgram(m_Program);
}
void Renderer::Resize(int width, int height)
{
    m_Width = width > 0 ? width : 1; m_Height = height > 0 ? height : 1;
    glViewport(0, 0, m_Width, m_Height);
}
void Renderer::Begin()
{
    m_Vertices.clear();
    glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(.055f, .068f, .075f, 1.f); glClear(GL_COLOR_BUFFER_BIT);
}
void Renderer::Triangle(Point a, Point b, Point c, Color t)
{
    for (Point p : { a,b,c }) m_Vertices.push_back({ p.x,p.y,t.r,t.g,t.b,t.a,-1.f,-1.f });
}
void Renderer::Quad(Point a, Point b, Point c, Point d, Color t)
{ Triangle(a,b,c,t); Triangle(a,c,d,t); }
void Renderer::Rect(float x, float y, float w, float h, Color t)
{ Quad({x,y},{x+w,y},{x+w,y+h},{x,y+h},t); }
void Renderer::Ellipse(float x, float y, float rx, float ry, Color t)
{
    for (int i=0;i<20;++i) {
        float a=i*6.2831853f/20, b=(i+1)*6.2831853f/20;
        Triangle({x,y},{x+std::cos(a)*rx,y+std::sin(a)*ry},{x+std::cos(b)*rx,y+std::sin(b)*ry},t);
    }
}
void Renderer::Flush()
{
    if (!m_Initialized || m_Vertices.empty()) return;
    glUseProgram(m_Program); glUniform2f(m_Viewport, float(m_Width), float(m_Height));
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_ActiveTexture);
    glBindVertexArray(m_Array); glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size()*sizeof(Vertex), m_Vertices.data(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_Vertices.size()));
    glBindVertexArray(0); glUseProgram(0); m_Vertices.clear();
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Renderer::LoadCharacterSprites()
{
    const wchar_t* files[2] = { L"mage-coarse-preview-v2.png", L"knight-coarse-preview-v2.png" };
    for (int i=0; i<2; ++i) {
        if (m_CharacterTextures[i]) continue;
        SpriteImage image;
        if (!LoadMercenarySprite(files[i], image)) return false;
        glGenTextures(1, &m_CharacterTextures[i]);
        if (!m_CharacterTextures[i]) return false;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_CharacterTextures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, image.rgba.data());
        GLint uploadedWidth = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &uploadedWidth);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (uploadedWidth != image.width) return false;
    }
    return true;
}

void Renderer::Character(int profession, float footX, float footY, bool faceLeft, int scale)
{
    if (profession < 0 || profession >= 2 || !m_CharacterTextures[profession] || scale < 1) return;
    Flush(); // Preserve world Y ordering across solid geometry and textured sprites.
    m_ActiveTexture = m_CharacterTextures[profession];
    const float w = 32.f*scale, h = 52.f*scale;
    const float x = std::round(footX)-w*.5f, y = std::round(footY)-50.f*scale;
    float u0 = faceLeft?1.f:0.f, u1 = faceLeft?0.f:1.f;
    Vertex a = {x,y,1,1,1,1,u0,0}, b = {x+w,y,1,1,1,1,u1,0};
    Vertex c = {x+w,y+h,1,1,1,1,u1,1}, d = {x,y+h,1,1,1,1,u0,1};
    m_Vertices.insert(m_Vertices.end(), {a,b,c,a,c,d});
    Flush();
    m_ActiveTexture = 0;
}
void Renderer::Text(float x, float y, const std::string& text, Color t)
{
    Flush(); // Compatibility context is requested explicitly for GLUT bitmap text.
    glColor4f(t.r,t.g,t.b,t.a); glWindowPos2f(x, m_Height-y);
    for (unsigned char c : text) glutBitmapCharacter(GLUT_BITMAP_8_BY_13,c);
}
void Renderer::DrawSolidRect(float x,float y,float z,float size,float r,float g,float b,float a)
{
    (void)z;
    Rect(m_Width*.5f+x-size*.5f,m_Height*.5f-y-size*.5f,size,size,Color(r,g,b,a));
}
