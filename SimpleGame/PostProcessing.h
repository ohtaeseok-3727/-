#pragma once
#include "Dependencies/glew.h"
#include <string>

struct PostSettings
{
    bool enabled = true;
    bool bloom = true;
    bool vignette = true;
    bool edgeBlur = true;
    float exposure = 1.15f;
    float bloomStrength = .32f;
    float bloomThreshold = 1.f;
    float bloomRadius = 2.5f; // Bloom-only sample spacing; edge blur keeps its original radius.
    float vignetteStrength = .38f;
    float edgeBlurStrength = .8f;
};

// Scene-linear HDR -> half-size blur/bloom -> SDR composite. UI is drawn afterwards.
class PostProcessing
{
public:
    ~PostProcessing();
    PostProcessing() = default;
    PostProcessing(const PostProcessing&) = delete;
    PostProcessing& operator=(const PostProcessing&) = delete;
    bool Initialize(int width, int height);
    bool Resize(int width, int height);
    bool BeginScene();
    void Composite();
    std::string Status() const;
    PostSettings settings;
private:
    bool MakeProgram();
    void ReleaseTargets();
    void BindSources(GLuint scene, GLuint blurred, GLuint bloom);
    void DrawPass(int mode);
    GLuint m_Program = 0, m_Array = 0;
    GLuint m_SceneFbo = 0, m_SceneTexture = 0;
    GLuint m_BlurFbo[2] = {}, m_BlurScene[2] = {}, m_BlurBloom[2] = {};
    int m_Width = 0, m_Height = 0, m_HalfWidth = 1, m_HalfHeight = 1;
    bool m_Ready = false;
    GLint m_Mode = -1, m_Direction = -1, m_Threshold = -1;
    GLint m_Enabled = -1, m_Exposure = -1, m_BloomStrength = -1;
    GLint m_VignetteStrength = -1, m_EdgeStrength = -1;
    GLint m_BloomRadius = -1;
};
