#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "Unit.h"
#include "Chat.h"
#include "WorldSession.h"
#include "Creature.h"

#define DEFAULT_MESSAGE 907

class custom_npc_tools_reset_instances : public CreatureScript
{
public:
    custom_npc_tools_reset_instances() : CreatureScript("npc_tools_reset_instances") { }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Reiniciar instancias." : "Reset Instances.", GOSSIP_SENDER_MAIN, 2,
            isSpanish ? "Estas seguro de que quieres eliminar el save de tus instancias?" : "Are you sure you want to reset your instances?", 0, false);
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
            case 2: // Reset Instances
                uint32 diff = 2;
                for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
                {
                    BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(player->GetGUIDLow(), Difficulty(i));
                    for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end();)
                    {
                        if (itr->first != player->GetMapId())
                        {
                            sInstanceSaveMgr->PlayerUnbindInstance(player->GetGUIDLow(), itr->first, Difficulty(i), true, player);
                            itr = m_boundInstances.begin();
                        }
                        else
                            ++itr;
                    }
                }


                player->GetSession()->SendNotification(isSpanish ? "|cffFFFF00SERVICIOS \n |cffFFFFFFInstancias reiniciadas satisfactoriamente!" : "|cffFFFF00SERVICES \n |cffFFFFFFInstances succesfully reseted!");
                player->CastSpell(player, 31726);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

	struct custom_npc_tools_reset_instancesAI : public ScriptedAI
	{
        custom_npc_tools_reset_instancesAI(Creature* creature) : ScriptedAI(creature) { }
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new custom_npc_tools_reset_instancesAI(creature);
	}
};

void AddSC_custom_npc_tools_reset_instances()
{
	new custom_npc_tools_reset_instances();
}
