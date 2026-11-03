#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Owns GLFW and its context. GPU resources must be released before this object.
class GlfwWindow
{
  public:
    GlfwWindow() = default;
    ~GlfwWindow();
    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;

    bool Open(int width, int height);
    GLFWwindow* Handle() const;
    void CursorPixels(double x, double y, int& pixelX, int& pixelY) const;
    void WaitForNextFrame(double frameStart, bool active) const;

  private:
    GLFWwindow* m_Window = nullptr;
    bool m_Initialized = false;
};
