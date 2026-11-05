#include "stdafx.h"
#include "SaveArchive.h"
#include "TradeSystem.h"
#include <sstream>
#include <stdexcept>

namespace
{
    void Check(bool value, const char* message)
    {
        if (!value)
        {
            throw std::runtime_error(message);
        }
    }
}

// Called by the existing C01 verification path; never touches the user's save slot.
void VerifyPersistenceAndTrading()
{
    for (int profession = 0; profession < 2; ++profession)
    {
        LevelMap map;
        map.Generate(743);
        GameSession original;
        original.Reset(profession, 743, map);
        original.OpenWorld(map);
        WorldPosition destination;
        Check(original.SkipChapter1(destination) && original.SkipChapter2(destination) &&
                  original.SkipChapter3(destination),
              "Story save fixture failed");
        original.Progress().DebugSetLevel(20);
        original.Progress().Damage(27);
        original.Progress().SpendMana(13);
        original.Progress().RecoverMana(.75);
        auto& inventory = original.Inventory();
        inventory.AddGold(500);
        inventory.AddEquipment(profession, 3);
        inventory.AddEquipment(profession, 7);
        Check(inventory.Equip(0), "Explicit equipment selection failed");
        Check(inventory.BuyPotion(false, 20) && inventory.BuyPotion(true, 20),
              "Potion purchase failed");
        const int gold = inventory.Gold();
        Check(!inventory.BuyPotion(false, gold + 1) && inventory.Gold() == gold &&
                  inventory.HealthPotions() == 1,
              "Failed purchase changed stock or gold");
        SkillSystem skills;
        skills.Reset(profession);
        std::stringstream bytes(std::ios::in | std::ios::out | std::ios::binary);
        SaveArchive writer(bytes, true);
        writer(original, skills);

        LevelMap restoredMap;
        restoredMap.Generate(743);
        GameSession restored;
        restored.Reset(profession, 743, restoredMap);
        restored.OpenWorld(restoredMap);
        SkillSystem restoredSkills;
        restoredSkills.Reset(profession);
        bytes.seekg(0);
        SaveArchive reader(bytes, false);
        reader(restored, restoredSkills);
        Check(restored.Complete() && restored.ActiveChapterId() == original.ActiveChapterId() &&
                  restored.Progress().Profession() == profession &&
                  restored.Progress().Level() == 20 &&
                  restored.Progress().Mana() == original.Progress().Mana() &&
                  restored.Progress().Health() == original.Progress().Health(),
              "Progress did not survive archive");
        Check(restored.Inventory().Gold() == gold && restored.Inventory().Items().size() == 2 &&
                  restored.Inventory().EquippedIndex() == 0 &&
                  restored.Inventory().HealthPotions() == 1 &&
                  restored.Inventory().ManaPotions() == 1,
              "Inventory did not survive archive");
        Check(!restored.SkipChapter3(destination), "Loading repeated story completion");
        Check(restored.Inventory().SellEquipment(0, 15) &&
                  restored.Inventory().EquippedIndex() == -1 &&
                  restored.Inventory().Gold() == gold + 15,
              "Selling equipped item corrupted selection");
        Check(restored.Inventory().Equip(0) && restored.Inventory().AttackBonus() == 7,
              "Selling did not preserve remaining equipment indices");
        Check(restored.Inventory().UsePotion(true) && !restored.Inventory().UsePotion(true),
              "A potion was consumed more than once");
        auto truncated = bytes.str();
        truncated.resize(truncated.size() / 2);
        std::stringstream damaged(truncated, std::ios::in | std::ios::out | std::ios::binary);
        SaveArchive badReader(damaged, false);
        bool rejected = false;
        try
        {
            badReader(restored, restoredSkills);
        }
        catch (const std::exception&)
        {
            rejected = true;
        }
        Check(rejected, "Truncated archive was accepted");
    }
}
