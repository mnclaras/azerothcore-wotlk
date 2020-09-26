#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "Unit.h"
#include "Chat.h"
#include "WorldSession.h"
#include "Creature.h"

#define DEFAULT_MESSAGE 907

class custom_npc_tools_reset_combat : public CreatureScript
{
public:
    custom_npc_tools_reset_combat() : CreatureScript("npc_tools_reset_combat") { }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }


    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Quitar combate." : "Reset Combat.", GOSSIP_SENDER_MAIN, 4,
            isSpanish ? "Quitar combate?" : "Reset combat?", 0, false);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender == GOSSIP_SENDER_MAIN)
        {
            bool isSpanish = IsSpanishPlayer(player);

            switch (action)
            {
            case 4: // Reset Combat
                player->CombatStop();
                player->GetSession()->SendNotification(isSpanish ? "|cffFFFF00SERVICIOS \n |cffFFFFFFCombate reiniciado satisfactoriamente!" : "|cffFFFF00SERVICES \n |cffFFFFFFCombat succesfully removed!");
                player->CastSpell(player, 31726);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

	struct custom_npc_tools_reset_combatAI : public ScriptedAI
	{
        custom_npc_tools_reset_combatAI(Creature* creature) : ScriptedAI(creature) { }
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new custom_npc_tools_reset_combatAI(creature);
	}
};

void AddSC_custom_npc_tools_reset_combat()
{
	new custom_npc_tools_reset_combat();
}
