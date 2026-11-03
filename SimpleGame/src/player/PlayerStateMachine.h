#pragma once

// Player action states form a hierarchy: Active -> Locomotion/Action -> leaf.
// Control restrictions are an independent gate so a paused cast remains visible.
class PlayerStateMachine
{
  public:
    enum class State
    {
        Idle,
        Moving,
        BasicAttack,
        MageProjectile,
        MageUltimate,
        KnightSkill,
        Defeated
    };

    enum class Branch
    {
        Active,
        Locomotion,
        Action,
        Casting,
        Defeated
    };

    struct Inputs
    {
        bool defeated = false;
        bool controlLocked = false;
        bool moving = false;
        bool attacking = false;
        bool mageProjectile = false;
        bool mageUltimate = false;
        bool knightSkill = false;
    };

    void Update(const Inputs& inputs)
    {
        m_ControlLocked = inputs.controlLocked;
        if (inputs.defeated)
        {
            m_State = State::Defeated;
        }
        else if (inputs.mageUltimate)
        {
            m_State = State::MageUltimate;
        }
        else if (inputs.knightSkill)
        {
            m_State = State::KnightSkill;
        }
        else if (inputs.mageProjectile)
        {
            m_State = State::MageProjectile;
        }
        else if (inputs.attacking)
        {
            m_State = State::BasicAttack;
        }
        else
        {
            m_State = inputs.moving ? State::Moving : State::Idle;
        }
    }

    State Current() const
    {
        return m_State;
    }

    bool IsIn(Branch branch) const
    {
        switch (branch)
        {
        case Branch::Active:
            return m_State != State::Defeated;
        case Branch::Locomotion:
            return m_State == State::Idle || m_State == State::Moving;
        case Branch::Action:
            return m_State == State::BasicAttack || IsIn(Branch::Casting);
        case Branch::Casting:
            return m_State == State::MageProjectile || m_State == State::MageUltimate ||
                   m_State == State::KnightSkill;
        case Branch::Defeated:
            return m_State == State::Defeated;
        }
        return false;
    }

    bool CanUseSkill() const
    {
        return !m_ControlLocked && IsIn(Branch::Locomotion);
    }

    bool CanMove() const
    {
        return !m_ControlLocked && IsIn(Branch::Locomotion);
    }

    bool CanAttack() const
    {
        return !m_ControlLocked && IsIn(Branch::Active) && !IsIn(Branch::Casting);
    }

  private:
    State m_State = State::Idle;
    bool m_ControlLocked = false;
};
