#include "stdafx.h"
#include "Chapter.h"
#include "Renderer.h"
#include <utility>

ChapterFlags Chapter::ExportFlags() const
{
    return {};
}

void Chapter::StrikeObjects(WorldPosition, bool, int, const LevelMap&)
{
}

bool Chapter::EnemyVisible(const FieldEnemy& enemy) const
{
    return enemy.health > 0;
}

void Chapter::AddActors(SceneGraph&, WorldPosition) const
{
}

bool Chapter::DrawLandmark(Renderer&, const MapLandmark&, Point, float) const
{
    return false;
}

void Chapter::DrawActor(Renderer&, int, Point, float) const
{
}

void Chapter::DrawWeather(Renderer&, float, int, int) const
{
}

const std::optional<ChapterTravel>& Chapter::PendingTravel() const
{
    return m_Travel;
}

void Chapter::ClearTravel() noexcept
{
    m_Travel.reset();
}

void Chapter::RequestTravel(ChapterTravel travel)
{
    m_Travel = std::move(travel);
}
