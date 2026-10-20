#include "stdafx.h"
#include "GameSession.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include "Chapter3.h"

bool GameSession::CanSkipChapter1() const
{
    const auto* chapter = dynamic_cast<const Chapter1*>(FindChapter("chapter1"));
    return m_OpenWorld && !m_StartAtChapter2 && !m_StartAtChapter3 && chapter &&
           !chapter->Complete();
}

bool GameSession::CanSkipChapter2FieldStory() const
{
    const auto* first = FindChapter("chapter1");
    const auto* second = dynamic_cast<const Chapter2*>(FindChapter("chapter2"));
    return m_OpenWorld && !m_StartAtChapter3 && first && (first->Complete() || m_StartAtChapter2) &&
           second && second->CanSkipFieldStory();
}

bool GameSession::SkipChapter1(WorldPosition& destination)
{
    return SkipStory(StorySkip::Chapter1, destination);
}

bool GameSession::SkipChapter2FieldStory(WorldPosition& destination)
{
    return SkipStory(StorySkip::Chapter2Field, destination);
}

bool GameSession::CanSkipChapter2() const
{
    const auto* first = FindChapter("chapter1");
    const auto* second = FindChapter("chapter2");
    return m_OpenWorld && !m_StartAtChapter3 && first && (first->Complete() || m_StartAtChapter2) &&
           second && !second->Complete();
}

bool GameSession::CanSkipChapter3() const
{
    const auto* second = FindChapter("chapter2");
    const auto* third = FindChapter("chapter3");
    return m_OpenWorld && second && (second->Complete() || m_StartAtChapter3) && third &&
           !third->Complete();
}

bool GameSession::SkipChapter2(WorldPosition& destination)
{
    return SkipStory(StorySkip::Chapter2, destination);
}

bool GameSession::SkipChapter3(WorldPosition& destination)
{
    return SkipStory(StorySkip::Chapter3, destination);
}

bool GameSession::SkipStory(StorySkip skip, WorldPosition& destination)
{
    const bool chapter1 = skip == StorySkip::Chapter1;
    const bool fieldOnly = skip == StorySkip::Chapter2Field;
    const bool chapter3 = skip == StorySkip::Chapter3;
    const bool allowed = chapter1    ? CanSkipChapter1()
                         : fieldOnly ? CanSkipChapter2FieldStory()
                         : chapter3  ? CanSkipChapter3()
                                     : CanSkipChapter2();
    if (!allowed)
    {
        ShowNotice("이미 완료했거나 스킵에 필요한 선행 조건이 충족되지 않았습니다.");
        return false;
    }
    const auto target = chapter1    ? m_Atlas.Find("chapter2", Chapter2Maps::Graywall)
                        : fieldOnly ? m_Atlas.Find("chapter2", Chapter2Maps::OldFortress)
                        : chapter3  ? m_Atlas.Find("chapter3", Chapter3Maps::Argen)
                                    : m_Atlas.Find("chapter3", Chapter3Maps::Memorial);
    if (!target)
    {
        return false;
    }
    const auto entry = m_Atlas.Regions()[*target].local.entry;
    if (m_Regions[*target].map.Blocked(entry.x, entry.y))
    {
        ShowNotice("스킵 목적지에 진입할 수 없습니다.");
        return false;
    }
    if (chapter1)
    {
        dynamic_cast<Chapter1&>(*m_Chapters.at("chapter1").instance).SkipStory();
    }
    else if (fieldOnly)
    {
        dynamic_cast<Chapter2&>(*m_Chapters.at("chapter2").instance).SkipFieldStory();
    }
    else if (chapter3)
    {
        auto& third = dynamic_cast<Chapter3&>(*m_Chapters.at("chapter3").instance);
        auto flags = FindChapter("chapter2")->ExportFlags();
        if (m_StartAtChapter3)
        {
            flags = {{"C02_COMPLETE", 1}, {"C02_SUPPLY_LOSS", 1}, {"C02_REFUGEE_LOSS", 1}};
        }
        third.ImportPrevious(flags);
        if (!third.SkipStory())
        {
            return false;
        }
    }
    else
    {
        dynamic_cast<Chapter2&>(*m_Chapters.at("chapter2").instance).SkipStory();
    }
    // Retire authored encounters without kill rewards; ambient field populations remain.
    for (std::size_t i = 0; i < m_Regions.size(); ++i)
    {
        const auto& region = m_Atlas.Regions()[i];
        if (region.chapter != (chapter1   ? "chapter1"
                               : chapter3 ? "chapter3"
                                          : "chapter2") ||
            (fieldOnly && region.local.id == Chapter2Maps::OldFortress))
        {
            continue;
        }
        auto& enemies = i == m_Region ? m_Enemies : m_Regions[i].enemies;
        if (chapter1)
        {
            for (std::size_t id = static_cast<std::size_t>(Chapter1Encounter::FarmRaid);
                 id <= static_cast<std::size_t>(Chapter1Encounter::HeroPayoff);
                 ++id)
            {
                enemies.RemoveEncounter(id);
            }
        }
        else if (chapter3)
        {
            enemies.RemoveEncounter(301);
        }
        else
        {
            for (const std::size_t id : {201u, 202u, 203u, 210u, 211u, 212u})
            {
                enemies.RemoveEncounter(id);
            }
            if (!fieldOnly)
            {
                for (const std::size_t id : {220u, 221u, 229u, 230u, 231u})
                {
                    enemies.RemoveEncounter(id);
                }
            }
        }
    }
    ActivateRegion(*target);
    SynchronizeChapterHistory();
    m_Chapter->ClearTravel();
    m_Chapter->PlayerArrived(entry, m_Regions[*target].map);
    destination = m_Atlas.ToWorld(*target, entry);
    m_Checkpoint = destination;
    m_Push = {};
    m_LandmarkInteractionTime = 0.f;
    m_LandmarkInteractionText.clear();
    m_Progress.Restore();
    m_Invulnerable = 1.f;
    ShowNotice(chapter1    ? "챕터 1 스킵 완료 · 회색벽 작전실에서 브리핑을 확인하세요."
               : fieldOnly ? "회색벽·협곡 스킵 완료 · 성채 진입로를 선택하세요."
               : chapter3  ? "챕터 3 스킵 완료 · 아르겐 귀환. 다음 단서: 야수족 영역의 원정 좌표."
                           : "챕터 2 스킵 완료 · 왕립 기록관 기념홀에서 다음 이야기를 시작하세요.");
    return true;
}
