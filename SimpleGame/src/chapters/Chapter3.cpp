#include "stdafx.h"
#include "Chapter3.h"
#include <algorithm>

const char* Chapter3::Title() const
{
    return "챕터 3 · 이름 없는 일곱째";
}

MapId Chapter3::InitialMap() const
{
    return Chapter3Maps::Memorial;
}

void Chapter3::Reset()
{
    m_State = {};
    m_Impact = {};
    m_Courier = {360.f, -250.f};
    m_CourierLeg = 0;
    m_Hold = m_PatrolTime = m_Alarm = m_CartDistance = m_AttackTime = 0.f;
    m_HoldAction = m_AttackNumber = 0;
    m_Recovery = 1.f;
    m_AttackTarget = {};
    m_BossPosition = {0.f, -1150.f};
    m_Subdued.reset();
    m_Ranged = m_WasHeld = false;
    m_ResetCombat = false;
    m_ParryWindow = m_ParryCooldown = 0.f;
    m_Delivery.fill(0.f);
    ClearTravel();
    m_Message = "일곱 개의 불, 여섯 이름. 벽화 탁본과 공식 전승을 비교하세요.";
}

void Chapter3::EnterMap(MapId map) noexcept
{
    m_State.map = map;
    m_Hold = m_ParryWindow = 0.f;
    m_HoldAction = 0;
    m_WasHeld = false;
    m_Impact = {};
    ClearTravel();
}

void Chapter3::ImportPrevious(const ChapterFlags& flags)
{
    const auto read = [&](const char* key)
    {
        const auto found = flags.find(key);
        return found == flags.end() ? 0 : found->second;
    };
    m_State.previousComplete = read("C02_COMPLETE") != 0;
    m_State.supplyLoss = read("C02_SUPPLY_LOSS");
    m_State.refugeeLoss = read("C02_REFUGEE_LOSS");
}

void Chapter3::Advance(Chapter3Phase phase, const char* message)
{
    m_State.phase = phase;
    m_Hold = 0.f;
    m_HoldAction = 0;
    m_Message = message;
}

std::string Chapter3::Interact(const MapLandmark& site,
                               const LevelMap& map,
                               EnemySystem& enemies,
                               PlayerProgression& progress,
                               PlayerInventory& inventory)
{
    using namespace Chapter3Objects;
    using Phase = Chapter3Phase;
    if (!m_State.previousComplete)
    {
        return "먼저 협곡 구호지에서 피해 보고와 벽화 탁본 검토를 마치세요.";
    }
    // Reject foreign/local IDs before changing state, including out-of-order archive visits.
    const auto& sites = map.Layout().Landmarks();
    if (std::none_of(sites.begin(),
                     sites.end(),
                     [&](const MapLandmark& item)
                     {
                         return item.object == site.object;
                     }))
    {
        return Objective();
    }
    const auto phase = m_State.phase;
    const auto id = site.object;
    if (id == CityExit && phase >= Phase::City)
    {
        if (!ContinuousWorld())
        {
            RequestTravel({{}, Chapter3Maps::Argen, std::nullopt});
        }
        return "연결된 길을 따라 기록도시 아르겐으로 이동하세요.";
    }
    if (id == ArchiveExit && phase >= Phase::Infiltration)
    {
        if (!ContinuousWorld())
        {
            RequestTravel({{}, Chapter3Maps::Archive, std::nullopt});
        }
        return "옛 군용 통로로 봉인 서고에 진입하세요. 순찰대는 피하거나 Space로 비살상 제압합니다.";
    }
    if (id == CityReturn && phase >= Phase::Preservation)
    {
        if (!ContinuousWorld())
        {
            RequestTravel({{}, Chapter3Maps::Argen, WorldPosition{0.f, -2050.f}});
        }
        return "서쪽 귀환로를 따라 아르겐 시민 회랑으로 돌아가세요.";
    }
    if (phase == Phase::Memorial && id >= Ceremony && id <= Rubbing)
    {
        m_State.memorial.set(id - Ceremony);
        m_Message = site.description;
        if (m_State.memorial.all())
        {
            Advance(Phase::City,
                    "엘리안: 긁힌 자국은 원 안료보다 후대입니다. 아르겐의 원본과 대조합시다.");
        }
        return site.description + "\n" + m_Message;
    }
    if (phase == Phase::City)
    {
        if (id == Petition)
        {
            m_State.petition = true;
            return site.description;
        }
        if (id == Rations)
        {
            m_State.rations = true;
            return "회색벽 생존자 대표: 대다수는 살아왔지만 손실은 남았습니다. 보급 피해 단계 " +
                   std::to_string(m_State.supplyLoss) + " / 피난민 피해 단계 " +
                   std::to_string(m_State.refugeeLoss) +
                   (m_State.supplyLoss >= 2 ? " · 배급 부족으로 대기 줄이 길어졌습니다."
                                            : " · 제한된 구호 물자가 도착했습니다.");
        }
        if (id == Permit && m_State.petition && m_State.rations)
        {
            Advance(
                Phase::Chase,
                "열람 허가 직후 탁본이 사라졌습니다! 시장 지름길로 앞을 막고 연락책 곁에서 Space를 유지하세요.");
            return m_Message;
        }
    }
    if (phase == Phase::Chase && id == Shortcut)
    {
        m_State.shortcut = true;
        return "지름길 확보. 순환하는 연락책이 가까워질 때 Space 유지로 붙잡으세요. 공격으로 죽이지 않습니다.";
    }
    if (phase == Phase::Research)
    {
        if (id >= ManuscriptEarly && id <= ManuscriptLate)
        {
            const int selected = id - ManuscriptEarly;
            if (m_State.manuscripts == 3)
            {
                return "연대 대조 완료: 일곱 인물의 기록에서 한 이름이 사라지고 후대에도 삭제가 반복됩니다.";
            }
            if (selected == m_State.manuscripts)
            {
                ++m_State.manuscripts;
                m_Message =
                    site.description + " · 시간축 " + std::to_string(m_State.manuscripts) + "/3";
            }
            else
            {
                m_State.manuscripts = 0;
                m_Message =
                    "연대 순서가 다릅니다. 초판 → 중기 개정본 → 후대 재필사본 순으로 다시 겹치세요.";
            }
            return m_Message;
        }
        if (id >= FoundationWest && id <= FoundationEast)
        {
            return "돌 기초 옆에서 Space를 2초 유지해 석재 규격을 복원·대조하세요.";
        }
        if (id == WorkshopLedger && m_State.foundations.all())
        {
            m_State.stoneLedger = true;
            return "공방 장부: 일곱 석상이 같은 시기에 제작됐다. 일곱째는 실제 기념 대상이었다.";
        }
        if (id >= RefugeRoom && id <= LastSermon)
        {
            return "기록을 읽고 같은 필체·인장의 다음 방으로 이동하세요. 입소실 → 장례 명부 → 마지막 설교실.";
        }
    }
    if (phase == Phase::Censure && id == Censure)
    {
        Advance(
            Phase::Edric,
            "세라드: 명부 정정 뒤 토지 분쟁과 징집 거부가 발생했습니다. 사실과 공개해도 되는 사실은 다릅니다. 열람 중지·왕실 심의를 통보합니다. 아직 수배나 체포는 아닙니다.");
        return m_Message;
    }
    if (phase == Phase::Edric)
    {
        if (id == Edric)
        {
            m_State.edricHeard = true;
            return "에드릭은 절도를 사과하고 연락책을 무장 해제했다. 전쟁 전 원정 기록을 찾다가 추방됐다며 탁본과 오늘 밤 이송표를 내민다. 주장만 믿지 말고 작전 인장을 대조하자.";
        }
        if (id == TransferSeal && m_State.edricHeard)
        {
            Advance(
                Phase::Infiltration,
                "탁본과 수도원 기록의 작전 인장이 일치한다. 이송 전에 하부 통로에서 이동표와 보관 연대를 확인하자.");
            return m_Message;
        }
    }
    if (phase == Phase::Infiltration)
    {
        if (id == MovementList)
        {
            m_State.movementList = true;
            return "이동표: 북부 경계 정화 작전 · 제7부대와 정규군 · 보급 72상자. 이 표식과 수량을 찾으세요. (수량은 퍼즐용 구현값)";
        }
        if (id == StorageDate)
        {
            m_State.storageDate = true;
            return "보관 연대: 최초 아크 드래곤 교전 이전. 교전 이후 문서는 이번 원본이 아닙니다.";
        }
        if (id >= WrongDateBox && id <= OriginalBox)
        {
            if (!m_State.movementList || !m_State.storageDate)
            {
                return "먼저 압수 상자 이동표와 서고 연대를 확보하세요.";
            }
            if (id != OriginalBox)
            {
                return site.description + " · 이동표와 일치하지 않습니다. 다른 상자를 대조하세요.";
            }
            Advance(
                Phase::Interception,
                "확인: 전쟁 전에 인간 정규군과 일곱째의 군대가 야수족 영역으로 이동했다. 목적·최종 명령은 훼손돼 미확인이다. 경보! 움직이는 운반대와 승강 브레이크를 막아라.");
            ResetInterception(enemies);
            return m_Message;
        }
    }
    if (phase == Phase::Interception)
    {
        if (id == Brake)
        {
            return m_State.wheelsStopped
                       ? "브레이크 옆에서 Space 2초 유지."
                       : "먼저 이동하는 원본 운반대 곁에서 Space를 유지해 바퀴 고정장치를 끊으세요.";
        }
        if (id == Disarm)
        {
            return "세라드의 기력을 소진시킨 뒤 가까이서 Space 유지로 무장을 해제하세요. 반출 차단이 먼저입니다.";
        }
        if (id == RecoverOriginal && m_State.seradAlive && m_State.brake)
        {
            return "멈춰 있는 원본 운반대 곁에서 Space를 유지해 상자를 직접 회수하세요.";
        }
    }
    if (phase == Phase::Preservation)
    {
        if (id == ConfirmedNotice || id == OpenNotice)
        {
            m_State.notice.set(id == ConfirmedNotice ? 0 : 1);
            return site.description;
        }
        if (m_State.notice.all() && id >= CopyCommunity && id <= CopyArchive)
        {
            m_State.copies.set(id - CopyCommunity);
            return site.description + " · 공증 사본 전달. 보관자가 독립된 출구로 이동합니다.";
        }
        if (m_State.notice.all() && (id == ExtraClergy || id == ExtraGuild))
        {
            if (!m_State.extraCopy)
            {
                m_State.extraCopy = id == ExtraClergy ? 0 : 1;
            }
            return *m_State.extraCopy == 0 ? "추가 사본: 성직자 공동체에 전달했습니다."
                                           : "추가 사본: 상인 조합에 전달했습니다.";
        }
        if (id == Testimony && m_State.copies.all() && m_State.extraCopy)
        {
            if (*std::min_element(m_Delivery.begin(), m_Delivery.end()) < 4.f)
            {
                return "사본 보관자들이 출구로 떠나고 있습니다. 출발을 확인한 뒤 용사의 증언 인장을 받으세요.";
            }
            m_State.testimony = true;
            Advance(
                Phase::Hearing,
                "용사: 내 이름을 지키려고 누군가의 이름을 지웠다면 지킬 가치가 없어. 인장으로 원본의 존재와 직접 확인을 증언한다. 다음 날 왕실 연락소로 오라는 통지가 도착했다.");
            return m_Message;
        }
    }
    if (phase == Phase::Hearing || phase == Phase::Complete)
    {
        if (id == Hearing)
        {
            m_State.hearing = true;
            return "왕실 조치: 출석·심의 및 이동 보고 의무, 일부 중앙 지원·기록 접근 동결. 엘리안 해임, 에드릭 보급 제재. 광역 수배 아님. 용사의 직위·현장 긴급 지휘권 유지. 각 창구에서 조치와 인장을 확인하세요.";
        }
        if (m_State.hearing && id >= RestrictedDoor && id <= CommandSeal)
        {
            m_State.restrictions.set(id - RestrictedDoor);
            if (id == SupplyDesk && !m_State.supportClaimed)
            {
                m_State.supportClaimed = true;
                progress.Heal(20);
                inventory.AddGold(10);
                return "지급 품목 축소: 치료품·여비만 지급(체력 +20, 금화 +10, 1회). 장비·추가 호송 지원은 동결되었습니다.";
            }
            return site.description;
        }
        if (id == FieldPermit && m_State.hearing && m_State.restrictions.all())
        {
            Advance(
                Phase::Complete,
                "챕터 3 완료 · 왕실 심의 중 / 제한 현장 조사 승인 / 일부 지원 동결. 용사의 공식 직위·긴급 지휘권 유지. 다음 단서: 야수족 영역의 원정 좌표. 전쟁의 발단은 아직 미확인입니다.");
            return m_Message;
        }
    }
    return "현재 진행: " + Objective();
}

void Chapter3::Retry(EnemySystem& enemies)
{
    if (m_State.phase == Chapter3Phase::Interception)
    {
        if (m_State.map == Chapter3Maps::Archive)
        {
            ResetInterception(enemies);
        }
        else
        {
            // Retry elsewhere must not clear that region or leave a stale archive boss.
            m_ResetCombat = true;
        }
    }
    m_Hold = m_Alarm = m_ParryWindow = 0.f;
    m_HoldAction = 0;
    m_WasHeld = false;
    m_Impact = {};
    ClearTravel();
    m_Message = "조사·공증 기록은 유지됩니다. 진행 중인 반출 저지전은 처음부터 다시 시작합니다.";
}

ChapterImpact Chapter3::ConsumeImpact()
{
    const auto result = m_Impact;
    m_Impact = {};
    return result;
}

bool Chapter3::ControlLocked() const
{
    return false;
}

bool Chapter3::Complete() const
{
    return m_State.phase == Chapter3Phase::Complete;
}

const std::string& Chapter3::Message() const
{
    return m_Message;
}

ChapterFlags Chapter3::ExportFlags() const
{
    const bool research = m_State.phase >= Chapter3Phase::Censure;
    return {{"C03_SEVENTH_EXISTED", research},
            {"C03_REPEATED_ERASURE", research},
            {"C03_IDEOLOGY_CHANGED", research},
            {"C03_PREWAR_EXPEDITION", m_State.phase >= Chapter3Phase::Interception},
            {"C03_SERAD_SURVIVED", m_State.seradAlive},
            {"C03_EXPORT_STOPPED", m_State.brake && m_State.original},
            {"C03_ORIGINAL_RECOVERED", m_State.original},
            {"C03_DISTRIBUTED_COPIES", static_cast<int>(m_State.copies.count())},
            {"C03_EXTRA_CUSTODIAN", m_State.extraCopy ? *m_State.extraCopy + 1 : 0},
            {"C03_HERO_TESTIMONY", m_State.testimony},
            {"C03_ROYAL_REVIEW", m_State.hearing},
            {"C03_MOVEMENT_REPORT_REQUIRED", m_State.hearing},
            {"C03_SUPPORT_FROZEN", m_State.hearing},
            {"C03_ELIAN_DISMISSED", m_State.hearing},
            {"C03_EDRIC_SANCTIONED", m_State.hearing},
            {"C03_HERO_COMMAND_RETAINED", m_State.hearing},
            {"C03_LIMITED_FIELD_PERMIT", Complete()},
            {"C03_EXPEDITION_COORDINATES", Complete()},
            {"C03_WANTED", 0},
            {"C03_WAR_ORIGIN_CONFIRMED", 0},
            {"C03_COMPLETE", Complete()}};
}
