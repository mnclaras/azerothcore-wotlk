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
        AddGossipItemFor(player, 9, "[Coin Exchange] ->", GOSSIP_SENDER_MAIN, 196);
        player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        bool isSpanish = IsSpanishPlayer(player);
        std::string confirmText = isSpanish ? ACTION_CONFIRM_TEXT_SPANISH : ACTION_CONFIRM_TEXT_ENGLISH;

        QueryResult result;
        bool isViplayer = false;

        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {

            case -1:
                SendMenu(player, creature);
                break;

            case 196:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Comprar 1 Amnesia Coin usando:" : "Buy 1 Amnesia Coin using:", GOSSIP_SENDER_MAIN, -1);

                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblema de escarcha] x 10." : "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblem of Frost] x 10.",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF, confirmText, 0, false);

                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\achievement_arena_2v2_2:20|t [Puntos de arena] x 250." : "|TInterface\\icons\\achievement_arena_2v2_2:20|t [Arena Points] x 250.",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, confirmText, 0, false);

                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\achievement_arena_2v2_7:20|t [Puntos de honor] x 20000." : "|TInterface\\icons\\achievement_arena_2v2_7:20|t [Honor Points] x 20000.",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2, confirmText, 0, false);


                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Usar 1 Amnesia Coin para comprar:" : "Use 1 Amnesia Coin to buy:", GOSSIP_SENDER_MAIN, -1);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblema de escarcha] x 5." : "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblem of Frost] x 5.",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3, confirmText, 0, false);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\achievement_arena_2v2_7:20|t [Puntos de honor] x 10000." : "|TInterface\\icons\\achievement_arena_2v2_7:20|t [Honor points] x 10000.",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4, confirmText, 0, false);

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Comprar 50 Amnesia Coins usando:" : "Buy 50 Amnesia Coins using:", GOSSIP_SENDER_MAIN, -1);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_misc_apexis_crystal:20|t [Legendary token] x 1." : "|TInterface\\icons\\inv_misc_apexis_crystal:20|t [Legendary token] x 1.",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5, confirmText, 0, false);

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Usar 100 Amnesia Coins para comprar:" : "Use 100 Amnesia Coins to buy:", GOSSIP_SENDER_MAIN, -1);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_misc_apexis_crystal:20|t [Legendary token] x 1." : "|TInterface\\icons\\inv_misc_apexis_crystal:20|t [Legendary token] x 1.",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6, confirmText, 0, false);


                result = CharacterDatabase.PQuery("SELECT AccountId FROM premium WHERE active = 1 AND AccountId = %u", player->GetSession()->GetAccountId());
                if (result) {
                    isViplayer = true;
                }

                if (isViplayer)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Usar 10 Emblemas de Triunfo para comprar:" : "Use 10 Emblem of Triumph to buy:", GOSSIP_SENDER_MAIN, -1);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                        isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblema de escarcha] x 1." : "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblem of Frost] x 1.",
                        GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7, confirmText, 0, false);
                }
                else
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Usar 15 Emblemas de Triunfo para comprar:" : "Use 15 Emblem of Triumph to buy:", GOSSIP_SENDER_MAIN, -1);
                    AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                        isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblema de escarcha] x 1." : "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblem of Frost] x 1.",
                        GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9, confirmText, 0, false);
                }

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Comprar 25 Creditos Etereos usando:" : "Buy 25 Ethereal Credits using:", GOSSIP_SENDER_MAIN, -1);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblema de escarcha] x 50." : "|TInterface\\icons\\inv_misc_frostemblem_01:20|t [Emblem of Frost] x 50.",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10, confirmText, 0, false);

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Comprar 100 Puntos de arena usando:" : "Buy 100 Arena points using:", GOSSIP_SENDER_MAIN, -1);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR,
                    isSpanish ? "|TInterface\\icons\\achievement_arena_2v2_7:20|t [Puntos de honor] x 10000." : "|TInterface\\icons\\achievement_arena_2v2_7:20|t [Honor points] x 10000.",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11, confirmText, 0, false);

                AddGossipItemFor(player, GOSSIP_ICON_TALK,
                    isSpanish ? "|TInterface/ICONS/Thrown_1H_Harpoon_D_01Blue:20|t Hasta Luego!" : "|TInterface/ICONS/Thrown_1H_Harpoon_D_01Blue:20|t Nevermind!",
                    GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

                player->PlayerTalkClass->SendGossipMenu(1, creature->GetGUID());
                break;

            case GOSSIP_ACTION_INFO_DEF:
                // Buy 1 Amnesia Coin with 10 Emblem of Frost
                DoExchange(player, EMBLEM_OF_FROST_ENTRY, 10, AMNESIA_COIN_ENTRY, 1);
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 1:
                // Buy 1 Amnesia Coin with 250 Arena points
                if (player->GetArenaPoints() >= 250) {
                    player->ModifyArenaPoints(-250);
                    bool ok = AddItem(player, AMNESIA_COIN_ENTRY, 1);
                    if (ok)
                    {
                        player->GetSession()->SendNotification(isSpanish ? ACTION_COMPLETE_TEXT_SPANISH : ACTION_COMPLETE_TEXT_ENGLISH);
                    }
                    else
                    {
                        player->ModifyArenaPoints(250);
                        player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                    }
                }
                else
                {
                    player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                }
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 2:
                // Buy 1 Amnesia Coin with 20000 Honor points
                if (player->GetHonorPoints() >= 20000)
                {
                    player->ModifyHonorPoints(-20000);
                    bool ok = AddItem(player, AMNESIA_COIN_ENTRY, 1);
                    if (ok)
                    {
                        player->GetSession()->SendNotification(isSpanish ? ACTION_COMPLETE_TEXT_SPANISH : ACTION_COMPLETE_TEXT_ENGLISH);
                    }
                    else
                    {
                        player->ModifyHonorPoints(20000);
                        player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                    }
                }
                else
                {
                    player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                }
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 3:
                // Buy 5 Emblem of Frost with 1 Amnesia Coin
                DoExchange(player, AMNESIA_COIN_ENTRY, 1, EMBLEM_OF_FROST_ENTRY, 5);
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 4:
                // Buy 10000 Honor points with 1 Amnesia Coin
                if (HasItem(player, AMNESIA_COIN_ENTRY, 1))
                {
                    DestroyItem(player, AMNESIA_COIN_ENTRY, 1);
                    player->ModifyHonorPoints(10000);
                    player->GetSession()->SendNotification(isSpanish ? ACTION_COMPLETE_TEXT_SPANISH : ACTION_COMPLETE_TEXT_ENGLISH);
                }
                else
                {
                    player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                }
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 5:
                // Buy 50 Amnesia Coin with 1 Legendary Token
                DoExchange(player, LEGENDARY_TOKEN_ENTRY, 1, AMNESIA_COIN_ENTRY, 50);
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 6:
                // Buy 1 Legendary Token with 100 Amnesia Coin
                DoExchange(player, AMNESIA_COIN_ENTRY, 100, LEGENDARY_TOKEN_ENTRY, 1);
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 7:
                // Buy 1 Emblem of Frost with 10 Emblem of Triumph
                DoExchange(player, EMBLEM_OF_TRIUMPH_ENTRY, 10, EMBLEM_OF_FROST_ENTRY, 1);
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 8:
                player->PlayerTalkClass->SendCloseGossip();
                break;

            case GOSSIP_ACTION_INFO_DEF + 9:
                // Buy 1 Emblem of Frost with 15 Emblem of Triumph
                DoExchange(player, EMBLEM_OF_TRIUMPH_ENTRY, 15, EMBLEM_OF_FROST_ENTRY, 1);
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 10:
                // Buy 25 Ethereal Credit with 50 Emblem of Frost
                DoExchange(player, EMBLEM_OF_FROST_ENTRY, 50, ETHEREAL_CREDIT_ENTRY, 25);
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 11:
                // Buy 100 Arena points with 10000 Honor Points
                if (player->GetHonorPoints() >= 10000)
                {
                    player->ModifyHonorPoints(-10000);
                    player->ModifyArenaPoints(100);
                    player->GetSession()->SendNotification(isSpanish ? ACTION_COMPLETE_TEXT_SPANISH : ACTION_COMPLETE_TEXT_ENGLISH);
                }
                else
                {
                    player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                }
                //player->PlayerTalkClass->SendCloseGossip();
                SendMenu(player, creature);
                break;
            }

        }
        return true;
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
