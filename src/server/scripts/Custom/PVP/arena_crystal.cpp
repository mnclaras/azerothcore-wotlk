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
