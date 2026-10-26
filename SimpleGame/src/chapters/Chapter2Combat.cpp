#include "stdafx.h"
#include "Chapter2.h"
#include "SceneCamera.h"
#include "MageProjectiles.h"
#include <algorithm>
#include <cmath>

namespace
{
    // Tuning assumptions, not newly confirmed game-design values.
    constexpr int OrcHealth = LevelTuning::OrcHealth, EliteHealth = 140, BarkanHealth = 900;
    constexpr float JumpDistance = 210.f, JumpCooldown = 6.f, CryCooldown = 11.f;
    constexpr float CryRadius = 185.f, CryBuffSeconds = 6.f, CrySpeed = 1.65f;

    float Distance(WorldPosition a, WorldPosition b)
    {
        return std::hypot(a.x - b.x, a.y - b.y);
    }

    WorldPosition Toward(WorldPosition from, WorldPosition to, float distance)
    {
        const float length = Distance(from, to);
        const float factor = length > .001f ? (std::min)(1.f, distance / length) : 0.f;
        return {from.x + (to.x - from.x) * factor, from.y + (to.y - from.y) * factor};
    }

    WorldPosition TargetPosition(Chapter2Target target)
    {
        switch (target)
        {
        case Chapter2Target::Beacon:
            return {0, -1250};
        case Chapter2Target::Workshop:
            return {-1100, -350};
        default:
            return {1100, -350};
        }
    }
}

WorldPosition Chapter2::DevicePosition(Chapter2Target target, int step)
{
    auto position = TargetPosition(target);
    position.x += step == 0 ? 0.f : step == 1 ? -120.f : 120.f;
    return position;
}

int Chapter2::TargetSteps(Chapter2Target target) const
{
    return 3 - static_cast<int>(StageLength(target));
}

const Chapter2Combat& Chapter2::Combat() const
{
    return m_Combat;
}

ChapterImpact Chapter2::ConsumeImpact()
{
    const auto result = m_Impact;
    m_Impact = {};
    return result;
}

WorldPosition Chapter2::SafeSpawn(WorldPosition center,
                                  float dx,
                                  float dy,
                                  const LevelMap& map) const
{
    const WorldPosition candidate{center.x + dx, center.y + dy};
    if (!map.Blocked(candidate.x, candidate.y) && map.ClearPath(center, candidate))
    {
        return candidate;
    }
    return center;
}

void Chapter2::SpawnOrcs(EnemySystem& enemies,
                         const LevelMap& map,
                         WorldPosition center,
                         int group,
                         int count,
                         int elites)
{
    for (int i = 0; i < count; ++i)
    {
        EnemyStats stats;
        const bool elite = i < elites;
        stats.health = elite ? EliteHealth : OrcHealth;
        stats.damage = elite ? 13 : 8;
        stats.speed = elite ? 78.f : 87.f;
        stats.detectionRange = 200.f;
        stats.appearance = elite ? 1 : 0;
        stats.preserveCorpse = true;
        if (group == 229)
        {
            // The opposing line holds its position; disengaging leaves survivors.
            stats.scriptedAI = true;
            stats.minimumHealth = 1;
        }
        const auto position = SafeSpawn(center, (i % 3 - 1) * 65.f, (i / 3) * 65.f + 65.f, map);
        enemies.Add(position, static_cast<std::size_t>(group), EnemyRole::Melee, stats);
    }
}

void Chapter2::BeginEncounter(int group, WorldPosition center, int waves)
{
    m_Combat.objectiveStep = 0;
    m_Combat.defenseTime = 0.f;
    m_Combat.group = group;
    m_Combat.center = center;
    m_Combat.wave = 0;
    m_Combat.waves = waves;
    m_Combat.spawned = false;
    m_Combat.cleared = false;
    m_Combat.interaction = 0.f;
}

void Chapter2::UpdateEncounter(WorldPosition player, const LevelMap& map, EnemySystem& enemies)
{
    if (!m_Combat.group || m_Combat.cleared)
    {
        return;
    }
    if (!m_Combat.spawned)
    {
        if (Distance(player, m_Combat.center) > 500.f)
        {
            return;
        }
        const int count = m_Combat.waves == 3 ? 4 : m_Combat.waves == 2 ? 3 : 2;
        const int elites = m_State.map == Chapter2Maps::Graywall ? 0 : 1;
        SpawnOrcs(enemies, map, m_Combat.center, m_Combat.group, count, elites);
        ++m_Combat.wave;
        m_Combat.spawned = true;
        m_Message =
            "오크 경계 " + std::to_string(m_Combat.wave) + "/" + std::to_string(m_Combat.waves);
    }
    else if (!enemies.Alive(static_cast<std::size_t>(m_Combat.group)))
    {
        if (m_Combat.wave < m_Combat.waves)
        {
            m_Combat.spawned = false;
        }
        else
        {
            m_Combat.cleared = true;
            m_Message = "경계를 격파했습니다. 주변 단서와 장치를 확인하세요.";
        }
    }
}

bool Chapter2::HoldDevice(float dt,
                          bool held,
                          WorldPosition player,
                          WorldPosition device,
                          const LevelMap& map,
                          float duration)
{
    if (!held || Distance(player, device) > 90.f || !map.ClearPath(player, device))
    {
        m_Combat.interaction = 0.f;
        return false;
    }
    m_Combat.interaction += dt;
    if (m_Combat.interaction < duration)
    {
        m_Message = "장치 조작 중 · Space 유지 " +
                    std::to_string(static_cast<int>(100.f * m_Combat.interaction / duration)) + "%";
        return false;
    }
    m_Combat.interaction = 0.f;
    return true;
}

void Chapter2::Update(float dt,
                      WorldPosition player,
                      bool held,
                      const LevelMap& map,
                      EnemySystem& enemies,
                      PlayerProgression&)
{
    if (dt <= 0.f)
    {
        return;
    }
    dt = (std::min)(dt, .1f);
    m_Impact = {};
    m_Combat.heroMoving = false;
    if (ContinuousWorld() && m_State.map == Chapter2Maps::Canyon &&
        m_State.phase == Chapter2Phase::Priorities && !m_State.activeTarget)
    {
        for (int index = 0; index < 3; ++index)
        {
            const auto target = static_cast<Chapter2Target>(index);
            if (!m_State.resolved.test(index) &&
                Distance(player, DevicePosition(target, 0)) < 420.f)
            {
                ChooseTarget(target);
                m_Message =
                    std::string(TargetName(target)) + "에 접근했습니다. 이 지역 대응을 시작합니다.";
                break;
            }
        }
    }
    if (!m_Combat.heroPresent)
    {
        m_Combat.heroPresent = true;
        m_Combat.hero =
            m_State.map == Chapter2Maps::OldFortress && m_State.phase == Chapter2Phase::Reunion
                ? WorldPosition{-500, -2100}
                : SafeSpawn(player, -55.f, 20.f, map);
    }
    if (m_State.map == Chapter2Maps::Graywall)
    {
        if (m_State.phase == Chapter2Phase::Reconnaissance &&
            !Has(Chapter2Event::ScheduleRecovered) && m_Combat.group != 201)
        {
            BeginEncounter(201, {550, -950}, 1);
        }
        if (m_State.phase == Chapter2Phase::Frontline)
        {
            if (!Has(Chapter2Event::FrontlineSupported) && m_Combat.group != 202)
            {
                BeginEncounter(202, {-180, -1550}, 2);
            }
            if (Has(Chapter2Event::FrontlineSupported) &&
                !Has(Chapter2Event::BeaconDirectionFound) && m_Combat.group != 203)
            {
                BeginEncounter(203, {730, -1800}, 1);
            }
            if (m_Combat.group == 202 && m_Combat.cleared)
            {
                RecordEvent(Chapter2Event::FrontlineSupported);
                BeginEncounter(203, {730, -1800}, 1);
                m_Message =
                    "용사: 지금 추격하면 끝낼 수 있어. 플레이어: 측면의 봉화를 먼저 확인하자.";
            }
        }
    }
    else if (m_State.map == Chapter2Maps::Canyon && m_State.activeTarget)
    {
        const auto target = *m_State.activeTarget;
        const int group = 210 + static_cast<int>(target);
        const auto center = TargetPosition(target);
        if (m_Combat.group != group)
        {
            BeginEncounter(group, center, 1);
        }
        const auto device = DevicePosition(target, m_Combat.objectiveStep);
        bool completed = false;
        if (target == Chapter2Target::Camp && m_Combat.objectiveStep > 0)
        {
            // Rescue needs a protected evacuation point, not another device hold.
            if (Distance(player, device) < 150.f && !enemies.Alive(group))
            {
                m_Combat.defenseTime += dt;
            }
            m_Message = "피난민 대피 지점 방어 · " +
                        std::to_string(static_cast<int>(m_Combat.defenseTime)) + "/8초";
            completed = m_Combat.defenseTime >= 8.f;
        }
        else if (m_Combat.cleared)
        {
            completed = HoldDevice(
                dt, held, player, device, map, target == Chapter2Target::Workshop ? 4.f : 2.f);
        }
        if (completed)
        {
            ++m_Combat.objectiveStep;
            m_Combat.defenseTime = 0.f;
            if (m_Combat.objectiveStep >= TargetSteps(target))
            {
                ResolveTarget(target);
                m_Message = std::string(TargetName(target)) + " 대응 완료. 다음 목표를 선택하세요.";
            }
            else if (target == Chapter2Target::Camp)
            {
                SpawnOrcs(enemies, map, center, group, 2, 0);
            }
            else
            {
                m_Message = target == Chapter2Target::Beacon
                                ? "중계 신호 차단. 표시된 다음 신호 장치를 해제하세요."
                                : "폭약 해체. 표시된 지지대와 남은 폭약을 확보하세요.";
            }
        }
    }
    else if (m_State.map == Chapter2Maps::OldFortress)
    {
        if (m_State.phase == Chapter2Phase::Infiltration && m_State.route && m_Combat.group != 220)
        {
            BeginEncounter(220, {0, -1250}, 1);
        }
        if (m_State.phase == Chapter2Phase::Infiltration && m_Combat.group == 220 &&
            m_Combat.cleared && HoldDevice(dt, held, player, {0, -1250}, map, 10.f))
        {
            RecordEvent(Chapter2Event::MuralCopied);
            m_Message =
                "일곱 귀환자, 알려진 이름은 여섯. 지워진 인물은 쓰러진 이종족에게 손을 내밀고 있다. 탁본 확보.";
        }
        if (m_State.phase == Chapter2Phase::Reunion)
        {
            if (m_Combat.group != 221)
            {
                BeginEncounter(221, {450, -2100}, 2);
                m_Combat.hero = {-500, -2100};
                // Visible opposing line pins the hero while the player opens the bypass.
                SpawnOrcs(enemies, map, {-500, -2100}, 229, 4, 2);
            }
            if (m_Combat.cleared)
            {
                const bool nearLine = Distance(player, {450, -1920}) < 90.f;
                const bool nearDoor = Distance(player, {450, -2270}) < 90.f;
                if (nearLine && !Has(Chapter2Event::DemolitionCut))
                {
                    if (HoldDevice(dt, held, player, {450, -1920}, map, 2.5f))
                    {
                        RecordEvent(Chapter2Event::DemolitionCut);
                        m_Message = "폭파선 차단. 용사에게 합류할 길을 열어야 합니다.";
                    }
                }
                else if (nearDoor && !Has(Chapter2Event::BypassOpened))
                {
                    if (HoldDevice(dt, held, player, {450, -2270}, map, 2.5f))
                    {
                        RecordEvent(Chapter2Event::BypassOpened);
                        m_Message =
                            "우회문 개방. 두 장치가 준비되면 합류 지점에서 신호를 보내세요.";
                    }
                }
                else
                {
                    m_Combat.interaction = 0.f;
                }
            }
        }
        if (m_State.phase == Chapter2Phase::Boss)
        {
            if (!m_Combat.demolitionDisabled && Distance(player, {-300, -3040}) < 90.f &&
                HoldDevice(dt, held, player, {-300, -3040}, map, 2.f))
            {
                m_Combat.demolitionDisabled = true;
                m_Combat.explosionWarning = 0.f;
                m_Message = "교량 폭파 장치 차단. 바르칸의 퇴로를 막으세요.";
            }
            UpdateBoss(dt, player, map, enemies);
            auto* boss = enemies.Boss();
            if (boss && boss->health <= boss->maxHealth * 2 / 3 &&
                !Has(Chapter2Event::RetreatBlocked) && Distance(player, {-340, -2830}) < 90.f &&
                HoldDevice(dt, held, player, {-340, -2830}, map, 2.f))
            {
                RecordEvent(Chapter2Event::RetreatBlocked);
                m_Message = "퇴로 봉쇄! 바르칸을 더 압박하면 용사가 자세를 무너뜨립니다.";
            }
        }
    }
    if (m_State.phase != Chapter2Phase::Boss && m_State.phase != Chapter2Phase::Aftermath &&
        m_State.phase != Chapter2Phase::Complete)
    {
        UpdateEncounter(player, map, enemies);
    }
    const bool workshopHazard = m_State.map == Chapter2Maps::Canyon &&
                                m_State.activeTarget == Chapter2Target::Workshop &&
                                Distance(player, TargetPosition(Chapter2Target::Workshop)) < 350.f;
    const bool bossHazard = m_State.phase == Chapter2Phase::Boss && enemies.Boss() &&
                            !m_Combat.demolitionDisabled && !m_Combat.finishing;
    if (workshopHazard || bossHazard)
    {
        if (m_Combat.explosionWarning > 0.f)
        {
            m_Combat.explosionWarning = (std::max)(0.f, m_Combat.explosionWarning - dt);
            if (m_Combat.explosionWarning == 0.f &&
                Distance(player, m_Combat.explosionPosition) < 95.f &&
                map.ClearPath(player, m_Combat.explosionPosition))
            {
                m_Impact.damage = (std::max)(m_Impact.damage, 16);
            }
        }
        else
        {
            m_Combat.hazardCooldown -= dt;
            if (m_Combat.hazardCooldown <= 0.f)
            {
                m_Combat.explosionPosition = player;
                m_Combat.explosionWarning = 1.5f;
                m_Combat.hazardCooldown = 8.f - m_State.outcome.bridgeDamage * 2.f;
            }
        }
    }
    else
    {
        m_Combat.explosionWarning = 0.f;
    }
    UpdateHero(dt, player, map, enemies);
    if (const auto* boss = enemies.Boss())
    {
        m_Combat.bossPosition = boss->position;
        m_Combat.bossHealthRatio = float(boss->health) / (std::max)(1, boss->maxHealth);
        m_Combat.bossTarget = boss->actionTarget;
        m_Combat.bossAction = boss->health > 0 ? boss->action : EnemyAction::None;
        m_Combat.bossProgress = boss->actionTime / boss->actionDuration;
    }
}

void Chapter2::UpdateHero(float dt, WorldPosition player, const LevelMap& map, EnemySystem& enemies)
{
    auto& combat = m_Combat;
    if (combat.finishing)
    {
        return;
    }
    combat.heroCooldown = (std::max)(0.f, combat.heroCooldown - dt);
    combat.heroAttack = (std::max)(0.f, combat.heroAttack - dt);
    combat.heroRepath = (std::max)(0.f, combat.heroRepath - dt);
    const bool holding = m_State.map == Chapter2Maps::OldFortress &&
                         m_State.phase == Chapter2Phase::Reunion && !combat.reunionRequested;
    const bool rejoining = m_State.map == Chapter2Maps::OldFortress &&
                           m_State.phase == Chapter2Phase::Reunion && combat.reunionRequested;
    WorldPosition goal = holding     ? WorldPosition{-500, -2100}
                         : rejoining ? WorldPosition{0, -2470}
                                     : SafeSpawn(player, -55, 20, map);
    std::uint64_t targetId = 0;
    float nearest = holding ? 240.f : 360.f;
    if (!rejoining && !combat.finishing)
    {
        for (const auto& enemy : enemies.Enemies())
        {
            const float distance = Distance(combat.hero, enemy.position);
            if (enemy.health > 0 && distance < nearest && (holding || enemy.encounter != 229) &&
                (!holding || enemy.encounter == 229) &&
                (holding || Distance(player, enemy.position) < 540.f))
            {
                targetId = enemy.id;
                nearest = distance;
                goal = enemy.position;
            }
        }
    }
    if (targetId && nearest < 75.f && map.ClearPath(combat.hero, goal))
    {
        if (combat.heroCooldown <= 0.f)
        {
            enemies.DamageEnemy(targetId, 18);
            combat.heroCooldown = .85f;
            combat.heroAttack = .35f;
            auto* boss = enemies.Boss();
            if (boss && targetId == boss->id && Has(Chapter2Event::RetreatBlocked) &&
                boss->health <= boss->maxHealth / 3 && !Has(Chapter2Event::ArmorBroken))
            {
                RecordEvent(Chapter2Event::ArmorBroken);
                boss->armorBroken = true;
                boss->action = EnemyAction::None;
                boss->windup = 0.f;
                m_Message = "용사: 지금이야! 동쪽 제어대의 지휘 인장을 직접 공격하세요.";
            }
        }
    }
    else if (Distance(combat.hero, goal) > 35.f)
    {
        if (combat.heroRepath <= 0.f)
        {
            m_HeroPathfinder.FindPath(combat.hero, goal, map, combat.heroPath);
            combat.heroWaypoint = 0;
            combat.heroRepath = combat.heroPath.empty() ? .8f : .6f;
        }
        float remaining = 175.f * dt;
        while (remaining > 0.f && combat.heroWaypoint < combat.heroPath.size())
        {
            const auto waypoint = combat.heroPath[combat.heroWaypoint];
            const float distance = Distance(combat.hero, waypoint);
            if (distance < .001f)
            {
                ++combat.heroWaypoint;
                continue;
            }
            const float step = (std::min)(remaining, distance);
            const auto next = Toward(combat.hero, waypoint, step);
            if (!map.ClearPath(combat.hero, next))
            {
                combat.heroPath.clear();
                combat.heroWaypoint = 0;
                break;
            }
            if (std::abs(next.x - combat.hero.x) > .001f)
            {
                combat.heroFaceLeft = next.x < combat.hero.x;
            }
            combat.heroMoving = true;
            combat.hero = next;
            remaining -= step;
            if (step >= distance)
            {
                ++combat.heroWaypoint;
            }
        }
    }

    if (rejoining && Distance(combat.hero, {0, -2470}) < 65.f)
    {
        // Leave the surviving opposing line behind when the hero disengages.
        RecordEvent(Chapter2Event::HeroRejoined);
        combat.group = 0;
        m_Message = "용사: 전멸시킬 필요는 없었군. 함께 바르칸의 퇴로를 끊자.";
    }
}

void Chapter2::UpdateBoss(float dt, WorldPosition player, const LevelMap& map, EnemySystem& enemies)
{
    auto* boss = enemies.Boss();
    if (!boss)
    {
        if (Distance(player, {0, -2900}) > 540.f)
        {
            return;
        }
        EnemyStats stats;
        stats.health = BarkanHealth;
        stats.damage = 18;
        stats.minimumHealth = 1;
        stats.appearance = 2;
        stats.scriptedAI = true;
        stats.preserveCorpse = true;
        enemies.Add({0, -2900}, 230, EnemyRole::Boss, stats);
        const int pressure = m_State.outcome.eliteReinforcement;
        SpawnOrcs(enemies, map, {180, -3020}, 231, pressure * 2, pressure * 2);
        boss = enemies.Boss(); // Add may reallocate the backing vector.
        m_Combat.reinforcements = true;
        m_Message = "바르칸: 늦었다! 봉화 대응 순서에 따른 정예병이 합류합니다.";
    }
    if (!boss)
    {
        return;
    }
    if (Has(Chapter2Event::CommandTargetDestroyed))
    {
        m_Combat.finishing = true;
        m_Combat.finisher += dt;
        m_Combat.heroAttack = .4f;
        m_Combat.hero = Toward(m_Combat.hero, boss->position, 180.f * dt);
        boss->action = EnemyAction::None;
        if (m_Combat.finisher >= 2.4f)
        {
            boss->stats.minimumHealth = 0;
            enemies.DamageEnemy(boss->id, boss->health);
            boss->ai.state = EnemyState::Dead;
            boss->stateTime = 0.f;
            enemies.RemoveEncounter(231);
            RecordEvent(Chapter2Event::JointFinisherEnded);
            m_Combat.finishing = false;
            m_Message =
                "플레이어가 지휘를 끊고 용사가 빈틈을 꿰뚫었다. 구호지에서 남은 피해를 확인하자.";
        }
        return;
    }
    if (Has(Chapter2Event::ArmorBroken))
    {
        boss->ai.state = EnemyState::Disabled;
        return;
    }
    m_Combat.jumpCooldown = (std::max)(0.f, m_Combat.jumpCooldown - dt);
    m_Combat.cryCooldown = (std::max)(0.f, m_Combat.cryCooldown - dt);
    boss->attackSpeedTime = (std::max)(0.f, boss->attackSpeedTime - dt);
    boss->cooldown = (std::max)(0.f, boss->cooldown - dt);
    const float distance = Distance(player, boss->position);
    m_Combat.bossPosition = boss->position;
    m_Combat.bossTarget = boss->actionTarget;
    m_Combat.bossAction = boss->action;
    m_Combat.bossProgress = boss->actionTime / boss->actionDuration;
    if (boss->action == EnemyAction::None)
    {
        boss->faceLeft = player.x < boss->position.x;
    }
    if (boss->action != EnemyAction::None)
    {
        boss->actionTime += dt;
        boss->windup = (std::max)(0.f, boss->actionDuration - boss->actionTime);
        if (boss->action == EnemyAction::JumpSlam)
        {
            const float t =
                std::clamp((boss->actionTime / boss->actionDuration - .3f) / .65f, 0.f, 1.f);
            boss->position = {
                boss->actionOrigin.x + (boss->actionTarget.x - boss->actionOrigin.x) * t,
                boss->actionOrigin.y + (boss->actionTarget.y - boss->actionOrigin.y) * t};
        }
        if (boss->actionTime < boss->actionDuration)
        {
            return;
        }
        const float range = boss->action == EnemyAction::JumpSlam    ? 110.f
                            : boss->action == EnemyAction::BattleCry ? CryRadius
                                                                     : 85.f;
        const bool facing = boss->action != EnemyAction::Swing ||
                            (player.x - boss->position.x) * (boss->faceLeft ? -1.f : 1.f) >= -18.f;
        if (facing && Distance(player, boss->position) < range &&
            map.ClearPath(boss->position, player))
        {
            m_Impact.damage = boss->action == EnemyAction::BattleCry  ? 0
                              : boss->action == EnemyAction::JumpSlam ? 26
                                                                      : boss->stats.damage;
            if (boss->action == EnemyAction::BattleCry)
            {
                const float length = (std::max)(1.f, Distance(player, boss->position));
                m_Impact.push = {(player.x - boss->position.x) / length * 120.f,
                                 (player.y - boss->position.y) / length * 120.f};
            }
        }
        if (boss->action == EnemyAction::BattleCry)
        {
            boss->attackSpeedTime = CryBuffSeconds;
            if (Distance(m_Combat.hero, boss->position) < CryRadius)
            {
                // Step away from the boss, respecting authored collision.
                const float length = (std::max)(1.f, Distance(boss->position, m_Combat.hero));
                const WorldPosition away{
                    m_Combat.hero.x + (m_Combat.hero.x - boss->position.x) / length * 100.f,
                    m_Combat.hero.y + (m_Combat.hero.y - boss->position.y) / length * 100.f};
                if (map.ClearPath(m_Combat.hero, away))
                {
                    m_Combat.hero = away;
                }
                m_Combat.heroCooldown = 1.4f;
            }
        }
        boss->action = EnemyAction::None;
        boss->ai.state = EnemyState::Recovery;
        boss->stateTime = 0.f;
        boss->cooldown = boss->attackSpeedTime > 0.f ? .65f : 1.1f;
        return;
    }
    if (!Has(Chapter2Event::RetreatBlocked) && boss->health <= boss->maxHealth * 2 / 3)
    {
        const auto retreat = Toward(boss->position, {0, -3210}, 100.f * dt);
        if (map.ClearPath(boss->position, retreat))
        {
            boss->position = retreat;
        }
        boss->ai.state = EnemyState::Chase;
        m_Message = "바르칸이 북쪽으로 후퇴합니다! 서쪽 쇠사슬 장치에서 Space를 유지하세요.";
        return;
    }
    if (boss->cooldown > 0.f || distance > 900.f)
    {
        return;
    }
    EnemyAction next = EnemyAction::None;
    if (m_Combat.cryCooldown <= 0.f && distance < CryRadius)
    {
        next = EnemyAction::BattleCry;
        m_Combat.cryCooldown = CryCooldown;
    }
    else if (distance > JumpDistance && m_Combat.jumpCooldown <= 0.f &&
             Distance(player, {0, -2900}) < 500.f && map.ClearPath(boss->position, player))
    {
        next = EnemyAction::JumpSlam;
        m_Combat.jumpCooldown = JumpCooldown;
    }
    else if (distance < 78.f)
    {
        next = EnemyAction::Swing;
    }
    if (next != EnemyAction::None)
    {
        boss->action = next;
        boss->actionOrigin = boss->position;
        boss->actionTarget = player; // Telegraph locks the target before takeoff.
        boss->actionTime = 0.f;
        boss->actionDuration = next == EnemyAction::JumpSlam ? 1.65f
                               : next == EnemyAction::BattleCry
                                   ? 1.2f
                                   : (boss->attackSpeedTime > 0.f ? .85f / CrySpeed : .85f);
        boss->windup = boss->actionDuration;
        boss->ai.state = EnemyState::Windup;
        boss->stateTime = 0.f;
    }
    else
    {
        const auto step = Toward(boss->position, player, 85.f * dt);
        if (Distance(step, {0, -2900}) < 490.f && map.ClearPath(boss->position, step))
        {
            boss->position = step;
            boss->ai.state = EnemyState::Chase;
        }
    }
}

void Chapter2::DestroyCommandTarget()
{
    if (RecordEvent(Chapter2Event::CommandTargetDestroyed))
    {
        m_Combat.finishing = true;
        m_Message = "지휘 인장 파괴! 용사: 네가 만든 길이다. 함께 끝내자.";
    }
}

void Chapter2::StrikeObjects(
    WorldPosition player, bool faceLeft, int damage, const LevelMap& map, float rangeScale)
{
    const WorldPosition target{340, -2830};
    const float dx = (target.x - player.x) * (faceLeft ? -1.f : 1.f);
    const float dy = target.y - player.y;
    if (m_State.map == Chapter2Maps::OldFortress && damage > 0 && Has(Chapter2Event::ArmorBroken) &&
        dx >= -18.f * rangeScale &&
        dx * dx + dy * dy * 2.f <=
            LevelTuning::HitRange * LevelTuning::HitRange * rangeScale * rangeScale &&
        map.ClearPath(player, target))
    {
        DestroyCommandTarget();
    }
}

void Chapter2::StrikeProjectileObjects(WorldPosition from,
                                       WorldPosition to,
                                       float radiusX,
                                       float radiusY,
                                       int damage,
                                       const LevelMap& map,
                                       float height,
                                       std::unordered_set<std::uint64_t>*,
                                       std::optional<WorldPosition> sightOrigin)
{
    const WorldPosition target{340, -2830};
    const float x = std::clamp(target.x, (std::min)(from.x, to.x), (std::max)(from.x, to.x));
    if (m_State.map == Chapter2Maps::OldFortress && damage > 0 && Has(Chapter2Event::ArmorBroken) &&
        std::abs(x - target.x) <= radiusX + 20.f &&
        std::abs((target.y - to.y) * SceneCamera::GroundDepthScale() + height - 35.f) <=
            radiusY + 35.f &&
        map.ClearPath(sightOrigin.value_or(WorldPosition{x, to.y}), target))
    {
        DestroyCommandTarget();
    }
}

bool Chapter2::BlocksMovement(WorldPosition p) const
{
    if (m_State.map != Chapter2Maps::OldFortress || (ContinuousWorld() && std::abs(p.x) > 1100.f))
    {
        return false;
    }
    if (m_State.phase == Chapter2Phase::Infiltration && p.y < -910.f && p.y > -985.f)
    {
        if (!m_State.route)
        {
            return true;
        }
        const bool selected = *m_State.route == Chapter2Route::Aqueduct ? p.x < -200.f
                              : *m_State.route == Chapter2Route::Breach ? p.x > 200.f
                                                                        : std::abs(p.x) < 200.f;
        return !selected;
    }
    if (!Has(Chapter2Event::MuralCopied) && p.y < -1445.f && p.y > -1515.f)
    {
        return true;
    }
    return !Has(Chapter2Event::HeroRejoined) && p.y < -2520.f && p.y > -2560.f;
}
