#include "stdafx.h"
#include "ChapterCatalog.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include "Chapter3.h"
#include "GameSession.h"
#include <memory>

void ChapterCatalog::Register(GameSession& session)
{
    session.RegisterChapter(InitialChapter,
                            []
                            {
                                return std::make_unique<Chapter1>();
                            });
    session.RegisterChapter("chapter2",
                            []
                            {
                                return std::make_unique<Chapter2>();
                            });
    session.RegisterChapter("chapter3",
                            []
                            {
                                return std::make_unique<Chapter3>();
                            });
}

void ChapterCatalog::RegisterDevelopment(GameSession&)
{
    // Chapter 2 now uses the normal catalog. Keep the preview CLI entry compatible.
}
