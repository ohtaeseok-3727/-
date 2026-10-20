#include "stdafx.h"
#include "ArcaneEffect.h"
#include "ArcaneEffectShaders.h"
#include "FrameProfiler.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace
{
    GLuint CompileEffect(GLenum type, const char* source)
    {
        const GLuint shader = glCreateShader(type);
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
            std::cerr << "Arcane effect shader: " << log << '\n';
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }
}

ArcaneEffect::~ArcaneEffect()
{
    if (m_Array)
        glDeleteVertexArrays(1, &m_Array);
    if (m_Program)
        glDeleteProgram(m_Program);
}

bool ArcaneEffect::Initialize()
{
    if (m_Attempted)
        return m_Program != 0 && m_Array != 0;
    m_Attempted = true;
    const GLuint vertex = CompileEffect(GL_VERTEX_SHADER, ArcaneShaders::Vertex);
    const GLuint fragment = CompileEffect(GL_FRAGMENT_SHADER, ArcaneShaders::Fragment);
    if (!vertex || !fragment)
    {
        if (vertex)
            glDeleteShader(vertex);
        if (fragment)
            glDeleteShader(fragment);
        return false;
    }
    m_Program = glCreateProgram();
    if (m_Program)
    {
        glAttachShader(m_Program, vertex);
        glAttachShader(m_Program, fragment);
        glLinkProgram(m_Program);
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    GLint ok = 0;
    if (m_Program)
        glGetProgramiv(m_Program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        if (m_Program)
        {
            char log[2048] = {};
            glGetProgramInfoLog(m_Program, sizeof(log), nullptr, log);
            std::cerr << "Arcane effect link: " << log << '\n';
            glDeleteProgram(m_Program);
        }
        m_Program = 0;
        return false;
    }
    glGenVertexArrays(1, &m_Array);
    m_Viewport = glGetUniformLocation(m_Program, "viewport");
    m_Rect = glGetUniformLocation(m_Program, "effectRect");
    m_Params = glGetUniformLocation(m_Program, "parameters");
    m_Tint = glGetUniformLocation(m_Program, "ink");
    m_Kind = glGetUniformLocation(m_Program, "effectKind");
    m_Linear = glGetUniformLocation(m_Program, "linearOutput");
    return m_Array != 0;
}

bool ArcaneEffect::Draw(Kind kind,
                        float x,
                        float y,
                        float radius,
                        float depth,
                        float rotation,
                        float time,
                        float red,
                        float green,
                        float blue,
                        float opacity,
                        int width,
                        int height,
                        bool linearOutput,
                        FrameProfiler& profiler)
{
    if (!Initialize())
        return false;
    if (radius <= .5f || opacity <= 0.f || width < 1 || height < 1)
        return true;
    const float extent = kind == Kind::Seal ? radius * 1.17f : 480.f;
    const float extentY = kind == Kind::Seal ? extent * (std::max)(.1f, depth) : extent;
    const int left = (std::max)(0, static_cast<int>(std::floor(x - extent)));
    const int right = (std::min)(width, static_cast<int>(std::ceil(x + extent)));
    const int top = (std::max)(0, static_cast<int>(std::floor(y - extentY)));
    const int bottom = (std::min)(height, static_cast<int>(std::ceil(y + extentY)));
    if (right <= left || bottom <= top)
        return true;
    // Renderer flushes first; restore the only state not rebound by its next batch.
    const GLboolean scissor = glIsEnabled(GL_SCISSOR_TEST);
    GLint box[4] = {};
    glGetIntegerv(GL_SCISSOR_BOX, box);
    const int clipLeft = scissor ? (std::max)(left, box[0]) : left;
    const int clipBottom = scissor ? (std::max)(height - bottom, box[1]) : height - bottom;
    const int clipRight = scissor ? (std::min)(right, box[0] + box[2]) : right;
    const int clipTop = scissor ? (std::min)(height - top, box[1] + box[3]) : height - top;
    if (clipRight <= clipLeft || clipTop <= clipBottom)
        return true;
    glEnable(GL_SCISSOR_TEST);
    glScissor(clipLeft, clipBottom, clipRight - clipLeft, clipTop - clipBottom);
    glUseProgram(m_Program);
    glBindVertexArray(m_Array);
    glUniform2f(m_Viewport, float(width), float(height));
    glUniform4f(m_Rect, x, y, extent, extentY);
    glUniform4f(m_Params, radius, depth, rotation, time);
    glUniform4f(m_Tint, red, green, blue, opacity);
    glUniform1i(m_Kind, static_cast<int>(kind));
    glUniform1i(m_Linear, linearOutput ? 1 : 0);
    glBlendFunc(GL_ONE, GL_ONE);
    profiler.RecordDrawCall();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(0);
    glUseProgram(0);
    glScissor(box[0], box[1], box[2], box[3]);
    if (!scissor)
        glDisable(GL_SCISSOR_TEST);
    return true;
}
