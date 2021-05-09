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
#include "Log.h"

#define DEFAULT_MESSAGE 907
#define ACTION_CONFIRM_TEXT_SPANISH "¿Estas seguro de que quieres obtener el abalorio?"
#define ACTION_CONFIRM_TEXT_ENGLISH "Are you sure you want to take the trinket?"
#define ACTION_ERROR_NO_SPACE_TEXT_SPANISH "Debes desequipar al menos un abalorio para obtener este objeto."
#define ACTION_ERROR_NO_SPACE_TEXT_ENGLISH "You must at least unequip one trinket to get this item."
#define ACTION_ERROR_TEXT_SPANISH "Ha habido un problema equipando el abalorio."
#define ACTION_ERROR_TEXT_ENGLISH "There was a problem equipping the trinket."
#define ACTION_ERROR_ALREADY_HAVE_TEXT_SPANISH "Solo se puede tener un abalorio de este tipo. Revisa tus bolsas y banco personal."
#define ACTION_ERROR_ALREADY_HAVE_TEXT_ENGLISH "You can only have one trinket of this type. Check your bags and personal bank."



enum TocTrinkets
{
    TRINKET_ALLIANCE_DEATH_CHOICE = 47464,
    TRINKET_ALLIANCE_REIGN_OF_THE_DEAD = 47477,
    TRINKET_ALLIANCE_SOLACE_OF_THE_FALLEN = 47432,
    TRINKET_ALLIANCE_JUGGERNAUT_VITALITY = 47451,

    TRINKET_HORDE_DEATH_VEREDICT = 47131,
    TRINKET_HORDE_REIGN_OF_THE_UNLIVING = 47188,
    TRINKET_HORDE_SOLACE_OF_THE_FALLEN = 47059,
    TRINKET_HORDE_SATRINA_IMPEDING_SCARAB = 47088,
};


bool IsVipPlayer(Player* player)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT AccountId FROM premium WHERE active = 1 AND AccountId = %u", player->GetSession()->GetAccountId());
    bool isVipPlayer = false;
    if (result)
        isVipPlayer = true;
    return isVipPlayer;
}

void TeleportToShop(Player* player, bool isSpanish)
{
    player->GetSession()->SendNotification(isSpanish ? "Que haces aqui? No eres VIP! - Has sido reportado a un GM!" : "Why are you here?  You are not a VIP! - You have been reported to a GM!");
    player->TeleportTo(1, -11823.9f, -4779.58f, 5.9206f, 1.1357f);
    sLog->outError("Player::VipVendorWithoutPrivileges: Possible hacking attempt: Account %u tried to buy in VIP vendor. He has been teleported to Shop.", player->GetSession()->GetAccountId());

    if (player && player->GetSession() && AccountMgr::IsPlayerAccount(player->GetSession()->GetSecurity()))
    {
        std::string str = "";
        str = "|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + std::string(player->GetName().c_str()) + "|cFF00FFFF] Possible cheater! Tried to buy in VIP vendor. He has been teleported to Shop";
        WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
        data << str;
        sWorld->SendGlobalGMMessage(&data);
    }
}

class custom_vip_only_npc : public CreatureScript
{
public:
    custom_vip_only_npc() : CreatureScript("npc_vip") { }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!player)
            return false;

        if (IsVipPlayer(player))
        {
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "VIP VENDOR", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_VENDOR);
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "BYE", GOSSIP_SENDER_MAIN, 1);
        }
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (sender == GOSSIP_SENDER_MAIN)
        {
            bool isSpanish = IsSpanishPlayer(player);

            switch (action)
            {
            case GOSSIP_OPTION_VENDOR:
                player->GetSession()->SendListInventory(creature->GetGUID());
                CloseGossipMenuFor(player);
                break;
            case 1:
                TeleportToShop(player, isSpanish);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

    struct custom_vip_only_npcAI : public ScriptedAI
    {
        custom_vip_only_npcAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_vip_only_npcAI(creature);
    }
};

class custom_vip_only_npc_talk : public CreatureScript
{
public:
    custom_vip_only_npc_talk() : CreatureScript("npc_vip_talk") { }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!player)
            return false;

        if (IsVipPlayer(player))
        {
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "VIP", GOSSIP_SENDER_MAIN, 2);
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "BYE", GOSSIP_SENDER_MAIN, 1);
        }
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (sender == GOSSIP_SENDER_MAIN)
        {
            bool isSpanish = IsSpanishPlayer(player);

            switch (action)
            {
            case 2:
                player->PrepareQuestMenu(creature->GetGUID());
                player->SendPreparedQuest(creature->GetGUID());
                CloseGossipMenuFor(player);
                break;
            case 1:
                TeleportToShop(player, isSpanish);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

    struct custom_vip_only_npc_talkAI : public ScriptedAI
    {
        custom_vip_only_npc_talkAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_vip_only_npc_talkAI(creature);
    }
};


class custom_vip_toc_trinkets_npc : public CreatureScript
{
public:
    custom_vip_toc_trinkets_npc() : CreatureScript("npc_vip_toc_trinkets") { }

    uint32 GetTrinketEquipmentSlot(Player* player)
    {
        if (!player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_TRINKET1))
        {
            return EQUIPMENT_SLOT_TRINKET1;
        }
        else if (!player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_TRINKET2))
        {
            return EQUIPMENT_SLOT_TRINKET2;
        }
        else
        {
            return NULL_SLOT;
        }
    }

    void EquipItem(Player* player, uint32 itemEntry)
    {
        bool isSpanish = IsSpanishPlayer(player);

        if (player->HasItemCount(itemEntry, 1, true))
        {
			player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_ALREADY_HAVE_TEXT_SPANISH : ACTION_ERROR_ALREADY_HAVE_TEXT_ENGLISH);
            return;
        }

        uint32 slot = GetTrinketEquipmentSlot(player);
        if (slot != NULL_SLOT)
        {
            //uint16 eDest;
            //InventoryResult msg = player->CanEquipNewItem(NULL_SLOT, eDest, itemEntry, false);
            //if (msg == EQUIP_ERR_OK)
            //{
                player->EquipNewItem(slot, itemEntry, true);
            //}
            //else
            //{
            //    std::string errorText = isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH;
            //    player->GetSession()->SendNotification(errorText.c_str());
            //}
        }
        else
        {
            player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_NO_SPACE_TEXT_SPANISH : ACTION_ERROR_NO_SPACE_TEXT_ENGLISH);
        }
    }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!player)
            return false;

        bool isSpanish = IsSpanishPlayer(player);

        if (player->IsInCombat())
        {
            player->GetSession()->SendNotification(isSpanish ? "Error! Estas en combate." : "Failure! You are in combat.");
            return false;
        }

        CloseGossipMenuFor(player);

        if (!IsVipPlayer(player))
        {
            TeleportToShop(player, isSpanish);
            return false;
        }
        else
        {
            std::string confirmText = isSpanish ? ACTION_CONFIRM_TEXT_SPANISH : ACTION_CONFIRM_TEXT_ENGLISH;

            if (player->GetTeamId() == TEAM_ALLIANCE)
            {
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_misc_bone_skull_02:20|t [Veredicto de la Muerte]" : "|TInterface\\icons\\inv_misc_bone_skull_02:20|t [Death's Verdict]",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4, confirmText, 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_crown_13:20|t [Reino de los sin vida]" : "|TInterface\\icons\\inv_crown_13:20|t [Reign of the Unliving]",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5, confirmText, 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\achievement_dungeon_ulduar77_25man:20|t [Consuelo de los derrotados]" : "|TInterface\\icons\\achievement_dungeon_ulduar77_25man:20|t [Solace of the Defeated]",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6, confirmText, 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_scarab_crystal:20|t [Escarabajo trabador de Satrina]" : "|TInterface\\icons\\inv_scarab_crystal:20|t [Satrina's Impeding Scarab]",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7, confirmText, 0, false);

                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
            }
            else if (player->GetTeamId() == TEAM_HORDE)
            {
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_misc_bone_skull_02:20|t [Eleccion de la muerte]" : "|TInterface\\icons\\inv_misc_bone_skull_02:20|t [Death's Choice]",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF, confirmText, 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_crown_13:20|t [Reino de los muertos]" : "|TInterface\\icons\\inv_crown_13:20|t [Reign of the Dead]",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, confirmText, 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\achievement_dungeon_ulduar77_25man:20|t [Consuelo de los caidos]" : "|TInterface\\icons\\achievement_dungeon_ulduar77_25man:20|t [Solace of the Fallen]",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2, confirmText, 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_scarab_crystal:20|t [Vitalidad de gigante]" : "|TInterface\\icons\\inv_scarab_crystal:20|t [Juggernaut's Vitality]",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3, confirmText, 0, false);
                
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
            }
            else
            {
                CloseGossipMenuFor(player);
                return false;
            }
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
                EquipItem(player, TRINKET_ALLIANCE_DEATH_CHOICE);
                CloseGossipMenuFor(player);
                break;
            case GOSSIP_ACTION_INFO_DEF + 1:
                EquipItem(player, TRINKET_ALLIANCE_REIGN_OF_THE_DEAD);
                CloseGossipMenuFor(player);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                EquipItem(player, TRINKET_ALLIANCE_SOLACE_OF_THE_FALLEN);
                CloseGossipMenuFor(player);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                EquipItem(player, TRINKET_ALLIANCE_JUGGERNAUT_VITALITY);
                CloseGossipMenuFor(player);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                EquipItem(player, TRINKET_HORDE_DEATH_VEREDICT);
                CloseGossipMenuFor(player);
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                EquipItem(player, TRINKET_HORDE_REIGN_OF_THE_UNLIVING);
                CloseGossipMenuFor(player);
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                EquipItem(player, TRINKET_HORDE_SOLACE_OF_THE_FALLEN);
                CloseGossipMenuFor(player);
                break;
            case GOSSIP_ACTION_INFO_DEF + 7:
                EquipItem(player, TRINKET_HORDE_SATRINA_IMPEDING_SCARAB);
                CloseGossipMenuFor(player);
                break;
            default:
                OnGossipHello(player, creature);
                break;
            }
        }

        return true;
    }

    struct custom_vip_toc_trinkets_npcAI : public ScriptedAI
    {
        custom_vip_toc_trinkets_npcAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_vip_toc_trinkets_npcAI(creature);
    }
};


class custom_npc_ashen_veredict_rep : public CreatureScript
{
public:
    custom_npc_ashen_veredict_rep() : CreatureScript("npc_ashen_veredict_rep") { }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }


    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!player)
            return false;

        bool isSpanish = IsSpanishPlayer(player);

        if (player->IsInCombat())
        {
            player->GetSession()->SendNotification(isSpanish ? "Error! Estas en combate." : "Failure! You are in combat.");
            return false;
        }

        if (!IsVipPlayer(player))
        {
            TeleportToShop(player, isSpanish);
            return false;
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Otener reputacion con El Veredicto Cinereo." : "Get The Ashen Verdict reputation.", GOSSIP_SENDER_MAIN, 4,
                isSpanish ? "Desea subir la reputacion con El Veredicto Cinereo a exaltado?" : "Want to raise your reputation with The Ashen Verdict to Exalted?", 0, false);
            SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
            return true;
        }
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (sender == GOSSIP_SENDER_MAIN)
        {
            bool isSpanish = IsSpanishPlayer(player);

            switch (action)
            {
            case 4: // The Ashen Veredict rep
                player->SetReputation(1156, 999999);
                player->GetSession()->SendNotification(isSpanish ? "|cffFFFF00SERVICIOS \n |cffFFFFFFAhora eres exaltado con El Veredicto Cinereo!"
                    : "|cffFFFF00SERVICES \n |cffFFFFFFYou are now exalted with The Ashen Veredict!");
                player->CastSpell(player, 31726);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

    struct custom_npc_ashen_veredict_repAI : public ScriptedAI
    {
        custom_npc_ashen_veredict_repAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_npc_ashen_veredict_repAI(creature);
    }
};

void AddSC_custom_vip_only_npc()
{
    new custom_vip_only_npc();
    new custom_vip_toc_trinkets_npc();
    new custom_vip_only_npc_talk();
    new custom_npc_ashen_veredict_rep();
}
