/*
Copyright 2022 Lee Taek Hee (Tech University of Korea)
This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/
#include "stdafx.h"
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
#include "Dependencies/freeglut.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <cctype>

namespace
{
    std::unique_ptr<Renderer> renderer;
    FrameProfiler frameProfiler;
    PrototypeWorld world;
    MonitorPublisher worldMonitor;
    AttackInput attackInput;
    bool keys[256] = {}, arrows[256] = {}, suppressedKeys[256] = {}, paused = false;
    int width = 1280, height = 720;
    bool benchmarkRender = false;
    int benchmarkFrames = 0;
    auto previous = std::chrono::steady_clock::now();

    int VirtualKey(int key)
    {
        return key >= 'a' && key <= 'z' ? key - 'a' + 'A' : key;
    }

    bool Focused()
    {
        return GetForegroundWindow() == GetActiveWindow();
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
            suppressedKeys[i] = (GetAsyncKeyState(VirtualKey(i)) & 0x8000) != 0;
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
        glutSwapBuffers();
        renderer->EndFrameProfiling();
    }

    void Resize(int w, int h)
    {
        world.CancelMenuPress();
        width = w > 0 ? w : 1;
        height = h > 0 ? h : 1;
        if (renderer)
        {
            renderer->Resize(width, height);
        }
    }

    unsigned char NormalizeKey(unsigned char key)
    {
        // Ctrl+A..Z arrive as control characters; keep movement and key-up paired.
        if (key >= 1 && key <= 26)
        {
            return static_cast<unsigned char>('a' + key - 1);
        }

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
            world.RetryDefeat();
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
        if (key == 'p')
        {
            if (!world.Started())
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
            if (paused || world.IsSkillWindowOpen())
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
            if (paused || world.IsSkillWindowOpen())
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
        if (!Focused() || world.SystemMenuOpen())
        {
            return;
        }
        if (key < 0 || key >= 256 || arrows[key])
        {
            return;
        }
        arrows[key] = true;
        if (key == GLUT_KEY_F10)
        {
            glutLeaveMainLoop();
            return;
        }
        if (key == GLUT_KEY_PAGE_UP)
        {
            world.ChangeInventoryPage(-1);
        }
        if (key == GLUT_KEY_PAGE_DOWN)
        {
            world.ChangeInventoryPage(1);
        }
        if (!renderer)
        {
            return;
        }
        auto& fx = renderer->Effects();
        if (key == GLUT_KEY_F1)
        {
            fx.enabled = !fx.enabled;
        }
        if (key == GLUT_KEY_F2)
        {
            fx.bloom = !fx.bloom;
        }
        if (key == GLUT_KEY_F3)
        {
            fx.vignette = !fx.vignette;
        }
        if (key == GLUT_KEY_F4)
        {
            fx.edgeBlur = !fx.edgeBlur;
        }
    }

    void SpecialUp(int key, int, int)
    {
        if (key >= 0 && key < 256)
        {
            arrows[key] = false;
        }
    }

    void Entry(int state)
    {
        if (state == GLUT_LEFT)
        {
            ClearInput();
        }
    }

    void MouseButton(int button, int state, int x, int y)
    {
        if (!Focused())
        {
            ClearInput();
            return;
        }
        if (world.SystemMenuOpen())
        {
            attackInput.Suspend();
            if (renderer && button == GLUT_LEFT_BUTTON)
            {
                const bool quit = world.SystemMenuMouseButton(
                    state == GLUT_DOWN, x, y, width, height, renderer->Effects());
                if (state == GLUT_UP)
                {
                    ClearInput();
                }
                if (quit)
                {
                    glutLeaveMainLoop();
                }
            }
            return;
        }
        if (paused)
        {
            world.CancelSkillDrag();
            return;
        }
        if (button == GLUT_LEFT_BUTTON)
        {
            world.SkillMouseButton(state == GLUT_DOWN, x, y);
        }
    }

    void MouseMove(int x, int y)
    {
        if (world.SystemMenuOpen())
        {
            world.SystemMenuMouseMove(x, y);
            return;
        }
        world.SkillMouseMove(x, y);
    }

    void Timer(int)
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
            if ((GetAsyncKeyState(VirtualKey(i)) & 0x8000) == 0)
            {
                suppressedKeys[i] = false;
            }
        }
        const bool skillUi = world.MenuOpen();
        const bool aMoves = !skillUi && !world.IsSkillBound('a') && keys['a'];
        const bool dMoves = !skillUi && !world.IsSkillBound('d') && keys['d'];
        const bool sMoves = !skillUi && !world.IsSkillBound('s') && keys['s'];
        const bool wMoves = !skillUi && !world.IsSkillBound('w') && keys['w'];
        float dx = float(dMoves || arrows[GLUT_KEY_RIGHT]) - float(aMoves || arrows[GLUT_KEY_LEFT]);
        float dy = float(sMoves || arrows[GLUT_KEY_DOWN]) - float(wMoves || arrows[GLUT_KEY_UP]);
        if (skillUi)
        {
            dx = 0.f;
            dy = 0.f;
        }
        bool sprint = focused && (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        world.SetPaused(paused || !focused);
        bool attackPressed = attackInput.Poll(focused && !paused && !skillUi && world.Started());
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
            std::_Exit(0);
        }
        frameProfiler.RecordUpdate(std::chrono::duration<double, std::milli>(
                                       std::chrono::steady_clock::now() - updateStart)
                                       .count());
        glutPostRedisplay();
        // Let active rendering exceed the old ~60 Hz timer ceiling, but avoid
        // spending the same CPU budget on an unfocused window.
        const double workMs =
            std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - now)
                .count();
        const double targetMs = focused ? 4.0 : 100.0;
        const unsigned delay = static_cast<unsigned>((std::max)(1.0, targetMs - workMs));
        glutTimerFunc(delay, Timer, 0);
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
        argc = 1; // Do not forward the application flag to freeglut.
        argv[1] = nullptr;
        std::cout << "챕터 2 시작 미리보기: 직업 선택 후 연결된 세계의 회색벽에서 시작합니다.\n";
    }
    if (argc == 2 && std::strcmp(argv[1], "--preview-chapter3") == 0)
    {
        world.EnableChapterPreview("chapter3");
        argc = 1;
        argv[1] = nullptr;
        std::cout << "챕터 3 시작 미리보기: 기념홀에서 시작합니다. 챕터 2 완료·피해 단계 1을 가정하며 챕터 3 진행은 생략하지 않습니다.\n";
    }
    if (argc == 2 && std::strcmp(argv[1], "--benchmark-render") == 0)
    {
        benchmarkRender = true;
        argc = 1;
        argv[1] = nullptr;
    }
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width, height);
    int window = glutCreateWindow("The Long War - Open World");
    if (window <= 0)
    {
        return 1;
    }
    SetWindowTextW(GetActiveWindow(), L"긴 전쟁 - 변경의 여정");
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK || !GLEW_VERSION_3_3)
    {
        std::cerr << "OpenGL 3.3 호환 모드가 필요합니다.\n";
        glutDestroyWindow(window);
        return 1;
    }
    renderer.reset(new Renderer(width, height, frameProfiler));
    if (!renderer->IsInitialized() || !renderer->LoadCharacterSprites())
    {
        std::cerr
            << "렌더러 또는 캐릭터 스프라이트 초기화에 실패했습니다. 위 에셋 오류를 확인하세요.\n";
        renderer.reset();
        glutDestroyWindow(window);
        return 1;
    }
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutCloseFunc(Close);
    glutDisplayFunc(Display);
    glutReshapeFunc(Resize);
    glutKeyboardFunc(KeyDown);
    glutKeyboardUpFunc(KeyUp);
    glutSpecialFunc(SpecialDown);
    glutSpecialUpFunc(SpecialUp);
    glutEntryFunc(Entry);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMove);
    glutPassiveMotionFunc(MouseMove);
    glutIgnoreKeyRepeat(1);
    world.Reset();
    if (benchmarkRender)
    {
        world.Select(1);
    }
    previous = std::chrono::steady_clock::now();
    glutTimerFunc(0, Timer, 0);
    std::cout
        << "1/2 선택. 이동: WASD/방향키. 달리기: Shift. 공격: Ctrl (1-2-3 연계). "
           "인벤토리: I (Page Up/Down). 지도: M. 확인: Space. 스킬: K. 기록: J. 일시정지: P. 시스템 메뉴: Esc (마우스 클릭). 야영지: B. 재도전: Enter. 종료: F10.\n";
    std::cout << "후처리: F1 전체, F2 블룸, F3 비네트, F4 가장자리 흐림, [ ] 노출.\n";
    glutMainLoop();
    // Freeglut close callback releases GPU objects before context destruction.
    return 0;
}
