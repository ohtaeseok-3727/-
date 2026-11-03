#include "stdafx.h"
#include "GlfwWindow.h"
#include <iostream>

GlfwWindow::~GlfwWindow()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
    }
    if (m_Initialized)
    {
        glfwTerminate();
    }
}

bool GlfwWindow::Open(int width, int height)
{
    glfwSetErrorCallback(
        [](int code, const char* description)
        {
            std::cerr << "GLFW " << code << ": " << description << '\n';
        });
    m_Initialized = glfwInit() == GLFW_TRUE;
    if (!m_Initialized)
    {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);
    m_Window = glfwCreateWindow(width, height, u8"긴 전쟁 - 변경의 여정", nullptr, nullptr);
    if (!m_Window)
    {
        return false;
    }
    glfwMakeContextCurrent(m_Window);
    // Preserve the existing high-refresh loop instead of imposing a 60 Hz ceiling.
    glfwSwapInterval(0);
    return true;
}

GLFWwindow* GlfwWindow::Handle() const
{
    return m_Window;
}

void GlfwWindow::CursorPixels(double x, double y, int& pixelX, int& pixelY) const
{
    int width, height, pixelsWide, pixelsHigh;
    glfwGetWindowSize(m_Window, &width, &height);
    glfwGetFramebufferSize(m_Window, &pixelsWide, &pixelsHigh);
    pixelX = width > 0 ? static_cast<int>(x * pixelsWide / width) : 0;
    pixelY = height > 0 ? static_cast<int>(y * pixelsHigh / height) : 0;
}

void GlfwWindow::WaitForNextFrame(double frameStart, bool active) const
{
    const double deadline = frameStart + (active ? 0.004 : 0.100);
    while (!glfwWindowShouldClose(m_Window))
    {
        const double remaining = deadline - glfwGetTime();
        if (remaining <= 0.0)
        {
            break;
        }
        glfwWaitEventsTimeout(remaining);
        if (!active && glfwGetWindowAttrib(m_Window, GLFW_FOCUSED))
        {
            break;
        }
    }
}
