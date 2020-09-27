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

class custom_vip_only_npc : public CreatureScript
{
public:
    custom_vip_only_npc() : CreatureScript("npc_vip") { }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }


    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        QueryResult result = CharacterDatabase.PQuery("SELECT AccountId FROM premium WHERE active = 1 AND AccountId = %u", player->GetSession()->GetAccountId());
        bool isVipPlayer = false;
        if (result) {
            isVipPlayer = true;
        }

        if (isVipPlayer)
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
        player->PlayerTalkClass->ClearMenus();

        if (sender == GOSSIP_SENDER_MAIN)
        {
            bool isSpanish = IsSpanishPlayer(player);

            switch (action)
            {
            case GOSSIP_OPTION_VENDOR:
                player->GetSession()->SendListInventory(creature->GetGUID());
                player->PlayerTalkClass->SendCloseGossip();
                break;
            case 1:
                player->GetSession()->SendNotification(isSpanish ? "Que haces aqui? No eres VIP! - Has sido reportado a un GM!" : "Why are you here?  You are not a VIP! - You have been reported to a GM!");
                player->TeleportTo(1, -11823.9f, -4779.58f, 5.9206f, 1.1357f);
                sLog->outError("Player::VipVendorWithoutPrivileges: Possible hacking attempt: Account %u tried to buy in VIP vendor. He has been teleported to Shop.", player->GetSession()->GetAccountId());
                player->PlayerTalkClass->SendCloseGossip();
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

void AddSC_custom_vip_only_npc()
{
	new custom_vip_only_npc();
}
