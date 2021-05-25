/*
* Copyright (C) 2020 WoW Amnesia - Amnesyx
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ScriptMgr.h"
#include "Player.h"
#include "Battleground.h"
#include "Group.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "BanManager.h"

enum Quests
{
    QUEST_WIN_10_ARENA = 80000,
    QUEST_WIN_50_ARENA = 80001,
    QUEST_WIN_5_BG = 80002,
    QUEST_WIN_50_BG = 80003,
    QUEST_KILL_100_PLAYERS = 80004,
    QUEST_WIN_10_ARENA_2V3_3V3 = 80007,
    QUEST_PVP_WIN_3_BG = 80010,
    QUEST_PVP_WIN_20_BG = 230007,
    QUEST_PVP_WIN_10_ARENA = 80012,
    QUEST_PVP_WIN_10_ARENA_2V2_3V3 = 80013,
    QUEST_PVP_KILL_2000_PLAYERS_IN_BG_ARENA = 80014,
    QUEST_PVP_WIN_100_BG = 80015,
    QUEST_DEATHMATCH_AREA_BEGIN = 80016,
    QUEST_DEATHMATCH_AREA_END = 80025,
    QUEST_DEATHMATCH_AREA_NPC_CREDIT = 80016,
    QUEST_CLIMBING_CIRCUIT_IMPOSSIBLE = 60012,
    QUEST_PVP_WIN_1_ARATHIBASHIN = 80030,
    QUEST_PVP_WIN_1_EYEOFTHESTORM = 80031,
};

enum EmblemEntries
{
    EMBLEM_OF_FROST_ENTRY = 49426,
    EMBLEM_OF_TRIUMPH_ENTRY = 47241,
    ETHEREAL_CREDIT_ENTRY = 38186,
    AMNESIA_COIN_ENTRY = 32569,
    GLADIATOR_CHEST_ENTRY = 32544
};

enum ArenaAndBgRewardVariables
{
    AREA_DEATHMATCH = 3817,
    MAP_DEATHMATCH = 13,
    MAP_TANARIS = 1,

    GAME_EVENT_DEATHMATCH = 77
};

class ArenaAndBgRewards : public BGScript
{
public:
    ArenaAndBgRewards() : BGScript("ArenaAndBgRewards") {}

    void OnBattlegroundEndReward(Battleground* bg, Player* player, TeamId winnerTeamId) override
    {
        TeamId bgTeamId = player->GetBgTeamId();

        if (bg->isArena() && bg->isRated() && bgTeamId == winnerTeamId)
        {
            if (player->GetQuestStatus(QUEST_WIN_10_ARENA) == QUEST_STATUS_INCOMPLETE) // Quest 10 Arena
                player->KilledMonsterCredit(QUEST_WIN_10_ARENA, 0);

            if (player->GetQuestStatus(QUEST_WIN_50_ARENA) == QUEST_STATUS_INCOMPLETE) // Quest 50 Arena
                player->KilledMonsterCredit(QUEST_WIN_50_ARENA, 0);

            if (player->GetQuestStatus(QUEST_PVP_WIN_10_ARENA) == QUEST_STATUS_INCOMPLETE) // Quest PVP 10 Arena
                player->KilledMonsterCredit(QUEST_PVP_WIN_10_ARENA, 0);

            uint32 rnd = urand(0, 100);
            switch (bg->GetArenaType())
            {
            case ARENA_TYPE_2v2: 
                if (rnd < 20) player->AddItem(GLADIATOR_CHEST_ENTRY, 1); // 20% Gladiator's Chest 2v2

                if (player->GetQuestStatus(QUEST_WIN_10_ARENA_2V3_3V3) == QUEST_STATUS_INCOMPLETE) // Quest 10 Arena 2v2-3v3
                    player->KilledMonsterCredit(QUEST_WIN_10_ARENA_2V3_3V3, 0);

                if (player->GetQuestStatus(QUEST_PVP_WIN_10_ARENA_2V2_3V3) == QUEST_STATUS_INCOMPLETE) // Quest PVP 10 Arena 2v2-3v3
                    player->KilledMonsterCredit(QUEST_PVP_WIN_10_ARENA_2V2_3V3, 0);       

                break;
            case ARENA_TYPE_3v3: 
                if (rnd < 25) player->AddItem(GLADIATOR_CHEST_ENTRY, 1); // 25% Gladiator's Chest 3v3

                if (player->GetQuestStatus(QUEST_WIN_10_ARENA_2V3_3V3) == QUEST_STATUS_INCOMPLETE) // Quest 10 Arena 2v2-3v3
                    player->KilledMonsterCredit(QUEST_WIN_10_ARENA_2V3_3V3, 0);

                if (player->GetQuestStatus(QUEST_PVP_WIN_10_ARENA_2V2_3V3) == QUEST_STATUS_INCOMPLETE) // Quest PVP 10 Arena 2v2-3v3
                    player->KilledMonsterCredit(QUEST_PVP_WIN_10_ARENA_2V2_3V3, 0);

                break;
            case ARENA_TYPE_5v5: 
                if (rnd < 10) player->AddItem(GLADIATOR_CHEST_ENTRY, 1); // 10% Gladiator's Chest 1v1
                break;
            }
        }

        if (!bg->isArena() && bg->isBattleground() && bgTeamId == winnerTeamId)
        {
            if (player->GetQuestStatus(QUEST_WIN_5_BG) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_WIN_5_BG, 0);

            if (player->GetQuestStatus(QUEST_WIN_50_BG) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_WIN_50_BG, 0);

            if (player->GetQuestStatus(QUEST_PVP_WIN_3_BG) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_PVP_WIN_3_BG, 0);

            if (player->GetQuestStatus(QUEST_PVP_WIN_20_BG) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_PVP_WIN_20_BG, 0);

            if (player->GetQuestStatus(QUEST_PVP_WIN_100_BG) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(QUEST_PVP_WIN_100_BG, 0);

            switch (player->GetZoneId())
            {
            case 3358: // Arathi Basin
                if (player->GetQuestStatus(QUEST_PVP_WIN_1_ARATHIBASHIN) == QUEST_STATUS_INCOMPLETE)
                    player->KilledMonsterCredit(QUEST_PVP_WIN_1_ARATHIBASHIN, 0);
                break;
            case 3820: // Eye of the Storm
                if (player->GetQuestStatus(QUEST_PVP_WIN_1_EYEOFTHESTORM) == QUEST_STATUS_INCOMPLETE)
                    player->KilledMonsterCredit(QUEST_PVP_WIN_1_EYEOFTHESTORM, 0);
                break;
            default:
                break;
            }

            // First RDF Win of the day
            if (!player->GetRandomWinner())
            {
                player->AddItem(ETHEREAL_CREDIT_ENTRY, 5);
            }

            // Gladiator's Chest
            player->AddItem(GLADIATOR_CHEST_ENTRY, 1);
        }
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

        if (killer->GetMap() && killer->GetMap()->IsBattlegroundOrArena())
        {
            if (killer->GetQuestStatus(QUEST_PVP_KILL_2000_PLAYERS_IN_BG_ARENA) == QUEST_STATUS_INCOMPLETE)
                killer->KilledMonsterCredit(QUEST_PVP_KILL_2000_PLAYERS_IN_BG_ARENA, 0);
        }

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

        // Deathmatch area quests
        if (killer->GetAreaId() == AREA_DEATHMATCH && sGameEventMgr->IsActiveEvent(GAME_EVENT_DEATHMATCH)) // Testing with event active
        { 
            for (uint32 questId = QUEST_DEATHMATCH_AREA_BEGIN; questId <= QUEST_DEATHMATCH_AREA_END; questId++) {
                if (killer->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                    killer->KilledMonsterCredit(QUEST_DEATHMATCH_AREA_NPC_CREDIT, 0);
            }
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

class ClimbingCircuitBan : public CreatureScript
{
public:

    ClimbingCircuitBan() : CreatureScript("ClimbingCircuitBan") { }

    bool OnGossipHello(Player* player, Creature* /*creature*/) override
    {
        if (!player)
            return true;

        // Ban player for 3 days
        //if (quest->GetQuestId() == QUEST_CLIMBING_CIRCUIT_IMPOSSIBLE)
        //{
            std::string accountName;

            if (player->GetSession()
                && AccountMgr::IsPlayerAccount(player->GetSession()->GetSecurity())
                && AccountMgr::GetName(player->GetSession()->GetAccountId(), accountName))
            {
                BanReturn banReturn;
                banReturn = sBan->BanAccount(accountName, "3d", "FlyHack Climbing Circuit Impossible", "AntiHack System Climbing");
            }
        //}
        return true;
    }
    
    struct ClimbingCircuitBanAI : public ScriptedAI
    {
        ClimbingCircuitBanAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new ClimbingCircuitBanAI(creature);
    }
};


void AddSC_ArenaAndBgRewards()
{
    new ArenaAndBgRewards();
    new ArenaAndBgRewardsPvPKills();
    new ClimbingCircuitBan();
}
