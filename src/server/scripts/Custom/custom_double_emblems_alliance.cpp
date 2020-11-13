#include "DBCEnums.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "DBCStores.h"
#include "Language.h"
#include "Player.h"
#include "Group.h"
#include "GroupMgr.h"
#include "InstanceScript.h"

enum EmblemEntries
{
    EMBLEM_OF_FROST_ENTRY = 49426,
    ETHEREAL_CREDIT_ENTRY = 38186
};

enum Emblem_MapIDs
{
    MAP_ICC = 631,
    MAP_RS = 724,
    MAP_ULDUAR = 603,
    MAP_TOC = 649
};

struct BossReward
{
    BossReward(uint32 entry, uint32 recompence) : entry(entry), recompence(recompence) { }

    uint32 entry;
    uint32 recompence;
};
typedef std::unordered_map<uint32, std::vector<BossReward>> BossEntriesEtherealCredit;

BossEntriesEtherealCredit boss_list_ethereal_credit = {
    {MAP_ICC, {
        { 38431, 5 },    // PP 25NM
        { 38586, 5 },    // PP 25HC
        { 38265, 5 },    // Sindra 25NM
        { 38267, 5 },    // Sindra 25HC
        { 39166, 10 },   // LK 25NM
        { 39168, 15 },   // LK 25HC
    } },
    {MAP_RS, {
        { 39864, 10 },   // Halion 25NM
        { 39945, 15 },   // Halion 25HC
    } },
    {MAP_ULDUAR, {
        { 33885, 5 },    // Desarmador XA - 002 25
        { 34175, 5 },    // Auriaya 25
        { 33955, 10 },   // Yogg-Saron 25
    } },
    {MAP_TOC, {
        { 34566, 15 },    // Anub'arak 25NM
        { 35616, 15 },    // Anub'arak 25HC
    } }
};

class custom_double_emblems_alliance : public PlayerScript
{
public:
    custom_double_emblems_alliance() : PlayerScript("custom_double_emblems_alliance") { }

    void OnCreatureKill(Player* player, Creature* boss) override
    {
        if (!player)
            return;

        // Icecrown Citadel & Rubi Sanctum
        if (boss->GetMap()->IsRaid() && boss->getLevel() > 80 && boss->IsDungeonBoss())
        {
            if (player->GetTeamId() == TEAM_ALLIANCE && (player->GetMapId() == 631 || player->GetMapId() == 724))
            {
                GiveEmblemsToAllGroup(player, EMBLEM_OF_FROST_ENTRY, 4);
            }

            if (player->GetMap()->Is25ManRaid() && player->GetTeamId() == TEAM_ALLIANCE)
            {
                FindAndGiveEtherealCredits(player, boss->GetEntry());
            }
        }
    }

    void GiveEmblemsToAllGroup(Player* player, uint32 emblemEntry, uint32 quantity)
    {
        player->AddItem(emblemEntry, quantity);

        Group* grp = player->GetGroup();
        if (grp)
        {
            for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
                if (Player* member = itr->GetSource())
                    if (member->IsInMap(player) && player->GetGUID() != member->GetGUID())
                    {
                        member->AddItem(emblemEntry, quantity);
                    }
        }
    }

    void FindAndGiveEtherealCredits(Player* player, uint32 bossEntry)
    {
        auto zone_bosses = boss_list_ethereal_credit.find(player->GetMapId());
        if (zone_bosses != boss_list_ethereal_credit.end())
        {
            std::vector<BossReward> bosses = zone_bosses->second;
            if (bosses.empty()) return;

            for (auto const& bossInfo : bosses)
            {
                if (bossInfo.entry == bossEntry)
                {
                    GiveEmblemsToAllGroup(player, ETHEREAL_CREDIT_ENTRY, bossInfo.recompence);
                    break;
                }
            }
        }
    }

};

void AddSC_custom_double_emblems_alliance()
{
    new custom_double_emblems_alliance();
}
