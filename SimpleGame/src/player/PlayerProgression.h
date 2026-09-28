#pragma once

class PlayerProgression
{
  public:
    void Reset(int profession);
    void DebugSetLevel(int level);
    int AddExperience(int amount);
    void Heal(int amount);
    void Damage(int amount);
    int RequiredExperience() const;
    int Level() const;
    int Experience() const;
    int Health() const;
    int MaxHealth() const;
    int MaxMana() const;
    int Mana() const;
    bool SpendMana(int amount);
    int Strength() const;
    int Intelligence() const;
    int Attack() const;

  private:
    int m_Profession = 0;
    int m_Level = 1;
    int m_Experience = 0;
    int m_Health = 100;
    int m_Mana = 0;
};
