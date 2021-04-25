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
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "Unit.h"
#include "Chat.h"
#include "WorldSession.h"
#include "Creature.h"
#include "Pet.h"

#define DEFAULT_MESSAGE 907

class custom_npc_leave_spectator : public CreatureScript
{
public:
    custom_npc_leave_spectator() : CreatureScript("npc_leave_spectator") { }

    static bool HandleSpectatorLeaveCommand(Player* player)
    {
        if (!player->IsSpectator() || !player->FindMap() || !player->FindMap()->IsBattleArena())
        {
            bool isSpanish = player->hasSpanishClient();
            player->GetSession()->SendNotification(isSpanish ? "No eres un espectador." : "You are not a spectator.");
            return true;
        }

        //player->SetIsSpectator(false);
        player->TeleportToEntryPoint();
        return true;
    }


    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = player->hasSpanishClient();
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Abandonar arena." : "Leave Arena.", GOSSIP_SENDER_MAIN, 3,
            isSpanish ? "Estas seguro de que quieres abandonar la arena?" : "Are you sure you want to leave the arena?", 0, false);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case 3: // Leave Arena
                HandleSpectatorLeaveCommand(player);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

    struct custom_npc_leave_spectatorAI : public ScriptedAI
    {
        custom_npc_leave_spectatorAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_npc_leave_spectatorAI(creature);
    }
};

void AddSC_custom_npc_leave_spectator()
{
    new custom_npc_leave_spectator();
}
