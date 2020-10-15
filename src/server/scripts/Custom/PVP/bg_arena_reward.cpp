#include "ScriptMgr.h"
#include "Player.h"
#include "Battleground.h"

enum Quests
{
    QUEST_WIN_10_ARENA = 80000,
    QUEST_WIN_50_ARENA = 80001,
    QUEST_WIN_5_BG = 80002,
    QUEST_WIN_50_BG = 80003,
    QUEST_KILL_100_PLAYERS = 80004,
    QUEST_HALLOWEEN_WIN_5_2V2 = 100005
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

            if (bg->GetArenaType() == ARENA_TYPE_2v2 && player->GetQuestStatus(QUEST_HALLOWEEN_WIN_5_2V2) == QUEST_STATUS_INCOMPLETE)
            {
                player->KilledMonsterCredit(QUEST_HALLOWEEN_WIN_5_2V2, 0);
            }
            

            if (player->GetQuestStatus(QUEST_WIN_50_ARENA) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_WIN_50_ARENA, 0);
        }

        if (!bg->isArena() && bg->isBattleground() && bgTeamId == winnerTeamId)
        {
            if (player->GetQuestStatus(QUEST_WIN_5_BG) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_WIN_5_BG, 0);

            if (player->GetQuestStatus(QUEST_WIN_50_BG) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_WIN_50_BG, 0);
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
    }
};


void AddSC_ArenaAndBgRewards()
{
    new ArenaAndBgRewards();
    new ArenaAndBgRewardsPvPKills();
}
