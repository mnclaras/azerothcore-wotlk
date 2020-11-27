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
#include "ScriptedCreature.h"
#include "ObjectMgr.h"
#include "Group.h"


enum ZOMBIE_DATA
{
    NPC_START_GAME = 125,
    NPC_START_NEXT_WAVE = 126,
    NPC_START_NEXT_ROUND = 127,
    NPC_WAIT_FOR_END_ROUND = 128,
    NPC_SPAWN_LOOP = 151,
    NPC_END_GAME = 153,
};

uint32 ZOMBIE_NPCS[7] = { 0, 434,435,436,437,440,441 };

const Position Spawn_T1 = { -10953.3662f,   -1997.1374f,    79.9735f,   3.0059f };
const Position Spawn_T2 = { -10974.728516f, -1972.700073f, 79.973518f, 5.158750f };


bool closeGossipNotify(Player* player, std::string message)
{
    player->PlayerTalkClass->SendCloseGossip();
    player->GetSession()->SendNotification(message.c_str());
    return true;
}

class zombie_bg_master : public CreatureScript
{
public:
    zombie_bg_master() : CreatureScript("zombie_bg_master") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->IsInCombat())
        {
            return closeGossipNotify(player, "You can't start the zombie event while being in combat.");
        }

        if (!player->GetGroup() || player->GetGroup() == NULL)
        {
            return closeGossipNotify(player, "You must be the group leader.");
        }

        Group* grp = player->GetGroup();

        //if (grp->GetMembersCount() > 10 || grp->GetMembersCount() < 5)
        //{
        //	return closeGossipNotify(player, "You must be the group leader in a party/raid with 5-10 players.");
        //}

        if (grp->GetLeaderGUID() != player->GetGUID())
        {
            return closeGossipNotify(player, "Only the party leader can activate the Zombie Event!");
        }

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Teleport me to the Zombie Attack event!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "No thanks.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        Group* grp = player->GetGroup();
        switch (uiAction)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            grp->ConvertToRaid();
            player->TeleportTo(532, -10966.888f, -2011.3127f, 79.98f, 1.37f);
            break;

        case GOSSIP_ACTION_INFO_DEF + 2:
            player->PlayerTalkClass->SendCloseGossip();
            break;
        }
        return true;
    }
};


class zombie_bg_master_endgame : public CreatureScript
{
public:
    zombie_bg_master_endgame() : CreatureScript("zombie_bg_master_endgame") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        //if(player->isInCombat())
        //	return false;
        //if(player->GetGroup()->GetLeaderGUID() == player->GetGUID()){
        //	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "End the Zombie Event!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        //	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Nevermind.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);		
        //	player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());
        //}
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        //player->PlayerTalkClass->ClearMenus();
        //Group* grp = player->GetGroup();
        //switch(uiAction)
        //{
        //case GOSSIP_ACTION_INFO_DEF+1:
        //		creature->MonsterYell("The group leader ended the zombie event!", 0, 0);
        //		creature->SummonCreature(NPC_END_GAME, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*1000); 
        //		creature->DespawnOrUnsummon();
        //	break;

        //case GOSSIP_ACTION_INFO_DEF+2:
        //	player->PlayerTalkClass->SendCloseGossip();
        //	break;
        //}
        return true;
    }
};

class zombie_bg_master_final : public CreatureScript
{
public:
    zombie_bg_master_final() : CreatureScript("zombie_bg_master_final") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (!player->GetGroup())
            return false;
        if (player->GetGroup() == NULL)
            return false;
        if (player->GetGroup()->GetLeaderGUID() == player->GetGUID())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Summon group to the Zombie Attack event!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            if (player->GetGroup()->GetLeaderGUID() == player->GetGUID())
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Start the event!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Nevermind", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage("You must be the group leader.");
            while (creature->SelectNearestPlayer(280.0f) != NULL)
            {
                Player* plr = creature->SelectNearestPlayer(280.0f);
                if (plr)
                {
                    plr->ResurrectPlayer(60, false);
                    plr->TeleportTo(534, 5541.770020f, -3783.469971f, 1594.910034f, 2.883010f);
                }
            }
        }

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {

        player->PlayerTalkClass->ClearMenus();
        Group* grp = player->GetGroup();
        if (grp) {
            GroupReference* grpRef = grp->GetFirstMember();
            if (grp->GetLeaderGUID() == player->GetGUID())
                GroupReference grpRef = player->GetGroupRef();

            switch (uiAction)
            {
            case GOSSIP_ACTION_INFO_DEF + 1:

                for (grpRef; grpRef != NULL; grpRef = grpRef->next())
                {
                    Player* groupMember = grpRef->GetSource();
                    ChatHandler(player->GetSession()).PSendSysMessage("Sending summon request to...");
                    if (!groupMember)
                        continue;
                    if (groupMember->GetGUID() == player->GetGUID())
                        break;
                    player->SetSelection(groupMember->GetGUID());
                    player->CastSpell(groupMember, 7720, true);
                    ChatHandler(player->GetSession()).PSendSysMessage("%s", groupMember->GetName().c_str());
                }
                break;

            case GOSSIP_ACTION_INFO_DEF + 2:
                creature->MonsterYell("Event starts in 30 seconds!", 0, 0);

                /* spawn NPC_START_GAME to despawn after 30 seconds and start the game */
                creature->SummonCreature(NPC_START_GAME, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30 * 1000);
                /* the main NPC will not be seen again by the group so teleport him away */
                creature->UpdatePosition(-10989.9003f, -1940.1300f, 93.9107f, 4.8345f, true);
                break;

            case GOSSIP_ACTION_INFO_DEF + 3:
                creature->MonsterYell("The group leader ended the zombie event!", 0, 0);
                creature->SummonCreature(NPC_END_GAME, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 500);
                break;

            case GOSSIP_ACTION_INFO_DEF + 4:
                //NPC_END_GAME

                break;
            }
        }
        player->PlayerTalkClass->SendCloseGossip();
        return true;
    }

    struct zombie_bg_master_finalAI : public ScriptedAI
    {
        zombie_bg_master_finalAI(Creature* c) : ScriptedAI(c), Summons(me) {}

        // Time between spawn of waves for each team
        double spawnTimer;
        // Adds summoned every spawnTimer seconds
        uint8 addsPerWave;
        SummonList Summons;

        bool inZombieGame;

        Player* zombieLeader;
        Group* playerGroup;
        GameObject* zombie_chest_loot;

        void JustSummoned(Creature* summon)
        {
            if (summon) {
                uint32 entry = summon->GetEntry();
                Summons.Summon(summon);
                if (entry >= 125 && entry <= 128 || entry == 151 || entry == 153)
                {

                }
                else {
                    Player* selPlr = summon->SelectNearestPlayer(200);
                    if (selPlr)
                        summon->Attack(selPlr, false);
                    else
                    {
                        /* cant find a player to select */
                    }
                }
            }
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            uint32 summonID = summon->GetEntry();

            if (summonID == NPC_START_GAME)
            {
                /* called at the beg of the game */
                /* reset all vars because this is new instance of game */
                resetZombieEvent();

                Player* selPlr = me->SelectNearestPlayer(280.0f);
                if (!selPlr)
                {
                    reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("!selPlr return", 0, 0); return;
                }
                if (selPlr == NULL)
                {
                    reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("selPlr == NULL return", 0, 0); return;
                }
                if (!selPlr->GetGroup())
                {
                    reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("!Group return", 0, 0); return;
                }
                if (selPlr->GetGroup() == NULL)
                {
                    reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("Group == NULL return", 0, 0); return;
                }
                if (playerGroup = selPlr->GetGroup())
                {
                    if (zombieLeader = sObjectAccessor->FindPlayer(playerGroup->GetLeaderGUID()))
                    {
                    }
                    else {
                        reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("LeaderGUID == NULL return", 0, 0); return;
                    }
                }
                else
                {
                    reviveAndTeleportOut(); resetZombieEvent(); me->MonsterYell("playerGroup == NULL return", 0, 0); return;
                }

                /* Set the ZombieGame to true so they cant keep starting the game */
                inZombieGame = true;

                me->MonsterYell("Event is starting now!", 0, 0);
                sendMessageToGroup("Event is starting now!");
                me->SetVisible(false);

                addsPerWave = getZombieSpawnCountPerWave();		/* calc how many adds have to */
                /* spawn */
                spawnZombies(me, addsPerWave);
                me->SummonCreature(NPC_SPAWN_LOOP, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, spawnTimer * 1000);
            }

            if (inZombieGame == false) { return; }
            char buffer[150];
            switch (summonID)
            {
            case NPC_START_GAME:
            {
                /* called at the beg of the game */
                break;
            }
            case NPC_SPAWN_LOOP:
            {
                bool teamOneDead = isGroupDead(1);
                bool teamTwoDead = isGroupDead(2);

                if (!teamOneDead && !teamTwoDead)
                {
                    if (spawnTimer > 0.5) spawnTimer -= 0.1;
                    addsPerWave = getZombieSpawnCountPerWave();		/* calc how many adds have to */

                    sprintf(buffer, "Next spawn of %u adds in %f seconds!", addsPerWave, spawnTimer);
                    me->MonsterYell(buffer, 0, 0);
                    sendMessageToGroup(buffer);

                    spawnZombies(me, addsPerWave);
                    me->SummonCreature(NPC_SPAWN_LOOP, -10989.9003f, -1940.1300f, 93.9107f, 4.8345f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, spawnTimer * 1000);
                }
                else
                {
                    reviveAndTeleportOut();
                    me->SetVisible(true);
                    Summons.DespawnAll();
                    Summons.RemoveNotExisting();
                    resetZombieEvent();

                    if (teamOneDead) sprintf(buffer, "Team 2 wins!");
                    else sprintf(buffer, "Team 1 wins!");
                    me->MonsterYell(buffer, 0, 0);
                    sendMessageToGroup(buffer);
                }
                break;
            }
            case NPC_END_GAME:
            {
                reviveAndTeleportOut();
                me->SetVisible(true);
                Summons.DespawnAll();
                Summons.RemoveNotExisting();
                resetZombieEvent();
                me->UpdatePosition(-10989.9003f, -1940.1300f, 93.9107f, 4.8345f, true);
                break;
            }
            default:
                break;
            }
        }

        bool isGroupDead(uint8 team)
        {
            uint32 playersDeadT1 = 0;
            uint32 playersDeadT2 = 0;

            Player* GroupMember;
            const Group::MemberSlotList members = playerGroup->GetMemberSlots();
            for (Group::member_citerator itr = members.begin(); itr != members.end(); ++itr)
            {
                GroupMember = (ObjectAccessor::GetPlayer(*me, itr->guid));
                if (itr->group == 1 || itr->group == 2) {
                    if (!GroupMember || !GroupMember->GetSession()) { playersDeadT1++; continue; }
                    if (GroupMember->isDead()) { playersDeadT1++; continue; }
                }
                else if (itr->group == 3 || itr->group == 4) {
                    if (!GroupMember || !GroupMember->GetSession()) { playersDeadT2++; continue; }
                    if (GroupMember->isDead()) { playersDeadT2++; continue; }
                }
            }

            if (!zombieLeader->GetGroup())
                return true;
            if (zombieLeader->GetGroup() == NULL)
                return true;
            if (!playerGroup)
                return true;
            if (playerGroup == NULL)
                return true;

            if (team == 1 && playersDeadT1 >= 1) {
                return true;
            }
            if (team == 2 && playersDeadT2 >= 1) {
                return true;
            }
            return false;
        }

        bool sendMessageToGroup(const char* message)
        {
            if (!zombieLeader->GetGroup())
                return true;
            if (zombieLeader->GetGroup() == NULL)
                return true;
            if (!playerGroup)
                return true;

            Player* GroupMember;
            const Group::MemberSlotList members = playerGroup->GetMemberSlots();
            for (Group::member_citerator itr = members.begin(); itr != members.end(); ++itr)
            {
                GroupMember = (ObjectAccessor::GetPlayer(*me, itr->guid));
                if (GroupMember && GroupMember->GetSession())
                {
                    if (!GroupMember)
                        continue;
                    GroupMember->GetSession()->SendAreaTriggerMessage(message);
                }
            }
            return true;
        }

        void reviveAndTeleportOut()
        {
            if (!zombieLeader || zombieLeader == NULL)
            {
                teleportPlayersOut();
                return;
            }

            if (!zombieLeader->GetGroup())
            {
                teleportPlayersOut();
                return;
            }
            if (zombieLeader->GetGroup() == NULL)
            {
                teleportPlayersOut();
                return;
            }
            if (!playerGroup)
            {
                teleportPlayersOut();
                return;
            }

            if (playerGroup) {
                Player* GroupMember;
                const Group::MemberSlotList members = playerGroup->GetMemberSlots();
                for (Group::member_citerator itr = members.begin(); itr != members.end(); ++itr)
                {
                    GroupMember = (ObjectAccessor::GetPlayer(*me, itr->guid));
                    if (GroupMember && GroupMember->GetSession())
                    {
                        //if (!GroupMember->IsWithinDistInMap(me, 100))
                        //{
                        if (GroupMember->isDead())
                            GroupMember->ResurrectPlayer(60, false);
                        GroupMember->TeleportTo(534, 5541.770020f, -3783.469971f, 1594.910034f, 2.883010f);
                        //}
                    }
                }
            }
        }

        bool teleportPlayersOut()
        {
            while (me->SelectNearestPlayer(280.0f) != NULL)
            {
                Player* plr = me->SelectNearestPlayer(280.0f);
                if (plr)
                {
                    plr->ResurrectPlayer(60, false);
                    plr->TeleportTo(534, 5541.770020f, -3783.469971f, 1594.910034f, 2.883010f);
                }
            }
            return true;
        }

        void spawnZombies(Creature* creature, uint32 count)
        {
            for (uint32 i = 1; i <= count; i++) {
                uint32 NPC_ID = rand() % 6 + 1;
                creature->SummonCreature(ZOMBIE_NPCS[NPC_ID], Spawn_T1);
                creature->SummonCreature(ZOMBIE_NPCS[NPC_ID], Spawn_T2);
            }
        }

        int getZombieSpawnCountPerWave()
        {
            if (spawnTimer > 8.5) return 1;
            else if (spawnTimer > 7.0) return 2;
            else if (spawnTimer > 5.5) return 3;
            else if (spawnTimer > 4.0) return 4;
            else if (spawnTimer > 2.5) return 5;
            else if (spawnTimer > 1.0) return 6;
            else return 7;
        }

        bool resetZombieEvent()
        {
            spawnTimer = 10.0;
            addsPerWave = 1;

            playerGroup = NULL;
            zombieLeader = NULL;

            inZombieGame = false;
            me->UpdatePosition(-10989.9003f, -1940.1300f, 93.9107f, 4.8345f, true);
            return true;
        }

        void reviveGroup()
        {
            GroupReference* grpRef = playerGroup->GetFirstMember();
            for (grpRef; grpRef != NULL; grpRef = grpRef->next())
            {
                Player* groupMember = grpRef->GetSource();
                if (!groupMember)
                    continue;

                if (!groupMember->IsInFlight() && !groupMember->IsFlying())
                {
                    if (!groupMember->IsAlive() && groupMember->IsInWorld())
                    {
                        if (!groupMember->InBattleground() && !groupMember->InArena())
                        {
                            groupMember->ResurrectPlayer(60, false);
                        }
                    }
                    groupMember->TeleportTo(532, -10966.888f, -2011.3127f, 79.98f, 1.37f);
                }
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new zombie_bg_master_finalAI(creature);
    }
};

/*
---------------
Zombie Creatures AI
---------------
*/

class mobs_normal_zombie : public CreatureScript
{
public:
    mobs_normal_zombie() : CreatureScript("mobs_normal_zombie") { }

    struct mobs_normal_zombieAI : public ScriptedAI
    {
        mobs_normal_zombieAI(Creature* c) : ScriptedAI(c) {}

        void JustDied(Unit* killer)
        {
            if (me)
                me->DespawnOrUnsummon();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mobs_normal_zombieAI(creature);
    }
};

void AddSC_zombie_event()
{
    new zombie_bg_master();
    new zombie_bg_master_endgame();
    new zombie_bg_master_final();
    new mobs_normal_zombie();
}
