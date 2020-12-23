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

class custom_npc_tools_reset_cds : public CreatureScript
{
public:
    custom_npc_tools_reset_cds() : CreatureScript("npc_tools_reset_cds") { }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Reiniciar CDs." : "Reset Cooldowns.", GOSSIP_SENDER_MAIN, 3,
            isSpanish ? "Estas seguro de que quieres reiniciar CDs?" : "Are you sure you want to reset your CDs?", 0, false);
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
            case 3: // Reset Cooldowns
                if (player->IsInCombat())
                {
                    player->GetSession()->SendNotification(isSpanish ? "Error! Estas en combate." : "Failure! You are in combat.");
                    CloseGossipMenuFor(player);
                    return false;
                }

                player->RemoveAllSpellCooldown();

                if (player->getPowerType() == POWER_MANA || player->getClass() == CLASS_DRUID)
                    player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));
                player->SetHealth(player->GetMaxHealth());

                if (Pet* pet = player->GetPet())
                {
                    if (!pet->m_CreatureSpellCooldowns.empty())
                    {
                        for (CreatureSpellCooldowns::const_iterator itr2 = pet->m_CreatureSpellCooldowns.begin(); itr2 != pet->m_CreatureSpellCooldowns.end(); ++itr2)
                            player->SendClearCooldown(itr2->first, pet);
                        // actually clear cooldowns
                        pet->m_CreatureSpellCooldowns.clear();
                    }
                }


                player->GetSession()->SendNotification(isSpanish ? "|cffFFFF00SERVICIOS \n |cffFFFFFFCDs reiniciados satisfactoriamente!" : "|cffFFFF00SERVICES \n |cffFFFFFFCooldowns succesfully reseted!");
                player->CastSpell(player, 31726);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

	struct custom_npc_tools_reset_cdsAI : public ScriptedAI
	{
        custom_npc_tools_reset_cdsAI(Creature* creature) : ScriptedAI(creature) { }
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new custom_npc_tools_reset_cdsAI(creature);
	}
};

void AddSC_custom_npc_tools_reset()
{
	new custom_npc_tools_reset_combat();
	new custom_npc_tools_reset_instances();
	new custom_npc_tools_reset_cds()
}
