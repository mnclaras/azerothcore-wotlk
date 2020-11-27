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

#include "Language.h"
#include "Chat.h"
#include "SpellMgr.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Log.h"
#include "AccountMgr.h"
#include "DBCStores.h"
#include "World.h"
#include "Object.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Common.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

#define CREATURE_VENDOR_T9_HORDE         601632
#define CREATURE_VENDOR_T9_ALLIANCE      601585
#define DEFAULT_MESSAGE 907

class custom_npc_tier_nine : public CreatureScript
{
public:
    custom_npc_tier_nine() : CreatureScript("custom_npc_tier_nine") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetTeamId() == TEAM_ALLIANCE)
        {
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_T9_ALLIANCE);
            player->PlayerTalkClass->SendCloseGossip();
        }
        else if (player->GetTeamId() == TEAM_HORDE)
        {
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_T9_HORDE);
            player->PlayerTalkClass->SendCloseGossip();
        }
        return true;
    }

    struct custom_npc_tier_nineAI : public ScriptedAI
    {
        custom_npc_tier_nineAI(Creature* creature) : ScriptedAI(creature) { }
    };

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new custom_npc_tier_nineAI(creature);
	}
};

void AddSC_custom_npc_tier_nine()
{
	new custom_npc_tier_nine();
}
