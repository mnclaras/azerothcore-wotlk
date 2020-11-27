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

        uint32 summonTimer = 120000;

        bool firstAdvertisementEmmited = false;
        bool secondAdvertisementEmmited = false;
        bool thirdAdvertisementEmmited = false;
        bool fourthAdvertisementEmmited = false;

        void Reset()
        {
            summonTimer = 120000; // 2 Minutes for spawning on server restart
            firstAdvertisementEmmited = false;
            secondAdvertisementEmmited = false;
            thirdAdvertisementEmmited = false;
            fourthAdvertisementEmmited = false;
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
                firstAdvertisementEmmited = false;
                secondAdvertisementEmmited = false;
                thirdAdvertisementEmmited = false;
                fourthAdvertisementEmmited = false;

                std::ostringstream stream;
                stream << "|cffFF0000[Arena Gurubashi]:|r |cffFFFF00El|r |cffFF0000Evento del Cofre|r |cff00FF00ha comenzado!|r";
                sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());

                std::ostringstream streamRange;
                streamRange << "|cffFFFF00El|r |cffFF0000Evento del Cofre|r |cff00FF00ha comenzado!|r";
                WorldPacket data;
                ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL, me, NULL, streamRange.str().c_str());
                me->SendMessageToSetInRange(&data, 500, false);
            }
            else
            {
                if (summonTimer <= 900000)
                {
                    if (isBetween(summonTimer, 600001, 900000) && !firstAdvertisementEmmited)
                    {
                        std::ostringstream stream1;
                        stream1 << "|cffFF0000[Arena Gurubashi]:|r |cffFFFF00El|r |cffFF0000Evento del Cofre|r |cffFFFF00va a comenzar en|r |cff00FF0015 minutos!|r";
                        sWorld->SendServerMessage(SERVER_MSG_STRING, stream1.str().c_str());
                        firstAdvertisementEmmited = true;
                    }
                    else if (isBetween(summonTimer, 300001, 600000) && !secondAdvertisementEmmited)
                    {
                        std::ostringstream stream2;
                        stream2 << "|cffFF0000[Arena Gurubashi]:|r |cffFFFF00El|r |cffFF0000Evento del Cofre|r |cffFFFF00va a comenzar en|r |cff00FF0010 minutos!|r";
                        sWorld->SendServerMessage(SERVER_MSG_STRING, stream2.str().c_str());
                        secondAdvertisementEmmited = true;
                    }
                    else if (isBetween(summonTimer, 60001, 300000) && !thirdAdvertisementEmmited)
                    {
                        std::ostringstream stream3;
                        stream3 << "|cffFF0000[Arena Gurubashi]:|r |cffFFFF00El|r |cffFF0000Evento del Cofre|r |cffFFFF00va a comenzar en|r |cff00FF005 minutos!|r";
                        sWorld->SendServerMessage(SERVER_MSG_STRING, stream3.str().c_str());
                        thirdAdvertisementEmmited = true;
                    }
                    else if (isBetween(summonTimer, 0, 60000) && !fourthAdvertisementEmmited)
                    {
                        std::ostringstream stream4;
                        stream4 << "|cffFF0000[Arena Gurubashi]:|r |cffFFFF00El|r |cffFF0000Evento del Cofre|r |cffFFFF00va a comenzar en|r |cff00FF001 minuto!|r";
                        sWorld->SendServerMessage(SERVER_MSG_STRING, stream4.str().c_str());
                        fourthAdvertisementEmmited = true;
                    }
                }

                summonTimer -= diff;
            }
        }

        bool isBetween(uint32 value, uint32 first, uint32 second)
        {
            return value >= first && value <= second;
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
