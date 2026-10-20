#include "stdafx.h"
#include "PrototypeWorld.h"
#include <cmath>
#include <random>
#include "ChapterCatalog.h"
#include "CheondanView.h"
#include "MageUltimateView.h"
#include <stdexcept>

void PrototypeWorld::EnableChapterPreview(const std::string& chapter)
{
    if (m_PreviewChapter.empty())
    {
        ChapterCatalog::RegisterDevelopment(m_Level);
    }
    m_PreviewChapter = chapter;
}

void PrototypeWorld::Select(int value)
{
    if (value < 0 || value >= 2)
    {
        return;
    }
    m_Class = value;
    m_Attack.Reset();
    m_Skills.Reset(m_Class);
    m_MageProjectiles.Reset();
    m_MageUltimate.Reset();
    m_KnightSkills.Reset();
    m_Level.Reset(m_Class, m_Map.Seed(), m_Map);
    m_Level.OpenWorld(m_Map, m_PreviewChapter);
    m_CompletionShown = false;
    m_JournalOpen = false;
    m_Started = true;
    Reset();
}

void PrototypeWorld::Reset()
{
    if (!m_MapGenerated)
    {
        m_Map.Generate(m_PreviewChapter.empty() ? std::random_device{}() : 743u);
        WorldPosition entry;
        if (!m_Level.TravelTo(
                {ChapterCatalog::InitialChapter, std::nullopt, std::nullopt}, m_Map, entry))
        {
            throw std::runtime_error("시작 챕터를 불러오지 못했습니다.");
        }
        m_MapGenerated = true;
    }

    if (m_Started && m_Level.CurrentChapter().ControlLocked())
    {
        return;
    }
    const auto entry =
        m_Level.WorldEnabled() ? m_Level.Checkpoint() : m_Map.Layout().Definition().entry;
    m_X = m_CameraX = entry.x;
    m_Y = m_CameraY = entry.y;
    m_Time = m_Walk = 0;
    m_FaceLeft = false;
    m_Moving = false;
    m_Attack.Reset();
    m_MageProjectiles.Reset();
    m_MageUltimate.Reset();
    m_KnightSkills.Reset();
    m_Skills.CloseWindow();
    m_Skills.CancelDrag();
    m_LevelView.CloseInventory();
    m_MapView.CloseOverview();
    m_JournalOpen = false;
    m_Scene.Clear();
    Stream();
}

void PrototypeWorld::DebugLevel20()
{
    if (!m_Started)
    {
        return;
    }
    m_Level.Progress().DebugSetLevel(20);
    m_Level.ShowSkillNotice("디버그: 레벨 20 · HP/MP 회복 · K에서 스킬을 등록하세요.");
}

void PrototypeWorld::ToggleInventory()
{
    if (m_Started)
    {
        m_JournalOpen = false;
        m_MapView.CloseOverview();
        m_Skills.CloseWindow();
        m_Skills.CancelDrag();
        m_LevelView.ToggleInventory();
    }
}

void PrototypeWorld::ChangeInventoryPage(int direction)
{
    m_LevelView.ChangePage(direction, static_cast<int>(m_Level.Inventory().Items().size()));
}

void PrototypeWorld::ToggleMap()
{
    if (m_Started)
    {
        m_JournalOpen = false;
        m_LevelView.CloseInventory();
        m_Skills.CloseWindow();
        m_Skills.CancelDrag();
        m_MapView.ToggleOverview();
    }
}

void PrototypeWorld::ToggleSkills()
{
    if (m_Started && !m_Paused)
    {
        m_JournalOpen = false;
        m_LevelView.CloseInventory();
        m_MapView.CloseOverview();
        m_Skills.ToggleWindow();
    }
}

bool PrototypeWorld::HandleSkillKey(char key)
{
    if (!m_Started)
    {
        return false;
    }
    if (m_Skills.IsSlotBound(key) &&
        (m_Paused || MenuOpen() || m_Level.CurrentChapter().ControlLocked() ||
         m_Level.IsDefeated() || m_Attack.IsAttacking() || m_MageProjectiles.Casting() ||
         m_MageUltimate.Casting() || m_KnightSkills.Casting()))
    {
        return true;
    }
    std::string notice;
    const bool consumed =
        m_Skills.TryUse(key, m_Level.Progress(), notice, m_Level.Inventory().AttackBonus());
    if (!m_Skills.LastRequest().skillId.empty())
    {
        m_MageProjectiles.Cast(m_Skills.LastRequest(), {m_X, m_Y}, m_FaceLeft);
        m_MageUltimate.Cast(m_Skills.LastRequest(), {m_X, m_Y}, m_FaceLeft);
        m_KnightSkills.Cast(
            m_Skills.LastRequest(), {m_X, m_Y}, m_FaceLeft, m_Level.Progress().AttackRangeScale());
        if (m_MageProjectiles.Casting() || m_MageUltimate.Casting() || m_KnightSkills.Casting())
        {
            m_Attack.Reset();
        }
    }
    if (!notice.empty())
    {
        m_Level.ShowSkillNotice(notice);
    }
    return consumed;
}

bool PrototypeWorld::IsSkillBound(char key) const
{
    return !m_Skills.IsWindowOpen() && m_Skills.IsSlotBound(key);
}

void PrototypeWorld::SkillMouseButton(bool down, int x, int y)
{
    if (m_Started && !m_Paused)
    {
        m_Skills.MouseButton(down, x, y, m_Width, m_Height, m_Level.Progress().Level());
    }
}

void PrototypeWorld::SkillMouseMove(int x, int y)
{
    m_Skills.MouseMove(x, y, m_Width, m_Height);
}

void PrototypeWorld::CancelSkillDrag()
{
    m_Skills.CancelDrag();
}

bool PrototypeWorld::IsSkillWindowOpen() const
{
    return m_Skills.IsWindowOpen();
}

void PrototypeWorld::InspectLandmark()
{
    if (!m_Started || m_Paused || MenuOpen() || m_Level.IsDefeated())
    {
        return;
    }
    const auto* site = m_Map.Layout().NearbyLandmark({m_X, m_Y});
    if (site)
    {
        m_Level.Inspect(*site, m_Map);
    }
}

void PrototypeWorld::Stream()
{
    m_Scene.Stream(m_Map, m_X, m_Y, m_Width, m_Height);
}

bool PrototypeWorld::Blocked(float x, float y) const
{
    return m_Map.Blocked(x, y) || (m_Started && m_Level.BlocksMovement({x, y}));
}

void PrototypeWorld::Update(
    float dt, float dx, float dy, bool sprint, bool attackPressed, bool interactionHeld)
{
    Stream();
    if (!m_Started || m_Paused || MenuOpen() || m_Level.IsDefeated())
    {
        m_Attack.Reset();
        m_Moving = false;
        return;
    }

    m_Level.FollowPlayer({m_X, m_Y});
    if (m_Level.CurrentChapter().ControlLocked())
    {
        dx = dy = 0;
        attackPressed = false;
        m_Attack.Reset();
        m_MageProjectiles.Reset();
        m_MageUltimate.Reset();
        m_KnightSkills.Reset();
    }
    m_Time += dt;
    m_Skills.Update(dt);
    const bool casting =
        m_MageProjectiles.Casting() || m_MageUltimate.Casting() || m_KnightSkills.Casting();
    m_MageProjectiles.Update(dt, m_Level, m_Map);
    m_MageUltimate.Update(dt, m_Level);
    m_KnightSkills.Update(dt, m_Level, m_Map);

    m_Attack.Update(dt);

    for (const int step : m_Attack.ConsumeHits())
    {
        m_Level.Strike(step, {m_X, m_Y}, m_FaceLeft, m_Map);
    }

    if (attackPressed && !casting)
    {
        // Pick the initial facing from movement, then hold it through the strike.
        if (!m_Attack.IsAttacking() && std::abs(dx) > 0.0001f)
        {
            m_FaceLeft = dx < 0.f;
        }

        m_Attack.Press(m_Class);
    }

    // Implementation assumption: each strike keeps the player in place.
    if (m_Attack.IsAttacking() || casting)
    {
        dx = 0.f;
        dy = 0.f;
    }

    m_Moving = false;
    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0)
    {
        float oldX = m_X, oldY = m_Y;
        dx /= length;
        dy /= length;
        float distance = (sprint ? 240.f : 145.f) * dt;
        // Short substeps prevent tunnelling through small props on slow frames.
        int steps = static_cast<int>(std::ceil(distance / 4.f));
        for (int i = 0; i < steps; ++i)
        {
            float nx = m_X + dx * distance / steps, ny = m_Y + dy * distance / steps;
            if (!Blocked(nx, m_Y))
            {
                m_X = nx;
            }
            if (!Blocked(m_X, ny))
            {
                m_Y = ny;
            }
        }
        float movedX = m_X - oldX, movedY = m_Y - oldY;
        float traveled = std::sqrt(movedX * movedX + movedY * movedY);
        if (traveled > 0.0001f)
        {
            m_Moving = true;
            if (std::abs(movedX) > 0.0001f)
            {
                m_FaceLeft = movedX < 0;
            }
            // Preserve the 80-world-unit stride with all eight mini walk frames.
            m_Walk = std::fmod(m_Walk + traveled / 10.f, 8.f);
        }
        else
        {
            m_Walk = 0;
        }
    }
    else
    {
        m_Walk = 0;
    }
    m_Level.FollowPlayer({m_X, m_Y});
    m_Level.Update(dt, {m_X, m_Y}, m_Map, interactionHeld);
    const auto push = m_Level.ConsumePush();
    const int pushSteps = static_cast<int>(std::ceil(std::hypot(push.x, push.y) / 4.f));
    for (int i = 0; i < pushSteps; ++i)
    {
        const float nx = m_X + push.x / pushSteps;
        const float ny = m_Y + push.y / pushSteps;
        if (!Blocked(nx, ny))
        {
            m_X = nx;
            m_Y = ny;
        }
        else
        {
            break;
        }
    }
    WorldPosition destination;
    if (m_Level.ApplyPendingTravel(m_Map, destination))
    {
        m_X = m_CameraX = destination.x;
        m_Y = m_CameraY = destination.y;
        m_Attack.Reset();
        m_MageProjectiles.Reset();
        m_MageUltimate.Reset();
        m_KnightSkills.Reset();
        m_Scene.Clear();
    }
    if (m_Level.IsDefeated())
    {
        m_Attack.Reset();
        m_MageProjectiles.Reset();
        m_MageUltimate.Reset();
        m_KnightSkills.Reset();
        m_Moving = false;
    }
    if (m_Level.Complete() && !m_CompletionShown)
    {
        m_CompletionShown = true;
        m_JournalOpen = true;
    }

    Stream();
    float follow = 1.f - std::exp(-9.f * dt);
    m_CameraX += (m_X - m_CameraX) * follow;
    m_CameraY += (m_Y - m_CameraY) * follow;
}

Point PrototypeWorld::Project(float x, float y) const
{
    return SceneCamera(m_CameraX, m_CameraY, m_Width, m_Height).Project(x, y);
}

void PrototypeWorld::Draw(Renderer& r, int width, int height)
{
    m_Width = width;
    m_Height = height;
    r.Begin();
    int left = int(std::floor((m_CameraX - width * .5f) / 64.f)) - 1;
    int top =
        int(std::floor((m_CameraY - height / SceneCamera::GroundDepthScale() * .52f) / 64.f)) - 1;
    int cols = width / 64 + 4, rows = int(height / (64 * SceneCamera::GroundDepthScale())) + 5;
    for (int y = top; y < top + rows; ++y)
    {
        for (int x = left; x < left + cols; ++x)
        {
            Point p = Project(x * 64.f, y * 64.f);
            m_MapView.DrawTile(r, m_Map, x, y, p);
        }
    }
    Stream();
    m_Scene.BeginDynamicActors();
    if (m_Started)
    {
        m_Level.AddWorldActors(m_Scene, {m_X, m_Y});
        m_Scene.AddDynamicActor({m_X, m_Y, 5, 1});
        const auto& projectiles = m_MageProjectiles.Projectiles();
        for (std::size_t i = 0; i < projectiles.size(); ++i)
        {
            if (projectiles[i].Visible())
            {
                m_Scene.AddDynamicActor({projectiles[i].position.x,
                                         projectiles[i].position.y,
                                         11,
                                         1.f,
                                         static_cast<int>(i)});
            }
        }
    }
    if (m_Started)
    {
        const auto& shots = m_KnightSkills.Shots();
        for (std::size_t i = 0; i < shots.size(); ++i)
        {
            if (shots[i].Visible())
            {
                m_Scene.AddDynamicActor(
                    {shots[i].position.x, shots[i].position.y, 12, 1.f, static_cast<int>(i)});
            }
        }
    }
    m_Scene.BuildRenderQueue(SceneCamera(m_CameraX, m_CameraY, width, height));
    const SceneFrame frame{m_Map,
                           m_Level,
                           m_Attack,
                           m_MageProjectiles,
                           m_MageUltimate,
                           m_KnightSkills,
                           m_LevelView,
                           m_MapView,
                           m_Time,
                           m_Walk,
                           m_Class,
                           m_Moving,
                           m_FaceLeft,
                           m_Started,
                           Project(m_X, m_Y),
                           width,
                           height};
    const auto ultimateCenter = m_MageUltimate.Center();
    const Point ultimateScreen = Project(ultimateCenter.x, ultimateCenter.y);
    if (m_Started)
        MageUltimateView::DrawFloor(r, m_MageUltimate, ultimateScreen);
    m_SceneRenderer.Draw(r, m_Scene, frame);
    if (m_Started)
        MageUltimateView::DrawSky(r, m_MageUltimate, ultimateScreen);
    m_Level.CurrentChapter().DrawWeather(r, m_Time, width, height);
    if (m_Started)
    {
        CheondanView::PrepareScreen(r, m_KnightSkills, width, height);
    }
    r.FinishWorld(); // Post-process only the completed world; UI stays sharp and ungraded.
    if (m_Started)
    {
        CheondanView::DrawScreen(r, m_KnightSkills, width, height);
    }
    if (m_Started)
    {
        const auto* nearbyLandmark = m_Map.Layout().NearbyLandmark({m_X, m_Y});
        const auto localPlayer = m_Level.LocalPlayer({m_X, m_Y});
        const bool immediateAttack =
            std::any_of(m_Level.Enemies().Enemies().begin(),
                        m_Level.Enemies().Enemies().end(),
                        [&](const FieldEnemy& enemy)
                        {
                            return enemy.health > 0 && enemy.windup > 0.f &&
                                   std::hypot(enemy.position.x - localPlayer.x,
                                              enemy.position.y - localPlayer.y) < 300.f;
                        });
        const auto interactionPosition = m_Level.LandmarkInteractionPosition();
        const bool viewingInteraction =
            m_Level.HasLandmarkInteraction() &&
            std::hypot(interactionPosition.x - m_X, interactionPosition.y - m_Y) < 180.f &&
            (!nearbyLandmark ||
             std::hypot(nearbyLandmark->position.x - interactionPosition.x,
                        nearbyLandmark->position.y - interactionPosition.y) < 1.f);
        if (viewingInteraction && !immediateAttack)
        {
            const WorldPosition position = m_Level.LandmarkInteractionPosition();
            Point landmarkScreen = Project(position.x, position.y);
            if (landmarkScreen.x >= 0.f && landmarkScreen.x <= width && landmarkScreen.y >= 0.f &&
                landmarkScreen.y <= height)
            {
                landmarkScreen.y -= m_MapView.BubbleAnchorHeight(m_Level.LandmarkInteractionKind());
                m_InteractionBubbleView.DrawDescription(
                    r, m_Level.LandmarkInteractionText(), landmarkScreen, width, height);
            }
        }
        else if (nearbyLandmark && !immediateAttack)
        {
            Point landmarkScreen = Project(nearbyLandmark->position.x, nearbyLandmark->position.y);
            landmarkScreen.y -= m_MapView.BubbleAnchorHeight(nearbyLandmark->kind);
            m_InteractionBubbleView.DrawPrompt(
                r, m_Level.LandmarkPrompt(*nearbyLandmark), landmarkScreen, width, height);
        }
        m_LevelView.DrawHud(r, m_Level, m_Map.RegionName(m_X, m_Y), width, height);
        m_Skills.Draw(r, width, height, m_Level.Progress().Level());
        if (!MenuOpen() && !m_Level.IsDefeated())
        {
            if (const auto target = m_Level.TrackedGoal())
            {
                const auto p = Project(target->position.x, target->position.y);
                if (p.x > 20.f && p.x < width - 20.f && p.y > 135.f && p.y < height - 80.f)
                {
                    const float pulse = 22.f + 3.f * std::sin(m_Time * 3.f);
                    r.Ellipse(p.x, p.y, pulse, pulse * .45f, Color(1.f, .8f, .3f, .2f));
                    r.Triangle({p.x, p.y - 40.f},
                               {p.x - 6.f, p.y - 49.f},
                               {p.x + 6.f, p.y - 49.f},
                               Color(1.f, .8f, .3f));
                }
            }
            m_ExpeditionView.DrawGuide(r, m_Level, m_Map, {m_X, m_Y}, width);
        }
        if (m_MapView.IsOpen())
        {
            m_ExpeditionView.DrawMap(r, m_Level, m_Map, {m_X, m_Y}, width, height);
        }
        if (m_JournalOpen)
        {
            m_ExpeditionView.DrawJournal(r, m_Level, width, height);
        }
        if (m_Level.IsDefeated())
        {
            m_ExpeditionView.DrawDefeat(r, width, height);
        }
        if (m_Paused && !m_SettingsMenu.IsOpen())
        {
            const std::string pauseText = "일시정지 · [P] 계속 · [Esc] 시스템 메뉴";
            r.Text(width * .5f - r.TextWidth(pauseText) * .5f,
                   height * .5f,
                   pauseText,
                   Color(1.f, .85f, .56f));
        }
        m_SettingsMenu.Draw(
            r, width, height, m_Level.CanSkipChapter1(), m_Level.CanSkipChapter2FieldStory());
        r.Flush();
        return;
    }
    // Quiet letterbox and readable UI, independent of world coordinates.
    r.Rect(0, 0, float(width), 78, Color(.025f, .04f, .045f, .94f));
    r.Rect(0, 78, float(width), 1, Color(.48f, .42f, .27f, .65f));
    r.Rect(0, float(height - 46), float(width), 46, Color(.025f, .04f, .045f, .94f));
    r.Text(24, 28, "긴 전쟁  /  변경의 여정", Color(.82f, .76f, .60f));
    r.Text(24, 53, "수백 년의 전쟁으로 지친 나라.", Color(.52f, .60f, .59f));
    r.Text(24, 71, r.EffectStatus(), Color(.58f, .63f, .61f));
    if (!m_Started)
    {
        float x = width * .5f - 250, y = height * .5f - 165;
        r.Rect(x, y, 500, 330, Color(.04f, .06f, .065f, .97f));
        r.Rect(x, y, 500, 2, Color(.65f, .54f, .33f));
        const std::string selectText = "용병을 선택하세요";
        r.Text(x + (500.f - r.TextWidth(selectText)) * .5f,
               y + 39,
               selectText,
               Color(.91f, .83f, .65f));
        r.Character(0, x + 135, y + 180);
        r.Character(1, x + 365, y + 180);
        r.Text(x + 100, y + 213, "[1] 마법사", Color(.76f, .79f, .75f));
        r.Text(x + 330, y + 213, "[2] 기사", Color(.76f, .79f, .75f));
        r.Text(x + 90, y + 240, "룬 장갑", Color(.42f, .76f, .80f));
        r.Text(x + 325, y + 240, "검과 방패", Color(.69f, .72f, .69f));
        r.Text(
            x + 55, y + 278, "직업 선택 → 마을 조사 → 자유 탐험과 전투", Color(.57f, .64f, .61f));
        r.Text(x + 102, y + 309, "Ctrl을 눌러 1 → 2 → 3단 공격", Color(.48f, .54f, .53f));
    }
    r.Text(24,
           float(height - 18),
           "WASD·방향키 이동   Shift 달리기   Ctrl 공격   Space 확인   B 야영지 복귀",
           Color(.64f, .68f, .63f));
    r.Flush();
}

bool PrototypeWorld::MenuOpen() const
{
    return m_SettingsMenu.IsOpen() || m_JournalOpen || m_MapView.IsOpen() ||
           m_LevelView.InventoryOpen() || m_Skills.IsWindowOpen();
}

bool PrototypeWorld::CloseMenus()
{
    const bool open = MenuOpen();
    m_SettingsMenu.Close();
    m_JournalOpen = false;
    m_MapView.CloseOverview();
    m_LevelView.CloseInventory();
    m_Skills.CloseWindow();
    m_Skills.CancelDrag();
    return open;
}

void PrototypeWorld::ToggleSystemMenu()
{
    if (!m_Started)
    {
        return;
    }
    if (m_SettingsMenu.IsOpen())
    {
        m_SettingsMenu.Escape();
        return;
    }
    CloseMenus();
    m_Attack.Reset();
    m_Moving = false;
    m_SettingsMenu.Open();
}

bool PrototypeWorld::SystemMenuOpen() const
{
    return m_SettingsMenu.IsOpen();
}

void PrototypeWorld::SystemMenuMouseMove(int x, int y)
{
    m_SettingsMenu.MouseMove(x, y);
}

void PrototypeWorld::CancelMenuPress()
{
    m_SettingsMenu.CancelPress();
}

bool PrototypeWorld::SystemMenuMouseButton(
    bool down, int x, int y, int width, int height, PostSettings& settings)
{
    const auto action = m_SettingsMenu.MouseButton(down,
                                                   x,
                                                   y,
                                                   width,
                                                   height,
                                                   settings,
                                                   m_Level.CanSkipChapter1(),
                                                   m_Level.CanSkipChapter2FieldStory());
    if (action == SettingsMenu::Action::Quit)
    {
        return true;
    }
    if (action == SettingsMenu::Action::SkipChapter1 ||
        action == SettingsMenu::Action::SkipChapter2)
    {
        WorldPosition destination;
        const bool skipped = action == SettingsMenu::Action::SkipChapter1
                                 ? m_Level.SkipChapter1(destination)
                                 : m_Level.SkipChapter2FieldStory(destination);
        if (skipped)
        {
            m_X = m_CameraX = destination.x;
            m_Y = m_CameraY = destination.y;
            m_Attack.Reset();
            m_MageProjectiles.Reset();
            m_MageUltimate.Reset();
            m_KnightSkills.Reset();
            m_Scene.Clear();
            m_Moving = false;
            m_Walk = 0.f;
            Stream();
        }
    }
    return false;
}

void PrototypeWorld::ToggleJournal()
{
    if (!m_Started || m_Level.IsDefeated())
    {
        return;
    }
    const bool open = m_JournalOpen;
    CloseMenus();
    m_JournalOpen = !open;
}

void PrototypeWorld::ReturnToCamp()
{
    if (!m_Started || m_Paused || MenuOpen())
    {
        return;
    }
    WorldPosition destination;
    if (m_Level.ReturnToCamp({m_X, m_Y}, destination))
    {
        m_X = m_CameraX = destination.x;
        m_Y = m_CameraY = destination.y;
        m_Attack.Reset();
        m_MageProjectiles.Reset();
        m_MageUltimate.Reset();
        m_KnightSkills.Reset();
        m_Moving = false;
        Stream();
    }
}

void PrototypeWorld::RetryDefeat()
{
    if (m_Started && m_Level.IsDefeated())
    {
        CloseMenus();
        ReturnToCamp();
    }
}
