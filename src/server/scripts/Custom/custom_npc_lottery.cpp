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
#include "Chat.h"
#include "ScriptMgr.h"
#include "CellImpl.h"
#include "GridNotifiersImpl.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "TemporarySummon.h"
#include "GossipDef.h"

#define LOCALE_esES LOCALE_esES
#define LOCALE_esMX LOCALE_esMX

#define ACTION_CONFIRM_TEXT_SPANISH "¿Seguro que quieres realizar la apuesta?"
#define ACTION_CONFIRM_TEXT_ENGLISH "Are you sure you want to make the bet?"

#define ACTION_ERROR_TEXT_SPANISH "No puedes realizar esta apuesta."
#define ACTION_ERROR_TEXT_ENGLISH "You can't make this bet."

#define ACTION_WIN_TEXT_SPANISH "Vaya vaya, ha salido cara! Disfruta de tu recompensa."
#define ACTION_WIN_TEXT_ENGLISH "Oh well, it was heads! Enjoy your reward."

#define ACTION_LOSE_TEXT_SPANISH "Ohhhhh, ha salido cruz! La suerte no esta contigo."
#define ACTION_LOSE_TEXT_ENGLISH "Ohhhhh, it was tails! Luck is not with you."


#define EMBLEM_OF_FROST_ENTRY 49426
#define EMBLEM_OF_TRIUMPH_ENTRY 47241
//#define AMNESIA_COIN_ENTRY 37742
//#define LEGENDARY_TOKEN_ENTRY 37711
#define AMNESIA_COIN_ENTRY 32569
#define LEGENDARY_TOKEN_ENTRY 32572

class custom_npc_lottery : public CreatureScript
{
public:
    custom_npc_lottery() : CreatureScript("npc_lottery") { }

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

    void SendMenu(Player* player, Creature* creature)
    {
        bool isSpanish = IsSpanishPlayer(player);
        std::string confirmText = isSpanish ? ACTION_CONFIRM_TEXT_SPANISH : ACTION_CONFIRM_TEXT_ENGLISH;

        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "1 Amnesia Coin.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF, confirmText, 0, false);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "5 Amnesia Coin.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, confirmText, 0, false);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "10 Amnesia Coin.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2, confirmText, 0, false);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, isSpanish ? "Hasta Luego!" : "Nevermind!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        SendGossipMenuFor(player, 105000, creature->GetGUID());
    }

    bool DiceRoll()
    {
        uint32 diceroll = urand(0, 1);
        return (diceroll == 0);
    }

    bool HasItem(Player* plr, uint32 itemId, uint32 quantity)
    {
        return plr->HasItemCount(itemId, quantity);
    }

    bool AddItem(Player* plr, uint32 itemId, uint32 quantity)
    {
        return plr->AddItem(itemId, quantity);
    }

    void DestroyItem(Player* plr, uint32 itemId, uint32 quantity)
    {
        plr->DestroyItemCount(itemId, quantity, true);
    }

    void DoExchange(Player* player, uint32 sourceItemId, uint32 sourceAmount, uint32 destinationItemId, uint32 destinationAmount, bool win) {
        bool isSpanish = IsSpanishPlayer(player);
        if (HasItem(player, sourceItemId, sourceAmount))
        {
            DestroyItem(player, sourceItemId, sourceAmount);

            if (win)
            {
                bool ok = AddItem(player, destinationItemId, destinationAmount);
                if (ok)
                {
                    player->GetSession()->SendNotification(isSpanish ? ACTION_WIN_TEXT_SPANISH : ACTION_WIN_TEXT_ENGLISH);
                }
                else
                {
                    AddItem(player, sourceItemId, sourceAmount);
                    player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
                }
            }
            else
            {
                player->GetSession()->SendNotification(isSpanish ? ACTION_LOSE_TEXT_SPANISH : ACTION_LOSE_TEXT_ENGLISH);
            }
        }
        else
        {
            player->GetSession()->SendNotification(isSpanish ? ACTION_ERROR_TEXT_SPANISH : ACTION_ERROR_TEXT_ENGLISH);
        }
    }

    void DoLottery(uint32 itemID, int betItemCount, Player* player)
    {
        bool win = DiceRoll();
        DoExchange(player, itemID, betItemCount, itemID, betItemCount * 2, win);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        bool isSpanish = IsSpanishPlayer(player);
        std::string confirmText = isSpanish ? ACTION_CONFIRM_TEXT_SPANISH : ACTION_CONFIRM_TEXT_ENGLISH;
        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case GOSSIP_ACTION_INFO_DEF:
                DoLottery(AMNESIA_COIN_ENTRY, 1, player);
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 1:
                DoLottery(AMNESIA_COIN_ENTRY, 5, player);
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 2:
                DoLottery(AMNESIA_COIN_ENTRY, 10, player);
                SendMenu(player, creature);
                break;

            case GOSSIP_ACTION_INFO_DEF + 3:
                player->PlayerTalkClass->SendCloseGossip();
                break;
            }
        }
        return true;
    }

	struct custom_npc_lotteryAI : public ScriptedAI
	{
        custom_npc_lotteryAI(Creature* creature) : ScriptedAI(creature) { }
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new custom_npc_lotteryAI(creature);
	}
};

void AddSC_custom_npc_lottery()
{
	new custom_npc_lottery();
}
