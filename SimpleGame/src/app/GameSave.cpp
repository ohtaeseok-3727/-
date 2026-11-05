#include "stdafx.h"
#include "GameSave.h"
#include "PrototypeWorld.h"
#include "SaveArchive.h"
#include <cstdlib>
#include <fstream>
#include <memory>
#include <Windows.h>

namespace
{
    constexpr std::uint32_t Version = 1;
    constexpr std::uint32_t MaxBytes = 32 * 1024 * 1024;

    std::uint32_t Checksum(const std::string& bytes)
    {
        std::uint32_t hash = 2166136261u;
        for (unsigned char byte : bytes)
        {
            hash = (hash ^ byte) * 16777619u;
        }
        return hash;
    }
}

std::filesystem::path GameSave::Path()
{
    wchar_t* directory = nullptr;
    std::size_t length = 0;
    const auto result = _wdupenv_s(&directory, &length, L"LOCALAPPDATA");
    std::unique_ptr<wchar_t, decltype(&std::free)> local(directory, &std::free);
    if (result != 0 || !local || length <= 1)
    {
        throw std::runtime_error("Local application data directory is unavailable");
    }
    return std::filesystem::path(local.get()) / L"SimpleGame" / L"Saves" / L"journey-v1.sav";
}

bool GameSave::Exists()
{
    try
    {
        return std::filesystem::is_regular_file(Path());
    }
    catch (const std::exception&)
    {
        return false;
    }
}

bool GameSave::Save(PrototypeWorld& w, std::string& message)
{
    if (!w.m_Started || !w.m_Level.WorldEnabled() || w.m_Level.IsDefeated() ||
        w.m_Level.CurrentChapter().ControlLocked() || w.m_Attack.IsAttacking() ||
        w.m_MageProjectiles.Casting() || !w.m_MageProjectiles.Projectiles().empty() ||
        w.m_MageUltimate.Active() || w.m_KnightSkills.Casting() ||
        !w.m_KnightSkills.Shots().empty())
    {
        message = "공격·스킬·이야기 연출이 끝난 뒤 살아 있는 상태에서 저장하세요.";
        return false;
    }
    try
    {
        std::stringstream payload(std::ios::in | std::ios::out | std::ios::binary);
        SaveArchive archive(payload, true);
        archive(w.m_Class,
                w.m_X,
                w.m_Y,
                w.m_Time,
                w.m_FaceLeft,
                w.m_CompletionShown,
                w.m_Level,
                w.m_Skills);
        std::string bytes = payload.str();
        if (bytes.size() > MaxBytes)
        {
            throw std::runtime_error("Save exceeds size limit");
        }
        std::string magic = "SimpleGameJourney";
        auto version = Version;
        auto seed = w.m_Map.Seed();
        auto size = static_cast<std::uint32_t>(bytes.size());
        auto checksum = Checksum(bytes);
        auto wordSize = static_cast<std::uint32_t>(sizeof(std::size_t));
        const auto path = Path();
        const auto temporary = std::filesystem::path(path.wstring() + L".tmp");
        std::filesystem::create_directories(path.parent_path());
        {
            std::fstream output(temporary, std::ios::out | std::ios::binary | std::ios::trunc);
            SaveArchive header(output, true);
            header(magic, version, wordSize, seed, size, checksum);
            output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
            output.flush();
            if (!output)
            {
                throw std::runtime_error("Save write failed");
            }
        }
        if (std::filesystem::exists(path))
        {
            std::filesystem::copy_file(path,
                                       std::filesystem::path(path.wstring() + L".bak"),
                                       std::filesystem::copy_options::overwrite_existing);
        }
        if (!MoveFileExW(temporary.c_str(),
                         path.c_str(),
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        {
            throw std::runtime_error("Save replacement failed");
        }
        message = "여정을 저장했습니다. 시작 화면의 이어하기 또는 Esc 메뉴에서 불러올 수 있습니다.";
        return true;
    }
    catch (const std::exception&)
    {
        message = "저장하지 못했습니다. 저장 폴더의 쓰기 권한과 디스크 여유를 확인하세요.";
        return false;
    }
}

bool GameSave::Load(PrototypeWorld& w, std::string& message)
{
    try
    {
        const auto path = Path();
        if (!std::filesystem::exists(path))
        {
            message = "저장된 여정이 없습니다. 게임 중 Esc → 여정 저장을 사용하세요.";
            return false;
        }
        if (std::filesystem::file_size(path) > MaxBytes + 256)
        {
            throw std::runtime_error("Save too large");
        }
        std::fstream input(path, std::ios::in | std::ios::binary);
        SaveArchive header(input, false);
        std::string magic;
        std::uint32_t version = 0, wordSize = 0, seed = 0, size = 0, checksum = 0;
        header(magic, version, wordSize, seed, size, checksum);
        if (magic != "SimpleGameJourney" || version != Version || wordSize != sizeof(std::size_t) ||
            size > MaxBytes)
        {
            throw std::runtime_error("Incompatible save");
        }
        std::string bytes(size, '\0');
        input.read(bytes.data(), size);
        if (!input || input.peek() != std::char_traits<char>::eof() || Checksum(bytes) != checksum)
        {
            throw std::runtime_error("Damaged save");
        }
        auto candidate = std::make_unique<PrototypeWorld>();
        candidate->m_Map.Generate(seed);
        candidate->m_MapGenerated = true;
        candidate->m_Level.Reset(0, seed, candidate->m_Map);
        candidate->m_Level.OpenWorld(candidate->m_Map);
        std::stringstream payload(bytes, std::ios::in | std::ios::out | std::ios::binary);
        SaveArchive archive(payload, false);
        archive(candidate->m_Class,
                candidate->m_X,
                candidate->m_Y,
                candidate->m_Time,
                candidate->m_FaceLeft,
                candidate->m_CompletionShown);
        if (candidate->m_Class < 0 || candidate->m_Class > 1)
        {
            throw std::runtime_error("Invalid saved profession");
        }
        candidate->m_Skills.Reset(candidate->m_Class);
        archive(candidate->m_Level, candidate->m_Skills);
        if (payload.peek() != std::char_traits<char>::eof() ||
            candidate->m_Class != candidate->m_Level.Progress().Profession() ||
            std::abs(candidate->m_X) > 1000000.f || std::abs(candidate->m_Y) > 1000000.f ||
            std::abs(candidate->m_Level.Checkpoint().x) > 1000000.f ||
            std::abs(candidate->m_Level.Checkpoint().y) > 1000000.f ||
            candidate->m_Level.Progress().Health() <= 0 ||
            candidate->m_Map.Blocked(candidate->m_X, candidate->m_Y) ||
            candidate->m_Map.Blocked(candidate->m_Level.Checkpoint().x,
                                     candidate->m_Level.Checkpoint().y))
        {
            throw std::runtime_error("Invalid saved position");
        }
        for (int i = 0; i < 8; ++i)
        {
            const auto& id = candidate->m_Skills.QuickSlots().At(i);
            const auto* skill = candidate->m_Skills.PlayerSkillsState().Find(id);
            if (!id.empty() && (!skill || skill->kind != SkillKind::Active ||
                                skill->requiredLevel > candidate->m_Level.Progress().Level()))
            {
                throw std::runtime_error("Invalid saved quick slot");
            }
        }
        for (const auto& cooldown : candidate->m_Skills.Cooldowns())
        {
            const auto* skill = candidate->m_Skills.PlayerSkillsState().Find(cooldown.first);
            if (!skill || cooldown.second < 0.f || cooldown.second > skill->cooldown)
            {
                throw std::runtime_error("Invalid saved skill cooldown");
            }
        }
        candidate->m_Width = w.m_Width;
        candidate->m_Height = w.m_Height;
        candidate->m_Scene.Stream(
            candidate->m_Map, candidate->m_X, candidate->m_Y, w.m_Width, w.m_Height);
        std::string loadedMessage = "저장한 여정을 불러왔습니다.";
        // All file reads and validation have completed. GPU objects stay with the live world.
        w.m_Class = candidate->m_Class;
        w.m_X = w.m_CameraX = candidate->m_X;
        w.m_Y = w.m_CameraY = candidate->m_Y;
        w.m_Time = candidate->m_Time;
        w.m_FaceLeft = candidate->m_FaceLeft;
        w.m_CompletionShown = candidate->m_CompletionShown;
        w.m_Map = std::move(candidate->m_Map);
        w.m_Level = std::move(candidate->m_Level);
        w.m_Skills = std::move(candidate->m_Skills);
        w.m_Level.RebindWorldGeometry(w.m_Map);
        w.m_MapGenerated = w.m_Started = true;
        w.m_Paused = w.m_Moving = false;
        w.m_Walk = 0.f;
        w.m_PreviewChapter.clear();
        w.CloseMenus();
        w.m_Attack.Reset();
        w.m_MageProjectiles.Reset();
        w.m_MageUltimate.Reset();
        w.m_KnightSkills.Reset();
        w.m_Scene.Swap(candidate->m_Scene);
        message.swap(loadedMessage);
        return true;
    }
    catch (const std::exception&)
    {
        message =
            "저장 파일이 손상되었거나 호환되지 않아 불러오지 않았습니다. 현재 여정은 유지됩니다.";
        return false;
    }
}
