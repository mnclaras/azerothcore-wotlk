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
