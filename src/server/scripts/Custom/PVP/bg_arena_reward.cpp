#include "ScriptMgr.h"
#include "Player.h"
#include "Battleground.h"
#include "Group.h"

enum Quests
{
    QUEST_WIN_10_ARENA = 80000,
    QUEST_WIN_50_ARENA = 80001,
    QUEST_WIN_5_BG = 80002,
    QUEST_WIN_50_BG = 80003,
    QUEST_KILL_100_PLAYERS = 80004,
};

enum EmblemEntries
{
    EMBLEM_OF_FROST_ENTRY = 49426,
    EMBLEM_OF_TRIUMPH_ENTRY = 47241,
    ETHEREAL_CREDIT_ENTRY = 38186,
    AMNESIA_COIN_ENTRY = 32569,
    GLADIATOR_CHEST_ENTRY = 32544
};

class ArenaAndBgRewards : public BGScript
{
public:
    ArenaAndBgRewards() : BGScript("ArenaAndBgRewards") {}

    void OnBattlegroundEndReward(Battleground* bg, Player* player, TeamId winnerTeamId) override
    {
        TeamId bgTeamId = player->GetBgTeamId();
        uint32 RewardCount = 0;

        if (bg->isArena() && bg->isRated() && bgTeamId == winnerTeamId)
        {
            if (player->GetQuestStatus(QUEST_WIN_10_ARENA) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_WIN_10_ARENA, 0);

            if (player->GetQuestStatus(QUEST_WIN_50_ARENA) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_WIN_50_ARENA, 0);

            uint32 rnd = urand(0, 100);
            switch (bg->GetArenaType())
            {
            case ARENA_TYPE_2v2: // 20% Gladiator's Chest 2v2
                if (rnd < 20) player->AddItem(GLADIATOR_CHEST_ENTRY, 1);
                break;
            case ARENA_TYPE_3v3: // 25% Gladiator's Chest 3v3
                if (rnd < 25) player->AddItem(GLADIATOR_CHEST_ENTRY, 1);
                break;
            case ARENA_TYPE_5v5: // 10% Gladiator's Chest 1v1
                if (rnd < 10) player->AddItem(GLADIATOR_CHEST_ENTRY, 1);
                break;
            }
        }

        if (!bg->isArena() && bg->isBattleground() && bgTeamId == winnerTeamId)
        {
            if (player->GetQuestStatus(QUEST_WIN_5_BG) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_WIN_5_BG, 0);

            if (player->GetQuestStatus(QUEST_WIN_50_BG) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_WIN_50_BG, 0);

            // First RDF Win of the day
            if (!player->GetRandomWinner())
            {
                player->AddItem(ETHEREAL_CREDIT_ENTRY, 5);
            }

            // Gladiator's Chest
            player->AddItem(GLADIATOR_CHEST_ENTRY, 1);

        }
       
        // Reward ItemID
        //uint32 itemWSG = CONF_INT(conf::BG_REWARD_ITEMID_WSG);
        //uint32 itemARATHI = CONF_INT(conf::BG_REWARD_ITEMID_ARATHI);
        //uint32 itemALTERAC = CONF_INT(conf::BG_REWARD_ITEMID_ALTERAC);
        //uint32 itemISLE = CONF_INT(conf::BG_REWARD_ITEMID_ISLE);
        //uint32 itemANCIENT = CONF_INT(conf::BG_REWARD_ITEMID_ANCIENTS);
        //uint32 itemEYE = CONF_INT(conf::BG_REWARD_ITEMID_EYE);

        // Reward Count
        //uint32 WinnerCount = CONF_INT(conf::BG_REWARD_WIN_TEAM_ITEM_COUNT);
        //uint32 LoserCount = CONF_INT(conf::BG_REWARD_LOS_TEAM_ITEM_COUNT);

        //TeamId bgTeamId = player->GetBgTeamId();
        //uint32 RewardCount = 0;

        //bgTeamId == winnerTeamId ? (RewardCount = WinnerCount) : (RewardCount = LoserCount);

        //switch (player->GetZoneId())
        //{
        //case 3277: // Warsong Gulch
        //    player->AddItem(itemWSG, RewardCount);
        //    break;
        //case 3358: // Arathi Basin
        //    player->AddItem(itemARATHI, RewardCount);
        //    break;
        //case 3820: // Eye of the Storm
        //    player->AddItem(itemEYE, RewardCount);
        //    break;
        //case 4710: // Isle of Conquest
        //    player->AddItem(itemISLE, RewardCount);
        //    break;
        //case 4384: // Strand of the Ancients
        //    player->AddItem(itemANCIENT, RewardCount);
        //    break;
        //case 2597: // Alterac Valley
        //    player->AddItem(itemALTERAC, RewardCount);
        //    break;
        //default:
        //    break;
        //}
    }
};

class ArenaAndBgRewardsPvPKills : public PlayerScript
{
public:
    ArenaAndBgRewardsPvPKills() : PlayerScript("ArenaAndBgRewardsPvPKills") { }

    void OnPVPKill(Player* killer, Player* victim)
    {
        //killer->KilledPlayerCredit();
        if (killer->GetQuestStatus(QUEST_KILL_100_PLAYERS) == QUEST_STATUS_INCOMPLETE)
            killer->KilledMonsterCredit(QUEST_KILL_100_PLAYERS, 0);

        uint32 rnd = urand(0, 100);

        switch (killer->GetZoneId())
        {
        case 3277: // Warsong Gulch
        case 3358: // Arathi Basin
        case 3820: // Eye of the Storm
        case 4710: // Isle of Conquest
        case 4384: // Strand of the Ancients
        case 2597: // Alterac Valley
            if (rnd < 10) GiveItemToAllGroup(killer, victim, EMBLEM_OF_TRIUMPH_ENTRY, 1);   // 10% of obtaining 1 Emblem of Triumph
            if (rnd < 5) GiveItemToAllGroup(killer, victim, ETHEREAL_CREDIT_ENTRY, 1);      // 5% of obtaining 1 Ethereal Credit
            if (rnd < 2) GiveItemToAllGroup(killer, victim, AMNESIA_COIN_ENTRY, 1);         // 2% of obtaining 1 Amnesia Coin
            break;
        default:
            break;
        }
    }

    void GiveItemToAllGroup(Player* player, Player* victim, uint32 itemEntry, uint32 quantity)
    {
        player->AddItem(itemEntry, quantity);

        Group* grp = player->GetGroup();
        if (grp)
        {
            for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
                if (Player* member = itr->GetSource())
                    if (member->IsInMap(player) && player->GetGUID() != member->GetGUID() && member->IsAtGroupRewardDistance(victim))
                    {
                        member->AddItem(itemEntry, quantity);
                    }
        }
    }
};


void AddSC_ArenaAndBgRewards()
{
    new ArenaAndBgRewards();
    new ArenaAndBgRewardsPvPKills();
}
