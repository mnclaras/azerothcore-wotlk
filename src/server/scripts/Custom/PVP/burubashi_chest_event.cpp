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

/*
* Name: Gurubashi Chest Event
* %Complete: 100
* Comment: Script to summon gurubashi chest every 3 hours, giving global announce to players
* Category: Custom Script
*/

#include "Common.h"
#include "DisableMgr.h"
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
#include "GameObjectAI.h"

// NPC: 731135
enum GurubashiChestEventInfo
{
    ZONE_STRANGLETHORN = 33,

    GAMEOBJECT_GURUBASHI_CHEST = 179697,
};

static const Position chestPos = { -13202.9f, 276.757f, 21.8571f, 2.77507f };

#define EMBLEM_OF_TRIUMPH_ENTRY 47241

class gurubashi_chest_event : public CreatureScript
{
public:
    gurubashi_chest_event() : CreatureScript("gurubashi_chest_event") { }

    struct gurubashi_chest_eventAI : public ScriptedAI
    {
        gurubashi_chest_eventAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 summonTimer;

        void Reset()
        {
            //summonTimer = 600000; // 10 Minutes for spawning on server restart
            summonTimer = 0; // Instant Spawn on server restart
        }

        void UpdateAI(const uint32 diff)
        {
            if (summonTimer <= diff)
            {
                me->SummonGameObject(GAMEOBJECT_GURUBASHI_CHEST,
                    chestPos.GetPositionX(),        // X
                    chestPos.GetPositionY(),        // Y
                    chestPos.GetPositionZ(),        // Z
                    chestPos.GetOrientation(),      // ANG
                    0,                              // Rotation0
                    0,                              // Rotation1
                    0,                              // Rotation2
                    0,                              // Rotation3
                    10740000);                      // RespawnTime

                summonTimer = 10740000;
            }
            else
            {
                std::ostringstream stream;
                if (summonTimer < 900000)
                {
                    stream << "|cffFFFF00[Arena Gurubashi]:|r El |cffFF0000Evento del Cofre|r va a comenzar en |cff00FF0015 minutos!|r";
                    sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
                }
                else if (summonTimer < 600000)
                {
                    stream << "|cffFFFF00[Arena Gurubashi]:|r El |cffFF0000Evento del Cofre|r va a comenzar en |cff00FF0010 minutos!|r";
                    sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
                }
                else if (summonTimer < 300000)
                {
                    stream << "|cffFFFF00[Arena Gurubashi]:|r El |cffFF0000Evento del Cofre|r va a comenzar en |cff00FF005 minutos!|r";
                    sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
                }
                else if (summonTimer < 60000)
                {
                    stream << "|cffFFFF00[Arena Gurubashi]:|r El |cffFF0000Evento del Cofre|r va a comenzar en |cff00FF001 minuto!|r";
                    sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
                }

                summonTimer -= diff;
            }
        }

        bool IsSpanishPlayer(Player* player)
        {
            LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
            return (locale == LOCALE_esES || locale == LOCALE_esMX);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new gurubashi_chest_eventAI(creature);
    }
};

void AddSC_gurubashi_chest_event()
{
    new gurubashi_chest_event();
}
