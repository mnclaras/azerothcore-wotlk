#include "DBCEnums.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "DBCStores.h"
#include "Language.h"
#include "Player.h"

enum Titles
{
    /* Alliance */
    PRIVATE = 1,
    CORPORAL = 2,
    SERGEANT = 3,
    MASTER_SERGEANT = 4,
    SERGEANT_MAJOR = 5,
    KNIGHT = 6,
    KNIGHT_LIEUTENANT = 7,
    KNIGHT_CAPTAIN = 8,
    KNIGHT_CHAMPION = 9,
    LIEUTENANT_COMMANDER = 10,
    COMMANDER = 11,
    MARSHAL = 12,
    FIELD_MARSHAL = 13,
    GRAND_MARSHAL = 14,
    OF_THE_ALLIANCE = 126, // Recompesa de logro 100.000 muertes con honor

    /* Horde */
    SCOUT = 15,
    GRUNT = 16,
    SERGEANT_H = 17,
    SENIOR_SERGEANT = 18,
    FIRST_SERGEANT = 19,
    STONE_GUARD = 20,
    BLOOD_GUARD = 21,
    LEGIONNAIRE = 22,
    CENTURION = 23,
    CHAMPION = 24,
    LIEUTENANT_GENERAL = 25,
    GENERAL = 26,
    WARLORD = 27,
    HIGH_WARLORD = 28,
    OF_THE_HORDE = 127 // Recompesa de logro 100.000 muertes con honor
};

// Retail-like Data
enum Kills
{
    RANK_1 = 15,
    RANK_2 = 2000,
    RANK_3 = 5000,
    RANK_4 = 10000,
    RANK_5 = 15000,
    RANK_6 = 20000,
    RANK_7 = 25000,
    RANK_8 = 30000,
    RANK_9 = 35000,
    RANK_10 = 40000,
    RANK_11 = 45000,
    RANK_12 = 50000,
    RANK_13 = 55000,
    RANK_14 = 60000,
    //RANK_15                 = ????? // Discontinued
    SPECIAL_100K = 100000
};

class HKTitles : public PlayerScript
{
public:
    HKTitles() : PlayerScript("HKTitles") { }

    void OnPVPKill(Player* player, Player* /*target*/)
    {
        if (!player)
            return;

        uint32 kills = player->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS);

        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? GRAND_MARSHAL : HIGH_WARLORD), kills < RANK_14);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? FIELD_MARSHAL : WARLORD), kills < RANK_13);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? MARSHAL : GENERAL), kills < RANK_12);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? COMMANDER : LIEUTENANT_GENERAL), kills < RANK_11);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? LIEUTENANT_COMMANDER : CHAMPION), kills < RANK_10);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? KNIGHT_CHAMPION : CENTURION), kills < RANK_9);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? KNIGHT_CAPTAIN : LEGIONNAIRE), kills < RANK_8);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? KNIGHT_LIEUTENANT : BLOOD_GUARD), kills < RANK_7);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? KNIGHT : STONE_GUARD), kills < RANK_6);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? SERGEANT_MAJOR : FIRST_SERGEANT), kills < RANK_5);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? MASTER_SERGEANT : SENIOR_SERGEANT), kills < RANK_4);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? SERGEANT : SERGEANT_H), kills < RANK_3);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? CORPORAL : GRUNT), kills < RANK_2);
        player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? PRIVATE : SCOUT), kills < RANK_1);

        if (kills < SPECIAL_100K)
            player->SetTitle(sCharTitlesStore.LookupEntry(player->GetTeamId() == TEAM_ALLIANCE ? OF_THE_ALLIANCE : OF_THE_HORDE), true);

        if (!player->HasTitle(player->GetInt32Value(PLAYER_CHOSEN_TITLE)))
            player->SetUInt32Value(PLAYER_CHOSEN_TITLE, 0);
    }
};

void AddSC_battleground_titles_pvp()
{
    new HKTitles();
}
