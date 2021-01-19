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

#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "Configuration/Config.h"
#include "Chat.h"
#include "Unit.h"
#include "WorldSession.h"
#include "Creature.h"

class multi_changer : public CreatureScript
{
public:
    multi_changer() : CreatureScript("char_tools") { }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        //player->PlayerTalkClass->ClearMenus();

        if (player->IsInCombat())
            return false;

        //if (!sConfigMgr->GetBoolDefault("CharacterTools", true))
        //    return false;

        AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "|TInterface/Icons/Ability_Paladin_BeaconofLight:50:50|tCambiar mi raza" : "|TInterface/Icons/Ability_Paladin_BeaconofLight:50:50|tChange My Race", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "|TInterface/Icons/INV_BannerPVP_01:50:50|tCambiar mi faccion" : "|TInterface/Icons/INV_BannerPVP_01:50:50|tChange My Faction", GOSSIP_SENDER_MAIN, 2);
        AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "|TInterface/Icons/Achievement_BG_returnXflags_def_WSG:50:50|tCambiar mi apariencia" : "|TInterface/Icons/Achievement_BG_returnXflags_def_WSG:50:50|tChange My Appearance", GOSSIP_SENDER_MAIN, 3);
        AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "|TInterface/Icons/INV_Inscription_Scroll:50:50|tCambiar mi nombre" : "|TInterface/Icons/INV_Inscription_Scroll:50:50|tChange My Name", GOSSIP_SENDER_MAIN, 4);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender == GOSSIP_SENDER_MAIN)
        {
            bool isSpanish = IsSpanishPlayer(player);

            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
            case 1:
                player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "INFORMACION: Relogea para cambiar tu raza." : "INFORMATION: Please log out for race change.");
                break;
            case 2:
                player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "INFORMACION: Relogea para cambiar tu faccion." : "INFORMATION: Please log out for faction change.");
                break;
            case 3:
                player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "INFORMACION: Relogea para customizar tu personaje." : "INFORMATION: Please log out for Character Customize.");
                break;
            case 4:
                player->SetAtLoginFlag(AT_LOGIN_RENAME);
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "INFORMACION: Relogea para cambiar tu nombre." : "INFORMATION: Please log out for name change.");
                break;
            }

            CloseGossipMenuFor(player);
        }

        return true;
    };

    struct myAI : public ScriptedAI
    {
        myAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new myAI(creature);
    }
};

void AddSC_multi_changer()
{
    new multi_changer();
}
