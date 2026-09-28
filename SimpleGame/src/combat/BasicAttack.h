#pragma once

#include <array>
#include <vector>

// Owns combo timing and buffered presses, independently of input and rendering.
class BasicAttack
{
  public:
    BasicAttack();

    void Reset();
    void Press(int profession);
    void Update(float seconds);
    bool IsAttacking() const;
    const char* ClipId() const;
    float ElapsedMilliseconds() const;
    int Step() const;
    std::vector<int> ConsumeHits();

  private:
    struct AttackClip
    {
        const char* id = nullptr;
        float duration = 0.f;
        float hitTime = 0.f;
    };

    void Begin(int step);
    const AttackClip& CurrentClip() const;

    // Implementation assumption: accept another press for 250 ms after a strike.
    const float m_ComboGraceMilliseconds = 250.f;
    std::array<std::array<AttackClip, 3>, 2> m_Clips{};
    int m_Profession = 0;
    int m_Step = 0;
    int m_BufferedPresses = 0;
    float m_ElapsedMilliseconds = 0.f;
    bool m_HitEmitted = false;
    std::vector<int> m_PendingHits;
};
