#pragma once

#include "EnemyPathfinder.h"
#include "EnemyAIState.h"

struct FieldEnemy;

class EnemyStateMachine
{
  public:
    int Update(FieldEnemy& enemy,
               std::vector<FieldEnemy>& enemies,
               float dt,
               WorldPosition player,
               const LevelMap& map,
               bool supportEnabled,
               int& searchBudget) const;
    void Interrupt(FieldEnemy& enemy) const;

  private:
    void Transition(FieldEnemy& enemy, EnemyState state) const;
    void Chase(FieldEnemy& enemy,
               float dt,
               WorldPosition player,
               const LevelMap& map,
               int& searchBudget) const;
    EnemyPathfinder m_Pathfinder;
};
