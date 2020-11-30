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

enum Events
{
    EVENT_NONE,
    EVENT_15_MINUTES_TO_START,
    EVENT_10_MINUTES_TO_START,
    EVENT_5_MINUTES_TO_START,
    EVENT_1_MINUTES_TO_START,
    EVENT_SUMMON_CHEST
};

static const Position chestPos = { -13202.9f, 276.757f, 21.8571f, 2.77507f };

class gurubashi_chest_event : public CreatureScript
{
public:
    gurubashi_chest_event() : CreatureScript("gurubashi_chest_event") { }

    struct gurubashi_chest_eventAI : public ScriptedAI
    {
        gurubashi_chest_eventAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            _events.Reset();

            time_t t = time(nullptr);
            tm aTm;
            localtime_r(&t, &aTm);

            // 11:40:40 -> 19 MINUTES AND 20 SECONDS TO START
            uint32 milliseconds_to_start = 0;

            // 3 - (11%3) = 3-2 = 1 * 3600 = 3600 SECONDS (1 HOUR)
            milliseconds_to_start = (3 - (aTm.tm_hour % 3)) * HOUR;
            // 3600 - (60 - (60-40)) = 3600 - (60-20) = 3600 - (40 * 60) = 3600 - 2400 = 1200 SECONDS (20 MINUTES)
            milliseconds_to_start -= ((60 - (60 - aTm.tm_min)) * MINUTE);
            // 1200 - (60 - (60-40)) = 1200 - (60-20) = 1200 - 40 = 1160 SECONDS (19 MINUTES AND 20 SECONDS)
            milliseconds_to_start -= (60 - (60 - aTm.tm_sec));
            // 1160 * 1000 = 1160000 MILLISECONDS TO START
            milliseconds_to_start *= IN_MILLISECONDS;

            if (milliseconds_to_start > (15 * MINUTE * IN_MILLISECONDS))
                _events.ScheduleEvent(EVENT_15_MINUTES_TO_START, (milliseconds_to_start - (15 * MINUTE * IN_MILLISECONDS)));

            if (milliseconds_to_start > (10 * MINUTE * IN_MILLISECONDS))
                _events.ScheduleEvent(EVENT_10_MINUTES_TO_START, (milliseconds_to_start - (10 * MINUTE * IN_MILLISECONDS)));

            if (milliseconds_to_start > (5 * MINUTE * IN_MILLISECONDS))
                _events.ScheduleEvent(EVENT_5_MINUTES_TO_START, (milliseconds_to_start - (5 * MINUTE * IN_MILLISECONDS)));

            if (milliseconds_to_start > (1 * MINUTE * IN_MILLISECONDS))
                _events.ScheduleEvent(EVENT_1_MINUTES_TO_START, (milliseconds_to_start - (1 * MINUTE * IN_MILLISECONDS)));

            if (milliseconds_to_start > 0)
                _events.ScheduleEvent(EVENT_SUMMON_CHEST, milliseconds_to_start);
        }

        void UpdateAI(const uint32 diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_15_MINUTES_TO_START:
                    BroadcastMessageLeftTime("15");
                    break;
                case EVENT_10_MINUTES_TO_START:
                    BroadcastMessageLeftTime("10");
                    break;
                case EVENT_5_MINUTES_TO_START:
                    BroadcastMessageLeftTime("5");
                    break;
                case EVENT_1_MINUTES_TO_START:
                    BroadcastMessageLeftOneMinute();
                    break;
                case EVENT_SUMMON_CHEST:
                    SummonChest();
                    Reset();
                    break;
                }
            }
        }

        void SummonChest()
        {
            std::ostringstream stream;
            stream << "|cffFF0000[Arena Gurubashi]:|r |cffFFFF00El|r |cffFF0000Evento del Cofre|r |cff00FF00ha comenzado!|r";
            sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());

            std::ostringstream streamRange;
            streamRange << "|cffFFFF00El|r |cffFF0000Evento del Cofre|r |cff00FF00ha comenzado!|r";
            WorldPacket data;
            ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_BOSS_EMOTE, LANG_UNIVERSAL, me, NULL, streamRange.str().c_str());
            me->SendMessageToSetInRange(&data, 500, false);

            me->SummonGameObject(GAMEOBJECT_GURUBASHI_CHEST,
                chestPos.GetPositionX(),        // X
                chestPos.GetPositionY(),        // Y
                chestPos.GetPositionZ(),        // Z
                chestPos.GetOrientation(),      // ANG
                0,                              // Rotation0
                0,                              // Rotation1
                0,                              // Rotation2
                0,                              // Rotation3
                10800000);                      // RespawnTime
        }

        void BroadcastMessageLeftTime(std::string timeLeft)
        {
            std::ostringstream stream;
            stream << "|cffFF0000[Arena Gurubashi]:|r |cffFFFF00El|r |cffFF0000Evento del Cofre|r |cffFFFF00va a comenzar en|r |cff00FF00" << timeLeft << " minutos!|r";
            sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
        }

        void BroadcastMessageLeftOneMinute()
        {
            std::ostringstream stream;
            stream << "|cffFF0000[Arena Gurubashi]:|r |cffFFFF00El|r |cffFF0000Evento del Cofre|r |cffFFFF00va a comenzar en|r |cff00FF001 minuto!|r";
            sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
        }


    private:
        EventMap _events;
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
