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

#include "AccountMgr.h"
#include "ArenaTeamMgr.h"
#include "Common.h"
#include "DisableMgr.h"
#include "BattlegroundMgr.h"
#include "ArenaTeam.h"
#include "Language.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "WorldSession.h"
#include "Log.h"
#include "Chat.h"
#include "Common.h"
#include "DisableMgr.h"
#include "DBCStores.h" 
#include "ScriptMgr.h"
#include "World.h"
#include "Group.h"
#include "GroupMgr.h"
#include "GameObjectAI.h"
#include "Player.h"
#include "LFGScripts.h"
#include "LFGMgr.h"


enum DeathmatchAreaVariables
{
    AREA_DEATHMATCH = 3817,
    MAP_DEATHMATCH = 13,
    MAP_TANARIS = 1,

    GAME_EVENT_DEATHMATCH = 77,

    SPELL_SUMMON_PET = 883,
    SPELL_IMMUNE_3_SECONDS = 29309,

    DEFAULT_MESSAGE = 907
};

const int MAX_PLAYER_SPAWN_POINTS = 4;
static const Position playerSpawnPoint[MAX_PLAYER_SPAWN_POINTS] =
{
    { -106.8676f, -104.6085f, -133.6136f, 0.0157f },
    { -121.4688f, 114.4886f, -135.2685f, 6.2714f },
    { 98.1109f, 113.5380f, -134.3560f, 0.0000f },
    { 112.9034f, -115.2211f, -132.3255f, 4.8027f },
   /* {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},
    {f, f, f, f},*/
};

//static const Position startingPos = { f, f, f, f};
//_victim->TeleportTo(MAP_DEATHMATCH, startingPos.GetPositionX(), startingPos.GetPositionY(), startingPos.GetPositionZ(), startingPos.GetOrientation());

static const Position shop = { -11823.9f, -4779.58f, 5.9206f, 1.1357f };

class deathmatch_resurrect_event : public BasicEvent
{
public:
    deathmatch_resurrect_event(Player* victim) : _victim(victim) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/)
    {
        _victim->ResurrectPlayer(1.0f, false);

        // Spawn on random point
        int i = urand(0, MAX_PLAYER_SPAWN_POINTS - 1);
        _victim->TeleportTo(MAP_DEATHMATCH,
            playerSpawnPoint[i].GetPositionX(),
            playerSpawnPoint[i].GetPositionY(),
            playerSpawnPoint[i].GetPositionZ(),
            playerSpawnPoint[i].GetOrientation());
        _victim->CastSpell(_victim, SPELL_IMMUNE_3_SECONDS, true);

        // Set full HP/MP
        _victim->ResetAllPowers();
        if (_victim->getClass() == CLASS_DRUID)
            _victim->SetPower(POWER_MANA, _victim->GetMaxPower(POWER_MANA));

        // Temporary basic cooldown reset
        _victim->RemoveArenaSpellCooldowns(true);

        return true;
    }

private:
    Player* _victim;
};

class deathmatch_resurrect_event_pet : public BasicEvent
{
public:
    deathmatch_resurrect_event_pet(Player* victim) : _victim(victim) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/)
    {
        _victim->CastSpell(_victim, SPELL_SUMMON_PET, false); // Summons last used pet
        return true;
    }

private:
    Player* _victim;
};

class deathmatch_area : public PlayerScript
{
public:
    deathmatch_area() : PlayerScript("deathmatch_area") { }

    uint64 killerGUID = 0;
    uint64 victimGUID = 0;

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }


    void OnUpdateArea(Player* player, uint32 oldArea, uint32 newArea)
    {
        if (sGameEventMgr->IsActiveEvent(GAME_EVENT_DEATHMATCH))
        {
            if (oldArea != AREA_DEATHMATCH && newArea == AREA_DEATHMATCH)
            {
                Group* group = player->GetGroup();
                if (group) {
                    group->RemoveMember(player->GetGUID());
                }

                if (!player || player->IsBeingTeleported() || !player->IsAlive())
                    return;

                int i = urand(0, MAX_PLAYER_SPAWN_POINTS - 1);
                player->TeleportTo(MAP_DEATHMATCH,
                    playerSpawnPoint[i].GetPositionX(),
                    playerSpawnPoint[i].GetPositionY(),
                    playerSpawnPoint[i].GetPositionZ(),
                    playerSpawnPoint[i].GetOrientation());

                player->CastSpell(player, SPELL_IMMUNE_3_SECONDS, true);

                // Set full HP/MP
                player->ResetAllPowers();
                if (player->getClass() == CLASS_DRUID)
                    player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));

                // Temporary basic cooldown reset
                player->RemoveArenaSpellCooldowns(true);
            }
        }   
    }

    void OnPVPKill(Player* killer, Player* victim)
    {
        if (killer->GetAreaId() == AREA_DEATHMATCH && victim->GetAreaId() == AREA_DEATHMATCH)
        {
            killerGUID = killer->GetGUID();
            victimGUID = victim->GetGUID();

            Group* group = killer->GetGroup();
            if (group)
            {
                ChatHandler(killer->GetSession()).PSendSysMessage(IsSpanishPlayer(killer) ?
                    "|cffff6060[Informacion]:|r La Zona Deathmatch no acepta grupos!"
                    : "|cffff6060[Information]:|r Groups are not allowed in Deathmatch Zone!");

                for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                    if (Player* member = itr->GetSource())
                        if (member->IsInMap(killer) && killer->GetGUID() != member->GetGUID())
                        {
                            ChatHandler(member->GetSession()).PSendSysMessage(IsSpanishPlayer(member) ?
                                "|cffff6060[Informacion]:|r La Zona Deathmatch no acepta grupos!"
                                : "|cffff6060[Information]:|r Groups are not allowed in Deathmatch Zone!");
                            member->TeleportTo(MAP_TANARIS, shop.GetPositionX(), shop.GetPositionY(), shop.GetPositionZ(), shop.GetOrientation());
                        }

                killer->TeleportTo(MAP_TANARIS, shop.GetPositionX(), shop.GetPositionY(), shop.GetPositionZ(), shop.GetOrientation());
                return;
            }

            // If player killed himself, do not execute any code (think of when a warlock uses Hellfire, when player falls to dead, etc.)
            if (killerGUID == victimGUID)
            {
                victim->m_Events.AddEvent(new deathmatch_resurrect_event(victim), victim->m_Events.CalculateTime(1000));

                if (victim->getClass() == CLASS_HUNTER || victim->getClass() == CLASS_WARLOCK)
                    victim->m_Events.AddEvent(new deathmatch_resurrect_event_pet(victim), victim->m_Events.CalculateTime(2000));

                return;
            }
           
            // This will cause the victim to be resurrected, teleported and health set to 100% after 1 second of dieing
            victim->m_Events.AddEvent(new deathmatch_resurrect_event(victim), victim->m_Events.CalculateTime(1000));

            // This will cause warlocks and hunters to have their last-used pet to be re-summoned when arriving on the island
            if (victim->getClass() == CLASS_HUNTER || victim->getClass() == CLASS_WARLOCK)
                victim->m_Events.AddEvent(new deathmatch_resurrect_event_pet(victim), victim->m_Events.CalculateTime(2000));
        }
    }
}; 

class deathmatch_event_teleporter : public CreatureScript
{
public:
    deathmatch_event_teleporter() : CreatureScript("deathmatch_event_teleporter") { }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Ir a la zona del evento." : "Go to event zone.", GOSSIP_SENDER_INFO, 2000,
            isSpanish ? "Seguro?" : "Are you sure?", 0, false);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 sender, uint32 action) override
    {
        if (sender == GOSSIP_SENDER_INFO)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case 2000:
                player->TeleportTo(MAP_DEATHMATCH, -106.8676f, -104.6085f, -133.6136f, 0.0157f);
                CloseGossipMenuFor(player);
                break;
            }
            return true;
        }
        return false;
    }

    struct deathmatch_event_teleporterAI : public ScriptedAI
    {
        deathmatch_event_teleporterAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new deathmatch_event_teleporterAI(creature);
    }
};

//class npc_teleport_pvp_island : public CreatureScript
//{
//public:
//    npc_teleport_pvp_island() : CreatureScript("npc_teleport_pvp_island") { }
//
//    struct npc_teleport_pvp_islandAI : public ScriptedAI
//    {
//        npc_teleport_pvp_islandAI(Creature* creature) : ScriptedAI(creature) { }
//
//        void UpdateAI(const uint32 /*diff*/)
//        {
//            if (Player* player = me->SelectNearestPlayer(0.7f))
//            {
//                if (!player || player->IsBeingTeleported() || !player->IsAlive())
//                    return;
//                
//                int i = urand(0, MAX_PLAYER_SPAWN_POINTS - 1);
//                player->TeleportTo(0, playerSpawnPoint[i].GetPositionX(), playerSpawnPoint[i].GetPositionY(), playerSpawnPoint[i].GetPositionZ(), playerSpawnPoint[i].GetOrientation());
//            }
//        }
//    };
//
//    CreatureAI* GetAI(Creature* creature) const
//    {
//        return new npc_teleport_pvp_islandAI(creature);
//    }
//};
//
//class npc_teleport_mall : public CreatureScript
//{
//public:
//    npc_teleport_mall() : CreatureScript("npc_teleport_mall") { }
//
//    struct npc_teleport_mallAI : public ScriptedAI
//    {
//        npc_teleport_mallAI(Creature* creature) : ScriptedAI(creature) { }
//
//        void UpdateAI(const uint32 /*diff*/) override
//        {
//            if (Player* player = me->SelectNearestPlayer(0.7f))
//            {
//                if (!player || player->IsBeingTeleported() || !player->IsAlive())
//                    return;
//
//                // We make it spam this message when player is on-top of the teleporter
//                if (player->IsInCombat())
//                {
//                    player->GetSession()->SendNotification(STRING_IN_COMBAT);
//                    return;
//                }
//                player->GetSession()->SendNotification("You left the warzone.");
//                player->TeleportTo(1, -11823.9, -4779.58, 5.9206, 1.1357);
//            }
//        }
//    };
//
//    CreatureAI* GetAI(Creature* creature) const
//    {
//        return new npc_teleport_mallAI(creature);
//    }
//};

void AddSC_deathmatch_area()
{
    new deathmatch_area();
    new deathmatch_event_teleporter();
}
