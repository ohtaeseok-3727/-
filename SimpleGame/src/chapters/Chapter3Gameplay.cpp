#include "stdafx.h"
#include "Chapter3.h"
#include <algorithm>
#include <cmath>

namespace
{
    float Distance(WorldPosition a, WorldPosition b)
    {
        return std::hypot(a.x - b.x, a.y - b.y);
    }

    bool Move(WorldPosition& position, WorldPosition target, float step, const LevelMap& map)
    {
        const float distance = Distance(position, target);
        if (distance < .01f)
        {
            return true;
        }
        const float amount = (std::min)(1.f, step / distance);
        const WorldPosition next{position.x + (target.x - position.x) * amount,
                                 position.y + (target.y - position.y) * amount};
        if (!map.Blocked(next.x, next.y) && map.ClearPath(position, next))
        {
            position = next;
        }
        return distance <= step;
    }
}

bool Chapter3::Hold(float dt,
                    bool held,
                    WorldPosition player,
                    WorldPosition target,
                    const LevelMap& map,
                    int action,
                    float seconds)
{
    if (m_HoldAction != action)
    {
        m_Hold = 0.f;
        m_HoldAction = action;
    }
    if (!held || Distance(player, target) > 85.f || !map.ClearPath(player, target))
    {
        m_Hold = 0.f;
        return false;
    }
    m_Hold += dt;
    if (m_Hold < seconds)
    {
        return false;
    }
    m_Hold = 0.f;
    return true;
}

WorldPosition Chapter3::Patrol(int index, float time)
{
    // Both patrols traverse authored, unobstructed aisles. The west passage is a stealth bypass.
    const float t = std::fmod(time * .12f + index * .5f, 2.f);
    const float progress = t < 1.f ? t : 2.f - t;
    return {0.f, -150.f - index * 400.f - progress * 280.f};
}

WorldPosition Chapter3::Cart() const
{
    return {0.f, -1050.f - m_CartDistance};
}

void Chapter3::Update(float dt,
                      WorldPosition player,
                      bool held,
                      const LevelMap& map,
                      EnemySystem& enemies,
                      PlayerProgression&)
{
    if (dt <= 0.f || !m_State.previousComplete)
    {
        return;
    }
    dt = (std::min)(dt, .1f);
    m_Impact = {};
    m_ParryWindow = (std::max)(0.f, m_ParryWindow - dt);
    m_ParryCooldown = (std::max)(0.f, m_ParryCooldown - dt);
    if (held && !m_WasHeld && m_ParryCooldown <= 0.f)
    {
        m_ParryWindow = .28f;
        m_ParryCooldown = .65f;
    }
    m_WasHeld = held;
    using Phase = Chapter3Phase;
    if (m_State.map == Chapter3Maps::Argen)
    {
        if (m_State.phase == Phase::Chase)
        {
            constexpr std::array<WorldPosition, 4> route = {
                {{360, -650}, {-360, -650}, {-360, -250}, {360, -250}}};
            if (Move(m_Courier, route[m_CourierLeg], 105.f * dt, map))
            {
                m_CourierLeg = (m_CourierLeg + 1) % static_cast<int>(route.size());
            }
            if (m_State.shortcut && Hold(dt, held, player, m_Courier, map, 1, .45f))
            {
                Advance(
                    Phase::Research,
                    "탁본 회수 · 연락책 생존 제압. '왕립 기록관에 들어간 원본은 다시 나오지 않아.' 절도는 잘못임을 지적하고 에드릭 연락 표식을 보관했다. 세 독립 증거를 조사하자.");
            }
        }
        else if (m_State.phase == Phase::Research)
        {
            UpdateResearch(dt, player, held, map);
        }
        for (std::size_t i = 0; i < m_Delivery.size(); ++i)
        {
            if (i < 3 ? m_State.copies.test(i) : m_State.extraCopy.has_value())
            {
                m_Delivery[i] = (std::min)(4.f, m_Delivery[i] + dt);
            }
        }
    }
    if (m_State.phase == Phase::Research && m_State.manuscripts == 3 && m_State.stoneLedger &&
        m_State.monastery == 3)
    {
        Advance(
            Phase::Censure,
            "교차 검증 완료: 실존·반복 삭제·사상 변화. 왕립 대서고 중앙 회랑에서 세라드의 통보를 확인하세요.");
    }
    if (m_State.map == Chapter3Maps::Archive)
    {
        if (m_State.phase == Phase::Infiltration)
        {
            UpdateInfiltration(dt, player, held, map);
        }
        else if (m_State.phase == Phase::Interception)
        {
            UpdateInterception(dt, player, held, map, enemies);
        }
    }
}

void Chapter3::UpdateResearch(float dt, WorldPosition player, bool held, const LevelMap& map)
{
    using namespace Chapter3Objects;
    bool working = false;
    for (int i = 0; i < 3; ++i)
    {
        if (!m_State.foundations.test(i) && Distance(player, Foundation(i)) < 85.f)
        {
            working = true;
            if (Hold(dt, held, player, Foundation(i), map, FoundationWest + i, 2.f))
            {
                m_State.foundations.set(i);
                m_Message = "석상 기초 복원 " + std::to_string(m_State.foundations.count()) +
                            "/3 · 같은 석재·고정 홈·접합 시기. 완료 후 공방 장부를 대조하세요.";
            }
            break;
        }
    }
    if (!working)
    {
        m_Hold = 0.f;
    }
    constexpr std::array<WorldPosition, 3> rooms = {{{500, -1450}, {900, -1600}, {600, -1850}}};
    if (m_State.monastery < 3 && Distance(player, rooms[m_State.monastery]) < 75.f)
    {
        constexpr std::array<const char*, 3> messages = {
            "초기 필체·인장 확인: 이종족 피난민 수용. 외벽의 장례 명부 기록으로 이동하세요.",
            "같은 필체·인장 확인: 인간은 인간의 힘으로 지켜야 한다. 외벽의 마지막 설교실 배치로 이어집니다.",
            "후기 설교 확인: 공존은 인간을 파멸시킨다. 같은 사람의 변화가 드러났다. 원정 목적까지 단정하지 않는다."};
        m_Message = messages[m_State.monastery++];
    }
}

void Chapter3::UpdateInfiltration(float dt, WorldPosition player, bool held, const LevelMap& map)
{
    m_PatrolTime += dt;
    bool detected = false;
    bool working = false;
    for (int i = 0; i < 2; ++i)
    {
        if (m_Subdued.test(i))
        {
            continue;
        }
        const auto p = Patrol(i, m_PatrolTime);
        const float distance = Distance(player, p);
        if (distance < 85.f)
        {
            working = true;
            if (Hold(dt, held, player, p, map, 400 + i, .6f))
            {
                m_Subdued.set(i);
                m_Alarm = 0.f;
                m_Message = "순찰대 생존 제압. 무기를 치우고 원본 탐색을 계속합니다.";
                continue;
            }
        }
        detected = detected || (distance < 145.f && map.ClearPath(p, player));
    }
    if (!working)
    {
        m_Hold = 0.f;
    }
    m_Alarm = detected ? m_Alarm + dt : (std::max)(0.f, m_Alarm - dt * 2.f);
    if (m_Alarm >= 1.5f)
    {
        m_Alarm = 0.f;
        m_Impact.damage = 6;
        m_Message =
            "순찰대 경고 타격! 서쪽 우회 통로로 피하거나 가까이서 Space 유지로 비살상 제압하세요.";
    }
}

void Chapter3::ResetInterception(EnemySystem& enemies)
{
    // The archive has no ambient population; it owns only this scripted combatant.
    enemies.Clear();
    m_ResetCombat = false;
    m_CartDistance = m_AttackTime = m_ParryWindow = 0.f;
    m_Recovery = 1.f;
    m_AttackNumber = 0;
    m_State.wheelsStopped = m_State.brake = m_State.seradAlive = false;
    m_BossPosition = {0.f, -1150.f};
    m_Hold = 0.f;
    m_HoldAction = 0;
}

void Chapter3::UpdateInterception(
    float dt, WorldPosition player, bool held, const LevelMap& map, EnemySystem& enemies)
{
    if (m_ResetCombat)
    {
        ResetInterception(enemies);
    }
    if (m_State.seradAlive)
    {
        if (Hold(dt, held, player, Cart(), map, Chapter3Objects::RecoverOriginal, 1.f))
        {
            m_State.original = true;
            Advance(
                Chapter3Phase::Preservation,
                "세라드는 살아 있습니다. 멈춘 운반대에서 원본을 직접 회수했습니다. 아르겐에서 공개문을 검증하고 공증 사본을 분산하세요.");
        }
        return;
    }
    if (!enemies.Alive(SeradEncounter))
    {
        // Minimum health protects every existing melee/projectile/ultimate damage path.
        EnemyStats stats;
        stats.health = 320;
        stats.minimumHealth = 1;
        stats.scriptedAI = true;
        stats.appearance = 301;
        enemies.Add(m_BossPosition, SeradEncounter, EnemyRole::Boss, stats);
    }
    auto* boss = enemies.Boss();
    if (!boss || boss->health <= 0)
    {
        return;
    }
    if (!m_State.brake)
    {
        m_CartDistance += dt * (m_State.wheelsStopped ? 9.f : 23.f);
        if (m_CartDistance >= 900.f)
        {
            ResetInterception(enemies);
            m_Message =
                "원본이 승강대에 도착했습니다. 반출 저지전을 다시 시도합니다. 운반대 바퀴 → 승강 브레이크 → 세라드 생존 제압 순서입니다.";
            return;
        }
    }
    bool working = false;
    if (!m_State.wheelsStopped && Distance(player, Cart()) < 85.f)
    {
        working = true;
        if (Hold(dt, held, player, Cart(), map, 500, 1.2f))
        {
            m_State.wheelsStopped = true;
            m_Message =
                "바퀴 고정장치 해제! 두 번째 운반조로 넘어가 속도가 느려졌습니다. 승강 브레이크를 내리세요.";
        }
    }
    else if (m_State.wheelsStopped && !m_State.brake && Distance(player, {350.f, -1800.f}) < 85.f)
    {
        working = true;
        if (Hold(dt, held, player, {350.f, -1800.f}, map, Chapter3Objects::Brake, 2.f))
        {
            m_State.brake = true;
            m_Message =
                "최종 반출 중단. 세라드의 기력을 소진시킨 뒤 곁에서 Space 유지로 생존 제압하세요.";
        }
    }
    else if (m_State.brake && boss->health == 1 && Distance(player, boss->position) < 85.f)
    {
        working = true;
        if (Hold(dt, held, player, boss->position, map, Chapter3Objects::Disarm, 1.f))
        {
            m_State.seradAlive = true;
            m_BossPosition = boss->position;
            m_AttackTime = 0.f;
            // Retire from the combat system without a death or loot event; view retains the survivor.
            enemies.Clear();
            m_Message =
                "세라드 생존 제압. 무장을 내려놓았고 원본 반출은 멈췄습니다. 멈춘 운반대 곁에서 Space를 유지해 원본을 회수하세요.";
            return;
        }
    }
    if (!working)
    {
        m_Hold = 0.f;
    }
    boss->hitFlash = (std::max)(0.f, boss->hitFlash - dt);
    if (boss->health == 1)
    {
        m_AttackTime = 0.f;
        boss->windup = 0.f;
        m_BossPosition = boss->position;
        return;
    }
    if (m_AttackTime > 0.f)
    {
        m_AttackTime = (std::max)(0.f, m_AttackTime - dt);
        if (m_AttackTime == 0.f)
        {
            const auto target = m_Ranged ? m_AttackTarget : boss->position;
            if (Distance(player, target) < (m_Ranged ? 75.f : 105.f) &&
                map.ClearPath(boss->position, player))
            {
                if (m_ParryWindow > 0.f)
                {
                    enemies.DamageEnemy(boss->id, 45);
                    m_Message = "패링 성공! 세라드의 자세가 무너졌다. 운반대를 차단할 틈이다.";
                    m_Recovery = 2.f;
                }
                else
                {
                    m_Impact.damage = m_Ranged ? 12 : 16;
                    m_Recovery = 1.1f;
                }
            }
            else
            {
                m_Recovery = 1.1f;
            }
        }
    }
    else if (m_Recovery > 0.f)
    {
        m_Recovery -= dt;
        if (Distance(player, boss->position) > 95.f && player.y < -1000.f)
        {
            Move(boss->position, player, dt * 90.f, map);
        }
    }
    else if (Distance(player, boss->position) < 600.f)
    {
        ++m_AttackNumber;
        m_Ranged = Distance(player, boss->position) > 160.f || m_AttackNumber % 3 == 0;
        m_AttackTarget = player;
        m_AttackTime = m_Ranged ? 1.2f : .9f;
    }
    m_BossPosition = boss->position;
    boss->windup = m_AttackTime;
}

std::optional<ChapterGoal> Chapter3::Goal() const
{
    using namespace Chapter3Objects;
    using Phase = Chapter3Phase;
    const auto goal = [](MapId map, ObjectId object, const char* hint = "[Space] 확인")
    {
        return ChapterGoal{map, object, std::nullopt, {}, hint};
    };
    switch (m_State.phase)
    {
    case Phase::Memorial:
        for (int i = 0; i < 3; ++i)
        {
            if (!m_State.memorial.test(i))
            {
                return goal(Chapter3Maps::Memorial, Ceremony + i);
            }
        }
        break;
    case Phase::City:
        return goal(Chapter3Maps::Argen,
                    !m_State.petition  ? Petition
                    : !m_State.rations ? Rations
                                       : Permit);
    case Phase::Chase:
        if (!m_State.shortcut)
        {
            return goal(Chapter3Maps::Argen, Shortcut);
        }
        return ChapterGoal{
            Chapter3Maps::Argen, 0, m_Courier, "탁본 연락책", "곁에서 [Space 유지] · 비살상 제압"};
    case Phase::Research:
        if (m_State.manuscripts < 3)
        {
            return goal(Chapter3Maps::ReadingRoom,
                        ManuscriptEarly + m_State.manuscripts,
                        "[Space] 오래된 사본부터 시간축 정렬");
        }
        for (int i = 0; i < 3; ++i)
        {
            if (!m_State.foundations.test(i))
            {
                return goal(Chapter3Maps::Argen, FoundationWest + i, "[Space 2초 유지] 구조 복원");
            }
        }
        return goal(Chapter3Maps::Argen,
                    !m_State.stoneLedger ? WorkshopLedger : RefugeRoom + m_State.monastery,
                    "장부 [Space] · 수도원 외벽의 필체·방 배치 조사");
    case Phase::Censure:
        return goal(Chapter3Maps::ReadingRoom, Censure);
    case Phase::Edric:
        return goal(Chapter3Maps::Argen, m_State.edricHeard ? TransferSeal : Edric);
    case Phase::Infiltration:
        return goal(Chapter3Maps::Archive,
                    !m_State.movementList  ? MovementList
                    : !m_State.storageDate ? StorageDate
                                           : OriginalBox);
    case Phase::Interception:
        if (!m_State.wheelsStopped)
        {
            return ChapterGoal{Chapter3Maps::Archive,
                               0,
                               Cart(),
                               "움직이는 원본 운반대",
                               "[Space 유지] 바퀴 고정장치 해제"};
        }
        if (!m_State.brake)
        {
            return goal(Chapter3Maps::Archive, Brake, "[Space 2초 유지] 승강대 정지");
        }
        if (!m_State.seradAlive)
        {
            return ChapterGoal{
                Chapter3Maps::Archive,
                0,
                m_BossPosition,
                "세라드 생존 제압",
                "[Ctrl/스킬] 기력 소진 · 공격 직전 [Space] 패링 · 소진 후 [Space 유지]"};
        }
        return ChapterGoal{
            Chapter3Maps::Archive, 0, Cart(), "멈춘 운반대의 원본 상자", "[Space 유지] 직접 회수"};
    case Phase::Preservation:
        for (int i = 0; i < 2; ++i)
        {
            if (!m_State.notice.test(i))
            {
                return goal(Chapter3Maps::Argen, ConfirmedNotice + i);
            }
        }
        for (int i = 0; i < 3; ++i)
        {
            if (!m_State.copies.test(i))
            {
                return goal(Chapter3Maps::Argen, CopyCommunity + i);
            }
        }
        return goal(Chapter3Maps::Argen,
                    m_State.extraCopy ? Testimony : ExtraClergy,
                    m_State.extraCopy ? "보관자 출발 후 [Space] 증언"
                                      : "[Space] 성직자 또는 동쪽 상인 조합 중 선택");
    case Phase::Hearing:
        if (!m_State.hearing)
        {
            return goal(Chapter3Maps::Argen, Hearing);
        }
        for (int i = 0; i < 5; ++i)
        {
            if (!m_State.restrictions.test(i))
            {
                return goal(Chapter3Maps::Argen, RestrictedDoor + i);
            }
        }
        return goal(Chapter3Maps::Argen, FieldPermit);
    case Phase::Complete:
        return std::nullopt;
    }
    return std::nullopt;
}

std::string Chapter3::Objective() const
{
    constexpr std::array<const char*, 11> text = {
        "일곱 불·여섯 명판·벽화 탁본을 비교",
        "행정 민원·배급소 확인 후 원본 열람 허가",
        "시장 지름길 확보 → 연락책 비살상 제압 [Space 유지]",
        "사본 연대 정렬 / 석상 구조 복원 / 수도원 필체 동선 추적",
        "대서고 중앙 회랑에서 세라드의 회수 통보 확인",
        "에드릭의 설명을 듣고 작전 인장 교차 검증",
        "순찰 회피·비살상 제압 → 이동표·연대 확인 → 원본 상자 선택",
        "움직이는 운반대 → 승강 브레이크 → 세라드 생존 제압 → 원본 회수",
        "확인·미확인 공개문 → 필수 사본 3부·추가 1부 → 용사 증언",
        "심의 통보 → 출입·해임·보급 제재·유효한 지휘권 확인 → 제한 조사 승인",
        "챕터 3 완료 · 왕실 심의 중 · 제한 조사 승인 · 다음 단서: 야수족 영역 원정 좌표"};
    return text[static_cast<std::size_t>(m_State.phase)];
}
