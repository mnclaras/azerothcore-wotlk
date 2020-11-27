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

#include "Battleground.h"
#include "Config.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include "GameObjectAI.h"

class FastArenaCrystal : public GameObjectScript
{
public:
    FastArenaCrystal() : GameObjectScript("FastArenaCrystal") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (Battleground* bg = player->GetBattleground())
        {
            // Don't let spectators to use arena crystal
            if (player->IsSpectator())
            {
                player->GetSession()->SendAreaTriggerMessage("You're not be able to do this while spectating.");
                return false;
            }

            if (player->IsGameMaster()) // Can cause bug if GM clicks the crystal. Lets avoid that.
            {
                player->GetSession()->SendAreaTriggerMessage("You're not allowed to do this while in GameMaster mode.");
                return false;
            }

            if (bg->isArena())
            {
                /* Cuando haya soloq */
                //if (bg->isArena() && bg->GetArenaType() == ARENA_TYPE_3v3_SOLO) // 3v3 solo queue
                    //player->GetSession()->SendAreaTriggerMessage("Players marked as ready: %u/6", bg->ClickFastStart(player, go));

                if (bg->isArena() && bg->GetArenaType() == ARENA_TYPE_5v5) // 1v1
                    player->GetSession()->SendAreaTriggerMessage("Players marked as ready: %u/2", bg->ClickFastStart(player, go));

                if (bg->isArena() && bg->GetArenaType() == ARENA_TYPE_2v2) // 2v2
                    player->GetSession()->SendAreaTriggerMessage("Players marked as ready: %u/4", bg->ClickFastStart(player, go));

                if (bg->isArena() && bg->GetArenaType() == ARENA_TYPE_3v3) // 3v3
                    player->GetSession()->SendAreaTriggerMessage("Players marked as ready: %u/6", bg->ClickFastStart(player, go));
            }
        }
        return false;
    }


    struct FastArenaCrystalAI : public GameObjectAI
    {
        FastArenaCrystalAI(GameObject* go) : GameObjectAI(go) { }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new FastArenaCrystalAI(go);
    }

};

void AddSC_fast_arena_start()
{
    new FastArenaCrystal();
}
