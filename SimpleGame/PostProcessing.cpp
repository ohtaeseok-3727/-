#include "stdafx.h"
#include "PostProcessing.h"
#include "PostProcessingShaders.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace
{
    float Clamp(float value, float low, float high)
    { return !std::isfinite(value) ? low : value < low ? low : value > high ? high : value; }

    GLuint Shader(GLenum type, const char* source)
    {
        GLuint shader = glCreateShader(type);
        if (!shader) return 0;
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[2048] = {};
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            std::cerr << "Post-processing shader: " << log << '\n';
            glDeleteShader(shader); return 0;
        }
        return shader;
    }

    void Texture(GLuint texture, int width, int height, GLint filter)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }
}

bool PostProcessing::MakeProgram()
{
    GLuint vs = Shader(GL_VERTEX_SHADER, PostShaders::Vertex);
    GLuint fs = Shader(GL_FRAGMENT_SHADER, PostShaders::Fragment);
    if (!vs || !fs) { if (vs) glDeleteShader(vs); if (fs) glDeleteShader(fs); return false; }
    m_Program = glCreateProgram();
    if (!m_Program) { glDeleteShader(vs); glDeleteShader(fs); return false; }
    glAttachShader(m_Program, vs); glAttachShader(m_Program, fs);
    glLinkProgram(m_Program); glDeleteShader(vs); glDeleteShader(fs);
    GLint success = 0;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[2048] = {};
        glGetProgramInfoLog(m_Program, sizeof(log), nullptr, log);
        std::cerr << "Post-processing link: " << log << '\n'; return false;
    }
    glUseProgram(m_Program);
    glUniform1i(glGetUniformLocation(m_Program, "sourceScene"), 0);
    glUniform1i(glGetUniformLocation(m_Program, "sourceBlur"), 1);
    glUniform1i(glGetUniformLocation(m_Program, "sourceBloom"), 2);
    glUseProgram(0);
    m_Mode = glGetUniformLocation(m_Program, "mode");
    m_Direction = glGetUniformLocation(m_Program, "direction");
    m_Threshold = glGetUniformLocation(m_Program, "threshold");
    m_Enabled = glGetUniformLocation(m_Program, "enabled");
    m_Exposure = glGetUniformLocation(m_Program, "exposure");
    m_BloomStrength = glGetUniformLocation(m_Program, "bloomStrength");
    m_BloomRadius = glGetUniformLocation(m_Program, "bloomRadius");
    m_VignetteStrength = glGetUniformLocation(m_Program, "vignetteStrength");
    m_EdgeStrength = glGetUniformLocation(m_Program, "edgeStrength");
    return true;
}

bool PostProcessing::Initialize(int width, int height)
{
    if (!MakeProgram()) return false;
    glGenVertexArrays(1, &m_Array);
    return m_Array != 0 && Resize(width, height);
}

void PostProcessing::ReleaseTargets()
{
    glDeleteFramebuffers(1, &m_SceneFbo); m_SceneFbo = 0;
    glDeleteTextures(1, &m_SceneTexture); m_SceneTexture = 0;
    glDeleteFramebuffers(2, m_BlurFbo);
    glDeleteTextures(2, m_BlurScene); glDeleteTextures(2, m_BlurBloom);
    for (int i=0; i<2; ++i) m_BlurFbo[i] = m_BlurScene[i] = m_BlurBloom[i] = 0;
    m_Ready = false;
}

PostProcessing::~PostProcessing()
{
    ReleaseTargets();
    if (m_Array) glDeleteVertexArrays(1, &m_Array);
    if (m_Program) glDeleteProgram(m_Program);
}

bool PostProcessing::Resize(int width, int height)
{
    if (!m_Program) return false;
    width = width > 0 ? width : 1; height = height > 0 ? height : 1;
    if (m_Ready && width == m_Width && height == m_Height) return true;
    ReleaseTargets();
    m_Width = width; m_Height = height;
    m_HalfWidth = (width+1)/2; m_HalfHeight = (height+1)/2;
    glActiveTexture(GL_TEXTURE0);
    glGenFramebuffers(1, &m_SceneFbo); glGenTextures(1, &m_SceneTexture);
    Texture(m_SceneTexture, width, height, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, m_SceneFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_SceneTexture, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    bool complete = m_SceneFbo && m_SceneTexture && glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glGenFramebuffers(2, m_BlurFbo); glGenTextures(2, m_BlurScene); glGenTextures(2, m_BlurBloom);
    const GLenum attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    for (int i=0; i<2; ++i) {
        Texture(m_BlurScene[i], m_HalfWidth, m_HalfHeight, GL_LINEAR);
        Texture(m_BlurBloom[i], m_HalfWidth, m_HalfHeight, GL_LINEAR);
        glBindFramebuffer(GL_FRAMEBUFFER, m_BlurFbo[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BlurScene[i], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_BlurBloom[i], 0);
        glDrawBuffers(2, attachments);
        complete = complete && m_BlurFbo[i] && m_BlurScene[i] && m_BlurBloom[i] &&
            glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
    glBindTexture(GL_TEXTURE_2D, 0); glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (!complete) {
        std::cerr << "HDR framebuffer allocation failed at " << width << 'x' << height << ". Using direct rendering.\n";
        ReleaseTargets(); return false;
    }
    m_Ready = true; return true;
}

bool PostProcessing::BeginScene()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_Ready ? m_SceneFbo : 0);
    return m_Ready;
}
void PostProcessing::BindSources(GLuint scene, GLuint blurred, GLuint bloom)
{
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, scene);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, blurred);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, bloom);
}
void PostProcessing::DrawPass(int mode)
{
    glUniform1i(m_Mode, mode);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
void PostProcessing::Composite()
{
    if (!m_Ready) return;
    glDisable(GL_BLEND); glDisable(GL_DEPTH_TEST); glDisable(GL_FRAMEBUFFER_SRGB);
    glUseProgram(m_Program); glBindVertexArray(m_Array);
    bool needsBlur = settings.enabled && (settings.bloom || settings.edgeBlur);
    if (needsBlur) {
        glViewport(0, 0, m_HalfWidth, m_HalfHeight);
        glBindFramebuffer(GL_FRAMEBUFFER, m_BlurFbo[0]);
        BindSources(m_SceneTexture, 0, 0);
        glUniform1f(m_Threshold, Clamp(settings.bloomThreshold, 0.f, 32.f));
        glUniform1f(m_BloomRadius, Clamp(settings.bloomRadius, 1.f, 4.f));
        DrawPass(0);
        // Ping-pong: never sample a texture attached to the active draw framebuffer.
        for (int pass=0; pass<4; ++pass) {
            int source = pass%2, target = 1-source;
            glBindFramebuffer(GL_FRAMEBUFFER, m_BlurFbo[target]);
            BindSources(m_BlurScene[source], 0, m_BlurBloom[source]);
            glUniform2f(m_Direction, pass%2 == 0 ? 1.f : 0.f, pass%2 == 0 ? 0.f : 1.f);
            DrawPass(1);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); glDrawBuffer(GL_BACK);
    glViewport(0, 0, m_Width, m_Height);
    BindSources(m_SceneTexture, needsBlur ? m_BlurScene[0] : m_SceneTexture,
        needsBlur ? m_BlurBloom[0] : m_SceneTexture);
    glUniform1i(m_Enabled, settings.enabled ? 1 : 0);
    glUniform1f(m_Exposure, Clamp(settings.exposure, .1f, 5.f));
    glUniform1f(m_BloomStrength, settings.bloom ? Clamp(settings.bloomStrength, 0.f, 2.f) : 0.f);
    glUniform1f(m_VignetteStrength, settings.vignette ? Clamp(settings.vignetteStrength, 0.f, .95f) : 0.f);
    glUniform1f(m_EdgeStrength, settings.edgeBlur ? Clamp(settings.edgeBlurStrength, 0.f, 1.f) : 0.f);
    DrawPass(2);
    for (int unit=2; unit>=0; --unit) {
        glActiveTexture(GL_TEXTURE0+unit); glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindVertexArray(0); glUseProgram(0);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

std::string PostProcessing::Status() const
{
    if (!m_Ready) return "POST FX unavailable - direct rendering";
    std::ostringstream text;
    text << "F1 FX:" << (settings.enabled?"ON":"OFF")
        << "  F2 Bloom:" << (settings.bloom?"ON":"OFF")
        << "  F3 Vignette:" << (settings.vignette?"ON":"OFF")
        << "  F4 Edge blur:" << (settings.edgeBlur?"ON":"OFF")
        << "  [ ] Exposure:" << std::fixed << std::setprecision(2) << settings.exposure;
    return text.str();
}
