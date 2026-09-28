#pragma once

class GameSession;

namespace ChapterCatalog
{
    inline constexpr const char* InitialChapter = "chapter1";
    void Register(GameSession& session);
    void RegisterDevelopment(GameSession& session);
}
