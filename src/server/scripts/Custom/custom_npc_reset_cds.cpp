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

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 sender, uint32 action) override
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

class custom_npc_tools_reset_stats : public CreatureScript
{
public:
    custom_npc_tools_reset_stats() : CreatureScript("custom_npc_tools_reset_stats") { }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    static bool HandleResetStatsOrLevelHelper(Player* player)
    {
        ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(player->getClass());
        if (!classEntry)
        {
            sLog->outError("Class %u not found in DBC (Wrong DBC files?)", player->getClass());
            return false;
        }

        uint8 powerType = classEntry->powerType;

        // reset m_form if no aura
        if (!player->HasAuraType(SPELL_AURA_MOD_SHAPESHIFT))
            player->SetShapeshiftForm(FORM_NONE);

        player->setFactionForRace(player->getRace());

        player->SetUInt32Value(UNIT_FIELD_BYTES_0, ((player->getRace()) | (player->getClass() << 8) | (player->getGender() << 16) | (powerType << 24)));

        // reset only if player not in some form;
        if (player->GetShapeshiftForm() == FORM_NONE)
            player->InitDisplayIds();

        player->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);

        player->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

        //-1 is default value
        player->SetUInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX, uint32(-1));
        return true;
    }


    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Reiniciar Stats." : "Reset Stats.", GOSSIP_SENDER_MAIN, 3,
            isSpanish ? "Estas seguro de que quieres reiniciar Stats?" : "Are you sure you want to reset your Stats?", 0, false);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 sender, uint32 action) override
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

                if (HandleResetStatsOrLevelHelper(player))
                {
                    player->InitRunes();
                    player->InitStatsForLevel(true);
                    player->InitTaxiNodesForLevel();
                    player->InitGlyphsForLevel();
                    player->InitTalentForLevel();
                }

                player->GetSession()->SendNotification(isSpanish ? "|cffFFFF00SERVICIOS \n |cffFFFFFFStats reiniciados satisfactoriamente!" : "|cffFFFF00SERVICES \n |cffFFFFFFStats succesfully reseted!");
                player->CastSpell(player, 31726);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

    struct custom_npc_tools_reset_statsAI : public ScriptedAI
    {
        custom_npc_tools_reset_statsAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_npc_tools_reset_statsAI(creature);
    }
};

void AddSC_custom_npc_tools_reset_cds()
{
    new custom_npc_tools_reset_cds();
    new custom_npc_tools_reset_stats();
}
