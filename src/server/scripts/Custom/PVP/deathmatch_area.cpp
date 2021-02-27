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

const int MAX_PLAYER_SPAWN_POINTS = 43;
static const Position playerSpawnPoint[MAX_PLAYER_SPAWN_POINTS] =
{
    { -133.3434,	-134.3434,	-130.3434f,	0.7696f },
    { -132.3434f,	-71.3434f,	-130.3434f,	6.2635f },
    { -131.3434f,	-19.3434f,	-130.3434f,	6.2674f },
    { -130.3434f,	40.3434f,	-130.3434f,	6.2674f },
    { -130.3434f,	86.3434f,	-130.3434f,	6.2674f },
    { -130.3434f,	129.3434f,	-130.3434f,	6.2674f },
    { -86.3434f,	135.3434f,	-130.3434f,	4.6417f },
    { -23.3434f,	130.3434f,	-130.3434f,	4.6417f },
    { 32.3434f,		130.3434f,	-130.3434f,	4.6417f },
    { 86.3434f,		130.3434f,	-130.3434f,	4.6417f },
    { 128.3434f,	130.3434f,	-130.3434f,	4.0809f },
    { 133.3434f,	92.3434f,	-130.3434f,	3.1149f },
    { 133.3434f,	36.3434f,	-130.3434f,	3.1149f },
    { 133.3434f,	-25.3434f,	-130.3434f,	3.1149f },
    { 133.3434f,	-88.3434f,	-130.3434f,	3.1149f },
    { 130.3434f,	-132.3434f,	-130.3434f,	2.3805f },
    { 94.3434f,		-136.3434f,	-130.3434f,	1.5598f },
    { 51.3434f,		-136.3434f,	-130.3434f,	1.5598f },
    { -10.3434f,	-136.3434f,	-130.3434f,	1.5598f },
    { -76.3434f,	-136.3434f,	-130.3434f,	1.5598f },
    { -102.3434f,	-104.3434f,	-130.3434f,	0.7744f },
    { -118.3434f,	-45.3434f,	-130.3434f,	6.2682f },
    { -117.3434f,	11.3434f,	-130.3434f,	6.2682f },
    { -118.3434f,	70.3434f,	-130.3434f,	0.0086f },
    { -70.3434f,	113.3434f,	-130.3434f,	4.7171f },
    { -12.3434f,	114.3434f,	-130.3434f,	4.7171f },
    { 31.3434f,		113.3434f,	-130.3434f,	4.7171f },
    { 88.3434f,		113.3434f,	-130.3434f,	4.7171f },
    { 114.3434f,	53.3434f,	-130.3434f,	3.1306f },
    { 115.3434f,	5.3434f,	-130.3434f,	3.1306f },
    { 116.3434f,	-43.3434f,	-130.3434f,	3.1306f },
    { 114.3434f,	-94.3434f,	-130.3434f,	3.1306f },
    { 67.3434f,		-119.3434f,	-130.3434f,	1.5928f },
    { 25.3434f,		-119.3434f,	-130.3434f,	1.5928f },
    { -22.3434f,	-119.3434f,	-130.3434f,	1.5928f },
    { -73.3434f,	-119.3434f,	-130.3434f,	1.5928f },
    { -115.3434f,	-119.3434f,	-130.3434f,	0.9527f },
    { -42.3434f,	1.3434f,	-130.3434f,	1.8527f },
    { 32.3434f,		0.9834f,	-130.3434f,	3.7527f },
    { 34.3434f,		-63.3434f,	-130.3434f,	2.5527f },
    { -35.3434f,	-68.3434f,	-130.3434f,	0.7527f },
    { 11.3434f,		-94.3434f,	-130.3434f,	1.5927f },
    { -6.3434f,		14.3434f,	-130.3434f,	4.7527f },
};

static const Position shop = { -11823.9f, -4779.58f, 5.9206f, 1.1357f };

class deathmatch_resurrect_event : public BasicEvent
{
public:
    deathmatch_resurrect_event(Player* victim) : _victim(victim) { }

    bool Execute(uint64 /*time*/, uint32 /*diff*/)
    {
        while (_victim->IsFlying() || _victim->IsFalling()) {
            std::this_thread::sleep_for(Microseconds(100));
        }

        if (!_victim->IsAlive())
        {
            _victim->ResurrectPlayer(1.0f, false);
        }

        // Temporary basic cooldown reset
        _victim->RemoveArenaSpellCooldowns(true);

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

        // This will cause warlocks and hunters to have their last-used pet to be re-summoned
        if (_victim->getClass() == CLASS_HUNTER || _victim->getClass() == CLASS_WARLOCK)
            _victim->m_Events.AddEvent(new deathmatch_resurrect_event_pet(_victim), _victim->m_Events.CalculateTime(1000));

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

                // Temporary basic cooldown reset
                player->RemoveArenaSpellCooldowns(true);

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

                //if (victim->getClass() == CLASS_HUNTER || victim->getClass() == CLASS_WARLOCK)
                //    victim->m_Events.AddEvent(new deathmatch_resurrect_event_pet(victim), victim->m_Events.CalculateTime(2000));

                return;
            }
           
            // This will cause the victim to be resurrected, teleported and health set to 100% after 1 second of dieing
            victim->m_Events.AddEvent(new deathmatch_resurrect_event(victim), victim->m_Events.CalculateTime(1000));

            // This will cause warlocks and hunters to have their last-used pet to be re-summoned
            //if (victim->getClass() == CLASS_HUNTER || victim->getClass() == CLASS_WARLOCK)
            //    victim->m_Events.AddEvent(new deathmatch_resurrect_event_pet(victim), victim->m_Events.CalculateTime(2000));
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

        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "---- TELEPORT ----", GOSSIP_SENDER_MAIN, 5000);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Ir a la zona del evento." : "Go to event zone.", GOSSIP_SENDER_MAIN, 2000,
            isSpanish ? "Seguro?" : "Are you sure?", 0, false);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "---- TELEPORT ----", GOSSIP_SENDER_MAIN, 5000);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        if (sender == GOSSIP_SENDER_MAIN)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case 2000:
                player->TeleportTo(MAP_DEATHMATCH, -106.8676f, -104.6085f, -133.6136f, 0.0157f);               
                break;
            case 5000:
                // return to OnGossipHello menu, otherwise it will freeze every menu
                OnGossipHello(player, creature);
                break;
            default:
                // return to OnGossipHello menu, otherwise it will freeze every menu
                OnGossipHello(player, creature);
                break;
            }
            CloseGossipMenuFor(player);
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

void AddSC_deathmatch_area()
{
    new deathmatch_area();
    new deathmatch_event_teleporter();
}
