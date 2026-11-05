/*
Copyright 2022 Lee Taek Hee (Tech University of Korea)
This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/
#include "stdafx.h"
#include <Windows.h>
#include "Renderer.h"
#include "FrameProfiler.h"
#include "PrototypeWorld.h"
#include "KnightSkillVerification.h"
#include "MageUltimateVerification.h"
#include "AttackInput.h"
#include "Chapter1Verification.h"
#include "Chapter2Verification.h"
#include "Chapter3Verification.h"
#include "VisualPlaytest.h"
#include "../monitoring/MonitorPublisher.h"
#include <cstring>
#include <exception>
#include <cstdlib>
#include "GlfwWindow.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <cctype>

namespace
{
    GlfwWindow appWindow;
    std::unique_ptr<Renderer> renderer;
    GLFWwindow* window = nullptr;
    FrameProfiler frameProfiler;
    PrototypeWorld world;
    MonitorPublisher worldMonitor;
    AttackInput attackInput;
    bool keys[256] = {}, arrows[GLFW_KEY_LAST + 1] = {}, suppressedKeys[256] = {}, paused = false;
    int width = 1280, height = 720;
    bool benchmarkRender = false;
    int benchmarkFrames = 0;
    auto previous = std::chrono::steady_clock::now();

    int GlfwKey(int key)
    {
        if (key == 27)
        {
            return GLFW_KEY_ESCAPE;
        }
        if (key == 13)
        {
            return GLFW_KEY_ENTER;
        }
        return key >= 'a' && key <= 'z' ? key - 'a' + 'A' : key;
    }

    bool Focused()
    {
        return window && glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE;
    }

    void ClearInput()
    {
        for (bool& k : keys)
        {
            k = false;
        }
        for (bool& k : arrows)
        {
            k = false;
        }
        for (int i = 0; i < 256; ++i)
        {
            suppressedKeys[i] =
                (i >= 32 && i <= 96) || (i >= 'a' && i <= 'z') || i == 13 || i == 27;
            suppressedKeys[i] = suppressedKeys[i] && glfwGetKey(window, GlfwKey(i)) == GLFW_PRESS;
        }
        world.CancelSkillDrag();
        world.CancelMenuPress();
    }

    void Close()
    {
        renderer.reset();
    }

    void Display()
    {
        if (!renderer)
        {
            return;
        }
        renderer->BeginFrameProfiling();
        const auto& effects = renderer->Effects();
        const unsigned flags = (effects.enabled ? 1u : 0u) | (effects.bloom ? 2u : 0u) |
                               (effects.vignette ? 4u : 0u) | (effects.edgeBlur ? 8u : 0u);
        frameProfiler.SetContext(width, height, flags, Focused(), paused || world.SystemMenuOpen());
        world.Draw(*renderer, width, height);
        frameProfiler.SetPhase(FrameProfiler::Phase::Present);
        glfwSwapBuffers(window);
        renderer->EndFrameProfiling();
    }

    void Resize(int w, int h)
    {
        world.CancelMenuPress();
        if (w <= 0 || h <= 0)
        {
            return;
        }
        width = w > 0 ? w : 1;
        height = h > 0 ? h : 1;
        if (renderer)
        {
            renderer->Resize(width, height);
        }
    }

    unsigned char NormalizeKey(unsigned char key)
    {
        return static_cast<unsigned char>(std::tolower(key));
    }

    void KeyDown(unsigned char key, int, int)
    {
        if (!Focused())
        {
            ClearInput();
            return;
        }
        if (key == 13 && !world.SystemMenuOpen())
        {
            if (!world.Started())
            {
                world.LoadGame();
                ClearInput();
            }
            else
            {
                world.RetryDefeat();
            }
            attackInput.Suspend();
            return;
        }
        key = NormalizeKey(key);
        if (suppressedKeys[key])
        {
            return;
        }
        if (keys[key])
        {
            return;
        }
        keys[key] = true;
        if (key == 27)
        {
            if (!world.Started())
            {
                return;
            }
            paused = false;
            world.SetPaused(false);
            world.ToggleSystemMenu();
            ClearInput();
            keys[27] = true;
            attackInput.Suspend();
            return;
        }
        if (world.SystemMenuOpen())
        {
            return;
        }
        if (!world.Started() && key >= '1' && key <= '2')
        {
            world.Select(key - '1');
            return;
        }
        if (key == '1')
        {
            world.DebugLevel20();
            return;
        }
        if (key == 't')
        {
            world.ToggleTrade();
            return;
        }
        if (key == '7' || key == '8')
        {
            world.UsePotion(key == '8');
            return;
        }
        if (key == 'p')
        {
            if (!world.Started() || world.MenuOpen())
            {
                return;
            }
            attackInput.Suspend();
            world.CancelSkillDrag();
            paused = !paused;
            world.SetPaused(paused);
        }
        if (key == 'j')
        {
            world.ToggleJournal();
            return;
        }
        if (key == 'k')
        {
            world.ToggleSkills();
            return;
        }
        if (key >= 'a' && key <= 'z' && world.HandleSkillKey(static_cast<char>(key)))
        {
            return;
        }
        if (key == 'b')
        {
            if (paused || world.MenuOpen())
            {
                return;
            }
            world.ReturnToCamp();
            attackInput.Suspend();
        }
        if (key == 'i')
        {
            world.ToggleInventory();
        }
        if (key == 'm')
        {
            world.ToggleMap();
        }
        if (key == ' ')
        {
            if (paused || world.MenuOpen())
            {
                return;
            }
            world.InspectLandmark();
        }
        if (renderer && key == '[')
        {
            auto& exposure = renderer->Effects().exposure;
            exposure -= .1f;
            if (exposure < .1f)
            {
                exposure = .1f;
            }
        }
        if (renderer && key == ']')
        {
            auto& exposure = renderer->Effects().exposure;
            exposure += .1f;
            if (exposure > 5.f)
            {
                exposure = 5.f;
            }
        }
    }

    void KeyUp(unsigned char key, int, int)
    {
        key = NormalizeKey(key);
        suppressedKeys[key] = false;
        keys[key] = false;
    }

    void SpecialDown(int key, int, int)
    {
        if (!Focused())
        {
            return;
        }
        if (key < 0 || key > GLFW_KEY_LAST || arrows[key])
        {
            return;
        }
        arrows[key] = true;
        if (key == GLFW_KEY_F10)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            return;
        }
        if (key == GLFW_KEY_F5 && !world.SystemMenuOpen())
        {
            world.SaveGame();
            return;
        }
        if (world.SystemMenuOpen())
        {
            return;
        }
        if (key == GLFW_KEY_PAGE_UP)
        {
            world.ChangeInventoryPage(-1);
        }
        if (key == GLFW_KEY_PAGE_DOWN)
        {
            world.ChangeInventoryPage(1);
        }
        if (!renderer)
        {
            return;
        }
        auto& fx = renderer->Effects();
        if (key == GLFW_KEY_F1)
        {
            fx.enabled = !fx.enabled;
        }
        if (key == GLFW_KEY_F2)
        {
            fx.bloom = !fx.bloom;
        }
        if (key == GLFW_KEY_F3)
        {
            fx.vignette = !fx.vignette;
        }
        if (key == GLFW_KEY_F4)
        {
            fx.edgeBlur = !fx.edgeBlur;
        }
    }

    void SpecialUp(int key, int, int)
    {
        if (key >= 0 && key <= GLFW_KEY_LAST)
        {
            arrows[key] = false;
        }
    }

    void MouseButton(int button, int state, int x, int y)
    {
        if (!Focused())
        {
            ClearInput();
            return;
        }
        if (!world.Started())
        {
            if (button == GLFW_MOUSE_BUTTON_LEFT)
            {
                world.TitleMouseButton(state == GLFW_PRESS, x, y);
                if (world.Started())
                {
                    ClearInput();
                    attackInput.Suspend();
                }
            }
            return;
        }
        if (world.SystemMenuOpen())
        {
            attackInput.Suspend();
            if (renderer && button == GLFW_MOUSE_BUTTON_LEFT)
            {
                const bool quit = world.SystemMenuMouseButton(
                    state == GLFW_PRESS, x, y, width, height, renderer->Effects());
                if (state == GLFW_RELEASE)
                {
                    ClearInput();
                }
                if (quit)
                {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
            }
            return;
        }
        if (paused)
        {
            world.CancelSkillDrag();
            return;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            world.SkillMouseButton(state == GLFW_PRESS, x, y);
        }
    }

    void MouseMove(int x, int y)
    {
        world.TitleMouseMove(x, y);
        if (world.SystemMenuOpen())
        {
            world.SystemMenuMouseMove(x, y);
            return;
        }
        world.SkillMouseMove(x, y);
    }

    void UpdateFrame()
    {
        if (!renderer)
        {
            return;
        }
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - previous).count();
        previous = now;
        if (dt > .05f)
        {
            dt = .05f;
        }
        bool focused = benchmarkRender || Focused();
        if (!focused)
        {
            ClearInput();
        }
        for (int i = 0; i < 256; ++i)
        {
            if (suppressedKeys[i] && glfwGetKey(window, GlfwKey(i)) == GLFW_RELEASE)
            {
                suppressedKeys[i] = false;
            }
        }
        const bool skillUi = world.MenuOpen();
        const bool aMoves = !skillUi && !world.IsSkillBound('a') && keys['a'];
        const bool dMoves = !skillUi && !world.IsSkillBound('d') && keys['d'];
        const bool sMoves = !skillUi && !world.IsSkillBound('s') && keys['s'];
        const bool wMoves = !skillUi && !world.IsSkillBound('w') && keys['w'];
        float dx = float(dMoves || arrows[GLFW_KEY_RIGHT]) - float(aMoves || arrows[GLFW_KEY_LEFT]);
        float dy = float(sMoves || arrows[GLFW_KEY_DOWN]) - float(wMoves || arrows[GLFW_KEY_UP]);
        if (skillUi)
        {
            dx = 0.f;
            dy = 0.f;
        }
        bool sprint = focused && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                                  glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
        world.SetPaused(paused || !focused, focused);
        const bool controlDown = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                                 glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
        bool attackPressed =
            attackInput.Poll(focused && !paused && !skillUi && world.Started(), controlDown);
        const auto updateStart = std::chrono::steady_clock::now();
        if (benchmarkRender)
        {
            dt = 1.f / 60.f;
            dx = 0.f;
            dy = benchmarkFrames > 120 ? -1.f : 0.f;
            sprint = false;
            attackPressed = false;
        }
        world.Update(
            dt, dx, dy, sprint, attackPressed, focused && !paused && !skillUi && keys[' ']);
        if (!benchmarkRender && worldMonitor.Due())
        {
            try
            {
                worldMonitor.Publish(world.CaptureMonitoring());
            }
            catch (const std::exception&)
            {
                OutputDebugStringA("World monitor: snapshot could not be prepared.\n");
            }
        }
        if (benchmarkRender && ++benchmarkFrames >= 1200)
        {
            // Automated benchmark has no interactive shutdown; logs flush each interval.
            std::cout.flush();
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        frameProfiler.RecordUpdate(std::chrono::duration<double, std::milli>(
                                       std::chrono::steady_clock::now() - updateStart)
                                       .count());
    }
}

int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);
    if (argc == 3 && std::strcmp(argv[1], "--playtest-visual") == 0)
    {
        return RunVisualPlaytest(argv[2]);
    }
    if (argc == 2 && std::strcmp(argv[1], "--benchmark-enemies") == 0)
    {
        return RunEnemyBenchmark();
    }
    if (argc == 2 && std::strcmp(argv[1], "--verify-mage-ultimate") == 0)
    {
        return RunMageUltimateVerification();
    }
    if (argc == 2 && std::strcmp(argv[1], "--verify-knight-skills") == 0)
    {
        return RunKnightSkillVerification();
    }
    if (argc == 2 && std::strcmp(argv[1], "--verify-open-world") == 0)
    {
        return RunOpenWorldVerification();
    }
    if (argc == 2 && std::strcmp(argv[1], "--verify-chapter1") == 0)
    {
        return RunChapter1Verification();
    }
    if (argc == 2 && std::strcmp(argv[1], "--verify-chapter2-foundation") == 0)
    {
        return RunChapter2Verification();
    }
    if (argc == 2 && std::strcmp(argv[1], "--verify-chapter3") == 0)
    {
        return RunChapter3Verification();
    }
    if (argc == 2 && std::strcmp(argv[1], "--preview-chapter2") == 0)
    {
        world.EnableChapterPreview("chapter2");
        argc = 1;
        argv[1] = nullptr;
        std::cout << "챕터 2 시작 미리보기: 직업 선택 후 연결된 세계의 회색벽에서 시작합니다.\n";
    }
    if (argc == 2 && std::strcmp(argv[1], "--preview-chapter3") == 0)
    {
        world.EnableChapterPreview("chapter3");
        argc = 1;
        argv[1] = nullptr;
        std::cout
            << "챕터 3 시작 미리보기: 기념홀에서 시작합니다. 챕터 2 완료·피해 단계 1을 가정하며 챕터 3 진행은 생략하지 않습니다.\n";
    }
    if (argc == 2 && std::strcmp(argv[1], "--benchmark-render") == 0)
    {
        benchmarkRender = true;
        argc = 1;
        argv[1] = nullptr;
    }
    if (!appWindow.Open(width, height))
    {
        return 1;
    }
    window = appWindow.Handle();
    glfwGetFramebufferSize(window, &width, &height);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK || !GLEW_VERSION_3_3)
    {
        std::cerr << "OpenGL 3.3 호환 모드가 필요합니다.\n";
        return 1;
    }
    renderer.reset(new Renderer(width, height, frameProfiler));
    if (!renderer->IsInitialized() || !renderer->LoadCharacterSprites())
    {
        std::cerr
            << "렌더러 또는 캐릭터 스프라이트 초기화에 실패했습니다. 위 에셋 오류를 확인하세요.\n";
        renderer.reset();
        return 1;
    }
    glfwSetFramebufferSizeCallback(window,
                                   [](GLFWwindow*, int w, int h)
                                   {
                                       Resize(w, h);
                                   });
    glfwSetKeyCallback(window,
                       [](GLFWwindow*, int key, int, int action, int)
                       {
                           if (action == GLFW_REPEAT || key == GLFW_KEY_UNKNOWN)
                           {
                               return;
                           }
                           int character = key;
                           if (key == GLFW_KEY_ESCAPE)
                           {
                               character = 27;
                           }
                           else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER)
                           {
                               character = 13;
                           }
                           else if (key > 255)
                           {
                               if (action == GLFW_PRESS)
                               {
                                   SpecialDown(key, 0, 0);
                               }
                               else
                               {
                                   SpecialUp(key, 0, 0);
                               }
                               return;
                           }
                           if (action == GLFW_PRESS)
                           {
                               KeyDown(static_cast<unsigned char>(character), 0, 0);
                           }
                           else
                           {
                               KeyUp(static_cast<unsigned char>(character), 0, 0);
                           }
                       });
    glfwSetWindowFocusCallback(window,
                               [](GLFWwindow*, int)
                               {
                                   ClearInput();
                                   attackInput.Suspend();
                                   previous = std::chrono::steady_clock::now();
                               });
    glfwSetMouseButtonCallback(window,
                               [](GLFWwindow* handle, int button, int action, int)
                               {
                                   double x, y;
                                   int pixelX, pixelY;
                                   glfwGetCursorPos(handle, &x, &y);
                                   appWindow.CursorPixels(x, y, pixelX, pixelY);
                                   MouseButton(button, action, pixelX, pixelY);
                               });
    glfwSetCursorPosCallback(window,
                             [](GLFWwindow*, double x, double y)
                             {
                                 int pixelX, pixelY;
                                 appWindow.CursorPixels(x, y, pixelX, pixelY);
                                 MouseMove(pixelX, pixelY);
                             });
    world.Reset();
    if (benchmarkRender)
    {
        world.Select(1);
    }
    previous = std::chrono::steady_clock::now();
    std::cout
        << "1/2 선택. 이동: WASD/방향키. 달리기: Shift. 공격: Ctrl (1-2-3 연계). "
           "인벤토리: I (Page Up/Down). 지도: M. 확인: Space. 스킬: K. 기록: J. 일시정지: P. 시스템 메뉴: Esc (마우스 클릭). 야영지: B. 재도전: Enter. 종료: F10.\n";
    std::cout << "후처리: F1 전체, F2 블룸, F3 비네트, F4 가장자리 흐림, [ ] 노출.\n";
    while (!glfwWindowShouldClose(window))
    {
        const double frameStart = glfwGetTime();
        glfwPollEvents();
        if (glfwWindowShouldClose(window))
        {
            break;
        }
        UpdateFrame();
        if (!glfwGetWindowAttrib(window, GLFW_ICONIFIED))
        {
            Display();
        }
        appWindow.WaitForNextFrame(frameStart, benchmarkRender || Focused());
    }
    Close(); // Release GPU resources while the GLFW context is still current.
    return 0;
}
