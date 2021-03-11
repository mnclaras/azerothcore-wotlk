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

#include "AuctionHouseMgr.h"
#include "AccountMgr.h"
#include "Bag.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "Item.h"
#include "Language.h"
#include "Log.h"
#include "Mail.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "ScriptMgr.h"
#include "CellImpl.h"
#include "GridNotifiersImpl.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Chat.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "TemporarySummon.h"
#include "GossipDef.h"

#define LOCALE_esES LOCALE_esES
#define LOCALE_esMX LOCALE_esMX

#define ACTION_CONFIRM_TEXT_SPANISH "¿Seguro que quieres realizar el intercambio?"
#define ACTION_CONFIRM_TEXT_ENGLISH "Are you sure you want to make the exchange?"

#define ACTION_ERROR_TEXT_SPANISH "No puedes realizar esta transaccion."
#define ACTION_ERROR_TEXT_ENGLISH "You can't make this transaction."

#define ACTION_COMPLETE_TEXT_SPANISH "Transaccion completada."
#define ACTION_COMPLETE_TEXT_ENGLISH "Transaction completed."


#define EMBLEM_OF_FROST_ENTRY 49426
#define EMBLEM_OF_TRIUMPH_ENTRY 47241
#define ETHEREAL_CREDIT_ENTRY 38186
#define AMNESIA_COIN_ENTRY 32569
#define LEGENDARY_TOKEN_ENTRY 32572

class custom_amnesia_coin_exchange : public CreatureScript
{
public:
    custom_amnesia_coin_exchange() : CreatureScript("npc_amnesia_coin_exc") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        SendMenu(player, creature);
        return true;
    }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool HasItem(Player* plr, uint32 itemId, uint32 quantity)
    {
        return plr->HasItemCount(itemId, quantity);
    }

    void DestroyItem(Player* plr, uint32 itemId, uint32 quantity)
    {
        plr->DestroyItemCount(itemId, quantity, true);
    }

    bool AddItem(Player* plr, uint32 itemId, uint32 quantity)
    {
        return plr->AddItem(itemId, quantity);
    }

    void DoExchange(Player* player, uint32 sourceItemId, uint32 sourceAmount, uint32 destinationItemId, uint32 destinationAmount) {
        bool isSpanish = IsSpanishPlayer(player);

        if (HasItem(player, sourceItemId, sourceAmount))
        {
            DestroyItem(player, sourceItemId, sourceAmount);

            bool ok = AddItem(player, destinationItemId, destinationAmount);
            if (ok)
            {
                player->GetSession()->SendNotification(isSpanish ? ACTION_COMPLETE_TEXT_SPANISH : ACTION_COMPLETE_TEXT_ENGLISH);
            }
            else
            {
                AddItem(player, sourceItemId, sourceAmount);
                player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
            }
        }
        else
        {
            player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
        }
    }

    void SendMenu(Player* player, Creature* creature)
    {
        bool isSpanish = IsSpanishPlayer(player);
        std::string confirmTexProduces = isSpanish ? "Cada unidad te otorga " : "Every unit gives you ";
        std::string confirmTextConsume = isSpanish ? " y consume " : " and consumes ";

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Comprar Amnesia Coin usando:" : "Buy Amnesia Coins using:", GOSSIP_SENDER_MAIN, -1);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT,
            isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 10x[Emblema de escarcha] => 1x[Amnesia Coin]"
            :           "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 10x[Emblem of Frost] => 1x[Amnesia Coin]",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF,
            isSpanish ? confirmTexProduces + "1x[Amnesia Coin]" + confirmTextConsume + "10x[Emblema de escarcha]"
            :           confirmTexProduces + "1x[Amnesia Coin]" + confirmTextConsume + "10x[Emblem of Frost]",
            0, true);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
            isSpanish ? "|TInterface\\icons\\achievement_arena_2v2_2:20|t 250x[Puntos de arena] => 1x[Amnesia Coin]"
            :           "|TInterface\\icons\\achievement_arena_2v2_2:20|t 250x[Arena Points] => 1x[Amnesia Coin]",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1,
            isSpanish ? confirmTexProduces + "1x[Amnesia Coin]" + confirmTextConsume + "250x[Puntos de arena]"
            :           confirmTexProduces + "1x[Amnesia Coin]" + confirmTextConsume + "250x[Arena Points]",
            0, true);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
            isSpanish ? "|TInterface\\icons\\achievement_arena_2v2_7:20|t 20000x[Puntos de honor] => 1x[Amnesia Coin]"
            :           "|TInterface\\icons\\achievement_arena_2v2_7:20|t 20000x[Honor Points] => 1x[Amnesia Coin]",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2,
            isSpanish ? confirmTexProduces + "1x[Amnesia Coin]" + confirmTextConsume + "20000x[Puntos de honor]"
            :           confirmTexProduces + "1x[Amnesia Coin]" + confirmTextConsume + "20000x[Honor Points]",
            0, true);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
            isSpanish ? "|TInterface\\icons\\inv_misc_apexis_crystal:20|t 1x[Legendary token] => 50x[Amnesia Coin]"
            :           "|TInterface\\icons\\inv_misc_apexis_crystal:20|t 1x[Legendary token] => 50x[Amnesia Coin]",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5,
            isSpanish ? confirmTexProduces + "50x[Amnesia Coin]" + confirmTextConsume + "1x[Legendary token]"
            :           confirmTexProduces + "50x[Amnesia Coin]" + confirmTextConsume + "1x[Legendary token]",
            0, true);


        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Usar Amnesia Coin para comprar:" : "Use Amnesia Coin to buy:", GOSSIP_SENDER_MAIN, -1);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
            isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 1x[Amnesia Coin] => 5x[Emblema de escarcha]"
            :           "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 1x[Amnesia Coin] => 5x[Emblem of Frost]",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3,
            isSpanish ? confirmTexProduces + "5x[Emblema de escarcha]" + confirmTextConsume + "1x[Amnesia Coin]"
            :           confirmTexProduces + "5x[Emblem of Frost]" + confirmTextConsume + "1x[Amnesia Coin]",
            0, true);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
            isSpanish ? "|TInterface\\icons\\achievement_arena_2v2_7:20|t 1x[Amnesia Coin] => 10000x[Puntos de honor]"
            :           "|TInterface\\icons\\achievement_arena_2v2_7:20|t 1x[Amnesia Coin] => 10000x[Honor points]",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4,
            isSpanish ? confirmTexProduces + "10000x[Puntos de honor]" + confirmTextConsume + "1x[Amnesia Coin]"
            :           confirmTexProduces + "10000x[Honor points]" + confirmTextConsume + "1x[Amnesia Coin]",
            0, true);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
            isSpanish ? "|TInterface\\icons\\inv_misc_apexis_crystal:20|t 100x[Amnesia Coin] => 1x[Legendary token]"
            :           "|TInterface\\icons\\inv_misc_apexis_crystal:20|t 100x[Amnesia Coin] => 1x[Legendary token]",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6,
            isSpanish ? confirmTexProduces + "1x[Legendary token]" + confirmTextConsume + "100x[Amnesia Coin]"
            :           confirmTexProduces + "1x[Legendary token]" + confirmTextConsume + "100x[Amnesia Coin]",
            0, true);


        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Usar Emblemas de Triunfo para comprar:" : "Use Emblem of triumph to buy:", GOSSIP_SENDER_MAIN, -1);
        QueryResult result = CharacterDatabase.PQuery("SELECT AccountId FROM premium WHERE active = 1 AND AccountId = %u", player->GetSession()->GetAccountId());
        if (result)
        {
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 10x[Emblema de Triunfo] => 1x[Emblema de escarcha]"
                :           "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 10x[Emblem of triumph] => 1x[Emblem of Frost]",
                GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7,
                isSpanish ? confirmTexProduces + "1x[Emblema de escarcha]" + confirmTextConsume + "10x[Emblem of triumph]"
                :           confirmTexProduces + "1x[Emblem of Frost]" + confirmTextConsume + "10x[Emblem of triumph]",
                0, true);
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 15x[Emblema de Triunfo] => 1x[Emblema de escarcha]"
                :           "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 15x[Emblem of triumph] => 1x[Emblem of Frost]",
                GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9,
                isSpanish ? confirmTexProduces + "1x[Emblema de escarcha]" + confirmTextConsume + "15x[Emblem of triumph]"
                :           confirmTexProduces + "1x[Emblem of Frost]" + confirmTextConsume + "15x[Emblem of triumph]",
                0, true);
        }


        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Comprar Creditos Etereos usando:" : "Buy Ethereal Credits using:", GOSSIP_SENDER_MAIN, -1);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
            isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 50x[Emblema de escarcha] => 25x[Credito Etereo]"
            :           "|TInterface\\icons\\inv_misc_frostemblem_01:20|t 50x[Emblem of Frost] => 25x[Ethereal Credit]",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10,
            isSpanish ? confirmTexProduces + "25x[Credito Etereo]" + confirmTextConsume + "50x[Emblema de escarcha]"
            :           confirmTexProduces + "25x[Ethereal Credit]" + confirmTextConsume + "50x[Emblem of Frost]",
            0, true);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Comprar Puntos de arena usando:" : "Buy Arena points using:", GOSSIP_SENDER_MAIN, -1);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
            isSpanish ? "|TInterface\\icons\\achievement_arena_2v2_7:20|t 10000x[Puntos de honor] => 100x[Puntos de arena]"
            :           "|TInterface\\icons\\achievement_arena_2v2_7:20|t 10000x[Honor points] => 100x[Arena Points]",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11,
            isSpanish ? confirmTexProduces + "100x[Puntos de arena]" + confirmTextConsume + "10000x[Puntos de honor] "
            :           confirmTexProduces + "100x[Arena Points]" + confirmTextConsume + "10000x[Honor points]",
            0, true);

        AddGossipItemFor(player, GOSSIP_ICON_TALK,
            isSpanish ? "|TInterface/ICONS/Thrown_1H_Harpoon_D_01Blue:20|t Hasta Luego!"
            :           "|TInterface/ICONS/Thrown_1H_Harpoon_D_01Blue:20|t Nevermind!",
            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

        SendGossipMenuFor(player, 1, creature);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF + 8: { CloseGossipMenuFor(player); break; }
            default: { SendMenu(player, creature); break; }
            }
        }
        else
        {
            SendMenu(player, creature);
        }

        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override
    {
        ClearGossipMenuFor(player);

        if (sender == GOSSIP_SENDER_MAIN)
        {
            bool isSpanish = IsSpanishPlayer(player);
            const char* quantity = code;
            uint32_t amount = atoi(quantity);
            if (amount > 0) {
                switch (action)
                {
                case GOSSIP_ACTION_INFO_DEF: // Buy 1 Amnesia Coin with 10 Emblem of Frost
                {
                    DoExchange(player, EMBLEM_OF_FROST_ENTRY, 10 * amount, AMNESIA_COIN_ENTRY, 1 * amount);
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 1: // Buy 1 Amnesia Coin with 250 Arena points
                {
                    if (player->GetArenaPoints() >= 250 * amount) {
                        player->ModifyArenaPoints(-250 * amount);
                        bool ok = AddItem(player, AMNESIA_COIN_ENTRY, 1 * amount);
                        if (ok)
                        {
                            player->GetSession()->SendNotification(isSpanish ? ACTION_COMPLETE_TEXT_SPANISH : ACTION_COMPLETE_TEXT_ENGLISH);
                        }
                        else
                        {
                            player->ModifyArenaPoints(250 * amount);
                            player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                        }
                    }
                    else
                    {
                        player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                    }
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 2: // Buy 1 Amnesia Coin with 20000 Honor points
                {
                    if (player->GetHonorPoints() >= 20000 * amount)
                    {
                        player->ModifyHonorPoints(-20000 * amount);
                        bool ok = AddItem(player, AMNESIA_COIN_ENTRY, 1 * amount);
                        if (ok)
                        {
                            player->GetSession()->SendNotification(isSpanish ? ACTION_COMPLETE_TEXT_SPANISH : ACTION_COMPLETE_TEXT_ENGLISH);
                        }
                        else
                        {
                            player->ModifyHonorPoints(20000 * amount);
                            player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                        }
                    }
                    else
                    {
                        player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                    }
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 3: // Buy 5 Emblem of Frost with 1 Amnesia Coin
                {          
                    DoExchange(player, AMNESIA_COIN_ENTRY, 1 * amount, EMBLEM_OF_FROST_ENTRY, 5 * amount);
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 4: // Buy 10000 Honor points with 1 Amnesia Coin
                {     
                    if (HasItem(player, AMNESIA_COIN_ENTRY, 1 * amount))
                    {
                        DestroyItem(player, AMNESIA_COIN_ENTRY, 1 * amount);
                        player->ModifyHonorPoints(10000 * amount);
                        player->GetSession()->SendNotification(isSpanish ? ACTION_COMPLETE_TEXT_SPANISH : ACTION_COMPLETE_TEXT_ENGLISH);
                    }
                    else
                    {
                        player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                    }
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 5: // Buy 50 Amnesia Coin with 1 Legendary Token
                {
                    DoExchange(player, LEGENDARY_TOKEN_ENTRY, 1 * amount, AMNESIA_COIN_ENTRY, 50 * amount);
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 6: // Buy 1 Legendary Token with 100 Amnesia Coin
                {            
                    DoExchange(player, AMNESIA_COIN_ENTRY, 100 * amount, LEGENDARY_TOKEN_ENTRY, 1 * amount);
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 7: // Buy 1 Emblem of Frost with 10 Emblem of Triumph
                {
                    DoExchange(player, EMBLEM_OF_TRIUMPH_ENTRY, 10 * amount, EMBLEM_OF_FROST_ENTRY, 1 * amount);
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 8: // Close menu
                {
                    CloseGossipMenuFor(player);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 9: // Buy 1 Emblem of Frost with 15 Emblem of Triumph
                {
                    DoExchange(player, EMBLEM_OF_TRIUMPH_ENTRY, 15 * amount, EMBLEM_OF_FROST_ENTRY, 1 * amount);
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 10: // Buy 25 Ethereal Credit with 50 Emblem of Frost
                { 
                    DoExchange(player, EMBLEM_OF_FROST_ENTRY, 50 * amount, ETHEREAL_CREDIT_ENTRY, 25 * amount);
                    SendMenu(player, creature);
                    return true;
                }
                case GOSSIP_ACTION_INFO_DEF + 11: // Buy 100 Arena points with 10000 Honor Points
                {      
                    if (player->GetHonorPoints() >= 10000 * amount)
                    {
                        player->ModifyHonorPoints(-10000 * amount);
                        player->ModifyArenaPoints(100 * amount);
                        player->GetSession()->SendNotification(isSpanish ? ACTION_COMPLETE_TEXT_SPANISH : ACTION_COMPLETE_TEXT_ENGLISH);
                    }
                    else
                    {
                        player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                    }
                    SendMenu(player, creature);
                    return true;
                }
                default:
                {
                    SendMenu(player, creature);
                    return true;
                }
                }
            }
            else
            {
                SendMenu(player, creature);
                return true;
            }
        }
        else
        {
            SendMenu(player, creature);
            return true;
        }
        return false;
    }

	struct custom_amnesia_coin_exchangeAI : public ScriptedAI
	{
        custom_amnesia_coin_exchangeAI(Creature* creature) : ScriptedAI(creature) { }
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new custom_amnesia_coin_exchangeAI(creature);
	}
};

void AddSC_custom_amnesia_coin_exchange()
{
	new custom_amnesia_coin_exchange();
}
