#include "stdafx.h"
#include "VisualPlaytest.h"
#include "PrototypeWorld.h"
#include "FrameProfiler.h"
#include "EnemyPathfinder.h"
#include "Chapter2.h"
#include <Windows.h>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace
{
    void Require(bool condition, const char* message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    // This window never becomes visible or receives desktop input/focus.
    class HiddenRenderContext
    {
      public:
        HiddenRenderContext()
        {
            WNDCLASSW type{};
            type.style = CS_OWNDC;
            type.lpfnWndProc = DefWindowProcW;
            type.hInstance = GetModuleHandleW(nullptr);
            type.lpszClassName = L"LongWarVisualPlaytest";
            RegisterClassW(&type);
            m_Window = CreateWindowW(type.lpszClassName,
                                     L"",
                                     WS_POPUP,
                                     0,
                                     0,
                                     1280,
                                     720,
                                     nullptr,
                                     nullptr,
                                     type.hInstance,
                                     nullptr);
            Require(m_Window != nullptr, "Hidden render window creation failed");
            m_DC = GetDC(m_Window);
            PIXELFORMATDESCRIPTOR format{};
            format.nSize = sizeof(format);
            format.nVersion = 1;
            format.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
            format.iPixelType = PFD_TYPE_RGBA;
            format.cColorBits = 32;
            const int index = ChoosePixelFormat(m_DC, &format);
            Require(index > 0 && SetPixelFormat(m_DC, index, &format), "Pixel format failed");
            m_Context = wglCreateContext(m_DC);
            Require(m_Context && wglMakeCurrent(m_DC, m_Context), "WGL initialization failed");
            glewExperimental = GL_TRUE;
            Require(glewInit() == GLEW_OK && GLEW_VERSION_3_3, "OpenGL 3.3 required");
        }

        ~HiddenRenderContext()
        {
            wglMakeCurrent(nullptr, nullptr);
            if (m_Context)
            {
                wglDeleteContext(m_Context);
            }
            if (m_DC)
            {
                ReleaseDC(m_Window, m_DC);
            }
            if (m_Window)
            {
                DestroyWindow(m_Window);
            }
        }

      private:
        HWND m_Window = nullptr;
        HDC m_DC = nullptr;
        HGLRC m_Context = nullptr;
    };
}

class VisualPlaytest
{
  public:
    void Run(const std::filesystem::path& output)
    {
        std::filesystem::create_directories(output);
        m_TrialLog.open(output / "combat-trials.csv");
        Require(bool(m_TrialLog), "Trial log creation failed");
        m_TrialLog << "chapter,seed,profession,dodge,seconds,damage,min_health,kills,defeated\n";
        HiddenRenderContext context;
        FrameProfiler profiler;
        Renderer renderer(1280, 720, profiler);
        Require(renderer.IsInitialized() && renderer.LoadCharacterSprites(),
                "Renderer/assets failed");
        PrototypeWorld world;
        // A fixed seed makes before/after scene comparisons reproducible.
        world.Reset();
        world.m_Map.Generate(743);
        world.Select(1);
        Capture(world, renderer, output / "01-start.bmp");
        const auto goal = world.m_Level.TrackedGoal();
        Require(goal.has_value(), "Initial goal missing");
        Walk(world, goal->position, 65.f);
        Capture(world, renderer, output / "02-ration-prompt.bmp");
        world.InspectLandmark();
        Require(world.m_Level.TrackedGoal()->label != goal->label, "Ration interaction failed");
        Capture(world, renderer, output / "03-ration-result.bmp");
        Walk(world, {-420.f, -920.f}, 15.f);
        bool warned = false;
        for (int frame = 0; frame < 1800 && !world.m_Level.IsDefeated(); ++frame)
        {
            world.Update(1.f / 60.f, 0.f, 0.f, false, frame % 30 == 0);
            if (!warned && std::any_of(world.m_Level.Enemies().Enemies().begin(),
                                       world.m_Level.Enemies().Enemies().end(),
                                       [](const FieldEnemy& enemy)
                                       {
                                           return enemy.windup > .1f;
                                       }))
            {
                Capture(world, renderer, output / "04-farm-windup.bmp");
                warned = true;
            }
        }
        Require(warned, "Farm combat windup never observed");
        Capture(world, renderer, output / "05-farm-result.bmp");
        std::cout << "PLAYTEST farm: health=" << world.m_Level.Progress().Health()
                  << " kills=" << world.m_Level.Kills()
                  << " defeated=" << world.m_Level.IsDefeated() << '\n';
        const int gold = world.m_Level.Inventory().Gold();
        world.m_Level.Progress().Damage(world.m_Level.Progress().MaxHealth());
        world.Update(1.f / 60.f, 0.f, 0.f, false, false);
        Capture(world, renderer, output / "05b-defeat.bmp");
        world.RetryDefeat();
        Require(!world.m_Level.IsDefeated() && world.m_Level.Inventory().Gold() == gold,
                "Retry failed or lost gold");
        world.ToggleMap();
        Capture(world, renderer, output / "06-world-map.bmp");
        world.CloseMenus();
        world.EnableChapterPreview("chapter2");
        world.Select(1);
        Capture(world, renderer, output / "07-chapter2.bmp");
        // Entity fixtures isolate cue readability; these do not claim boss progression.
        renderer.Begin();
        auto* chapter = world.m_Level.FindChapter("chapter2");
        for (int i = 0; i < 3; ++i)
        {
            FieldEnemy enemy;
            enemy.position = {};
            enemy.role = i == 1 ? EnemyRole::Ranged : EnemyRole::Melee;
            enemy.ai.state = EnemyState::Windup;
            enemy.stats.appearance = i == 2 ? 1 : 0;
            enemy.windup = .25f;
            enemy.stateTime = .45f;
            chapter->DrawEnemy(renderer, enemy, {290.f + i * 340.f, 350.f});
        }
        renderer.FinishWorld();
        Save(renderer, output / "08-orc-cues.bmp", 1280, 720);
        CaptureBoss(world, renderer, output);
        renderer.Resize(960, 540);
        world.Draw(renderer, 960, 540);
        Save(renderer, output / "09-small-hud.bmp", 960, 540);
        for (const unsigned seed : {743u, 911u, 1201u})
        {
            for (int profession = 0; profession < 2; ++profession)
            {
                CombatTrial(seed, profession, false, false);
                CombatTrial(seed, profession, true, false);
                CombatTrial(seed, profession, false, true);
                CombatTrial(seed, profession, true, true);
            }
        }
        std::cout
            << "PASS hidden visual playtest: movement, interaction, combat, retry, map and scene captures\n";
    }

  private:
    void CaptureBoss(PrototypeWorld& world, Renderer& renderer, const std::filesystem::path& output)
    {
        WorldPosition destination;
        Require(world.m_Level.SkipChapter2FieldStory(destination), "Boss fixture skip failed");
        auto& chapter = static_cast<Chapter2&>(world.m_Level.CurrentChapter());
        Require(chapter.SelectRoute(Chapter2Route::Aqueduct) &&
                    chapter.RecordEvent(Chapter2Event::MuralCopied) &&
                    chapter.RecordEvent(Chapter2Event::DemolitionCut) &&
                    chapter.RecordEvent(Chapter2Event::BypassOpened) &&
                    chapter.RecordEvent(Chapter2Event::HeroRejoined),
                "Boss fixture events failed");
        const auto region = world.m_Level.Atlas().Find("chapter2", Chapter2Maps::OldFortress);
        Require(region.has_value(), "Fortress missing");
        for (const auto action :
             {EnemyAction::Swing, EnemyAction::JumpSlam, EnemyAction::BattleCry})
        {
            const auto position =
                world.m_Level.Atlas().ToWorld(*region,
                                              {action == EnemyAction::Swing      ? -60.f
                                               : action == EnemyAction::JumpSlam ? 260.f
                                                                                 : 120.f,
                                               -2900.f});
            world.m_X = world.m_CameraX = position.x;
            world.m_Y = world.m_CameraY = position.y;
            world.m_Level.Progress().Restore();
            bool captured = false;
            for (int frame = 0; frame < 1000 && !world.m_Level.IsDefeated(); ++frame)
            {
                world.Update(1.f / 60.f, 0.f, 0.f, false, false);
                const auto& enemies = world.m_Level.Enemies().Enemies();
                const auto boss = std::find_if(enemies.begin(),
                                               enemies.end(),
                                               [](const FieldEnemy& enemy)
                                               {
                                                   return enemy.role == EnemyRole::Boss;
                                               });
                if (boss != enemies.end() && boss->action == action && boss->actionTime > .2f)
                {
                    const char* name = action == EnemyAction::Swing      ? "10-boss-swing.bmp"
                                       : action == EnemyAction::JumpSlam ? "11-boss-jump.bmp"
                                                                         : "12-boss-cry.bmp";
                    Capture(world, renderer, output / name);
                    captured = true;
                    break;
                }
            }
            Require(captured, "Boss action never observed");
        }
    }

    void CombatTrial(unsigned seed, int profession, bool dodge, bool chapter2)
    {
        PrototypeWorld world;
        world.Reset();
        world.m_Map.Generate(seed);
        if (chapter2)
        {
            world.EnableChapterPreview("chapter2");
        }
        world.Select(profession);
        if (chapter2)
        {
            const auto& enemies = world.m_Level.Enemies().Enemies();
            Require(!enemies.empty(), "Chapter2 field pack missing");
            const auto origin = world.m_Level.WorldOrigin();
            Walk(world,
                 {origin.x + enemies.front().position.x, origin.y + enemies.front().position.y},
                 70.f);
        }
        else
        {
            Walk(world, world.m_Level.TrackedGoal()->position, 65.f);
            world.InspectLandmark();
            Walk(world, {-420.f, -920.f}, 15.f);
        }
        const int initial = world.m_Level.Progress().Health();
        int minimum = initial, received = 0, frames = 0;
        for (; frames < 3600 && !world.m_Level.IsDefeated(); ++frames)
        {
            const FieldEnemy* nearest = nullptr;
            float distance = 1000.f;
            for (const auto& enemy : world.m_Level.Enemies().Enemies())
            {
                const auto origin = world.m_Level.WorldOrigin();
                const WorldPosition position{origin.x + enemy.position.x,
                                             origin.y + enemy.position.y};
                const float candidate = std::hypot(position.x - world.m_X, position.y - world.m_Y);
                if (enemy.health > 0 && candidate < distance)
                {
                    nearest = &enemy;
                    distance = candidate;
                }
            }
            if (!nearest || distance > 380.f)
            {
                break;
            }
            const auto origin = world.m_Level.WorldOrigin();
            const WorldPosition position{origin.x + nearest->position.x,
                                         origin.y + nearest->position.y};
            float dx = position.x - world.m_X, dy = position.y - world.m_Y;
            const bool retreat =
                dodge && nearest->windup > 0.f && distance < nearest->AttackRange() + 25.f;
            if (retreat)
            {
                dx = -dx;
                dy = -dy;
            }
            const bool attack = !retreat && distance < 70.f;
            const int before = world.m_Level.Progress().Health();
            world.Update(1.f / 60.f, dx, dy, retreat, attack);
            const int after = world.m_Level.Progress().Health();
            received += (std::max)(0, before - after);
            minimum = (std::min)(minimum, after);
        }
        std::cout << "TRIAL chapter=" << (chapter2 ? 2 : 1) << " seed=" << seed
                  << " profession=" << profession << " dodge=" << dodge
                  << " seconds=" << frames / 60.f << " damage=" << received
                  << " min_health=" << minimum << " kills=" << world.m_Level.Kills()
                  << " defeated=" << world.m_Level.IsDefeated() << '\n';
        m_TrialLog << (chapter2 ? 2 : 1) << ',' << seed << ',' << profession << ',' << dodge << ','
                   << frames / 60.f << ',' << received << ',' << minimum << ','
                   << world.m_Level.Kills() << ',' << world.m_Level.IsDefeated() << '\n';
    }

    std::ofstream m_TrialLog;

    void Walk(PrototypeWorld& world, WorldPosition target, float tolerance)
    {
        EnemyPathfinder pathfinder;
        std::vector<WorldPosition> path;
        Require(pathfinder.FindPath({world.m_X, world.m_Y}, target, world.m_Map, path),
                "Walk route missing");
        std::size_t waypoint = 0;
        for (int frame = 0; frame < 2400; ++frame)
        {
            if (std::hypot(target.x - world.m_X, target.y - world.m_Y) < tolerance)
            {
                return;
            }
            while (waypoint < path.size() &&
                   std::hypot(path[waypoint].x - world.m_X, path[waypoint].y - world.m_Y) < 5.f)
            {
                ++waypoint;
            }
            const auto next = waypoint < path.size() ? path[waypoint] : target;
            world.Update(1.f / 60.f, next.x - world.m_X, next.y - world.m_Y, false, false);
        }
        throw std::runtime_error("Automatic walk stalled");
    }

    void Capture(PrototypeWorld& world, Renderer& renderer, const std::filesystem::path& file)
    {
        world.Draw(renderer, 1280, 720);
        Save(renderer, file, 1280, 720);
    }

    void Save(Renderer& renderer, const std::filesystem::path& file, int width, int height)
    {
        renderer.Flush();
        std::vector<unsigned char> pixels(static_cast<std::size_t>(width) * height * 4);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glReadBuffer(GL_BACK);
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pixels.data());
        BITMAPFILEHEADER header{};
        header.bfType = 0x4D42;
        header.bfOffBits = sizeof(header) + sizeof(BITMAPINFOHEADER);
        header.bfSize = header.bfOffBits + static_cast<DWORD>(pixels.size());
        BITMAPINFOHEADER info{};
        info.biSize = sizeof(info);
        info.biWidth = width;
        info.biHeight = height;
        info.biPlanes = 1;
        info.biBitCount = 32;
        std::ofstream stream(file, std::ios::binary);
        stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
        stream.write(reinterpret_cast<const char*>(&info), sizeof(info));
        stream.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
        Require(bool(stream), "Capture write failed");
    }
};

int RunVisualPlaytest(const std::string& outputDirectory)
{
    try
    {
        VisualPlaytest().Run(std::filesystem::u8path(outputDirectory));
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL visual playtest: " << error.what() << '\n';
        return 1;
    }
}
