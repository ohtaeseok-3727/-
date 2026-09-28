#pragma once

#include "Dependencies/glew.h"
#include <memory>
#include <string>

// Converts UTF-8 text into cached Windows font textures. OpenGL drawing stays in Renderer.
class WindowsTextRenderer
{
  public:
    struct TextSprite
    {
        GLuint texture = 0;
        int width = 0;
        int height = 0;
        int baseline = 0;
    };

    WindowsTextRenderer();
    ~WindowsTextRenderer();
    WindowsTextRenderer(const WindowsTextRenderer&) = delete;
    WindowsTextRenderer& operator=(const WindowsTextRenderer&) = delete;

    bool Initialize();
    bool IsInitialized() const;
    float TextWidth(const std::string& utf8Text) const;
    bool Prepare(const std::string& utf8Text, TextSprite& sprite, bool* cached = nullptr);

  private:
    class Implementation;
    std::unique_ptr<Implementation> m_Impl;
};
