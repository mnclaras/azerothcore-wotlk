#include "Language.h"
#include "Chat.h"
#include "SpellMgr.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Log.h"
#include "AccountMgr.h"
#include "DBCStores.h"
#include "World.h"
#include "Object.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Common.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

#define CREATURE_ENTRY_TRAINER_GLOBAL       101004

#define CREATURE_VENDOR_WARRIOR             105001
#define CREATURE_VENDOR_PALADIN             105002
#define CREATURE_VENDOR_HUNTER              105003
#define CREATURE_VENDOR_ROGUE               105004
#define CREATURE_VENDOR_PRIEST              105005
#define CREATURE_VENDOR_DEATHKNIGHT         105006
#define CREATURE_VENDOR_SHAMAN              105007
#define CREATURE_VENDOR_MAGE                105008
#define CREATURE_VENDOR_WARLOCK             105009
#define CREATURE_VENDOR_DRUID               105011

#define DEFAULT_MESSAGE 907

class custom_npc_global_trainer : public CreatureScript
{
public:
    custom_npc_global_trainer() : CreatureScript("npc_global_trainer") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        //AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Entrename." : "Train me.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        bool isSpanish = IsSpanishPlayer(player);

        if (player->getClass() == CLASS_WARRIOR)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

        else if (player->getClass() == CLASS_PALADIN)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

        else if (player->getClass() == CLASS_HUNTER)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

        else if (player->getClass() == CLASS_ROGUE)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);

        else if (player->getClass() == CLASS_PRIEST)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

        else if (player->getClass() == CLASS_DEATH_KNIGHT)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);

        else if (player->getClass() == CLASS_SHAMAN)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);

        else if (player->getClass() == CLASS_MAGE)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);

        else if (player->getClass() == CLASS_WARLOCK)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);

        else if (player->getClass() == CLASS_DRUID)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);


        if (player->GetSpecsCount() == 1 && player->getLevel() >= PlayerDualSpecLevel())
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Aprender Doble Especializacion." : "Learn Double Specialization.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14,
                isSpanish ? "Quieres aprender doble especializacion de talentos?" : "Do you want to learn double specialization?", 0, false);
        }

        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Olvidar mis talentos." : "Unlearn my talents.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15,
            isSpanish ? "Quieres olvidar todos tus talentos? Esto retirara cualquier mascota controlada." : "Do you want to unlearn all of your talents? This will unsummon any controlled pet.", 0, false);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    uint32 PlayerDualSpecLevel() const
    {
        return sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL);
    }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);

        bool isSpanish = IsSpanishPlayer(player);

        switch (action)
        {
        //case GOSSIP_ACTION_INFO_DEF + 1:
        //    player->GetSession()->SendTrainerList(creature->GetGUID());
        //    break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_WARRIOR);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_PALADIN);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_HUNTER);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 7:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_ROGUE);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 8:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_PRIEST);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 9:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_DEATHKNIGHT);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 10:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_SHAMAN);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 11:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_MAGE);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 12:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_WARLOCK);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 13:
            player->GetSession()->SendListInventory(creature->GetGUID(), CREATURE_VENDOR_DRUID);
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 14:
            if (player->IsInCombat())
            {
                player->CLOSE_GOSSIP_MENU();
                player->GetSession()->SendNotification(isSpanish ? "Estas en combate!" : "You are in combat!");
                return false;
            }

            player->learnSpell(63644);
            player->CastSpell(player, 31726);
            player->CastSpell(player, 63624);
            player->learnSpell(63645);
            player->UpdateSpecCount(2);
            player->GetSession()->SendNotification(isSpanish ? "|cffFFFF00GLOBAL TRAINER \n |cffFFFFFFHas aprendido la doble especialización de talentos!" : "|cffFFFF00GLOBAL TRAINER \n |cffFFFFFFDual Talents Learned Succesfully!");
            player->PlayerTalkClass->SendCloseGossip();
            break;
        case GOSSIP_ACTION_INFO_DEF + 15:
            player->resetTalents(true);
            player->SendTalentsInfoData(false);
            player->GetSession()->SendNotification(isSpanish ? "|cffFFFF00GLOBAL TRAINER \n |cffFFFFFFTalentos reiniciados satisfactoriamente!" : "|cffFFFF00GLOBAL TRAINER \n |cffFFFFFFTalents reseted succesfully!");
            player->CastSpell(player, 31726);
            player->PlayerTalkClass->SendCloseGossip();
            break;

        default:
            //AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Entrename." : "Train me.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            if (player->getClass() == CLASS_WARRIOR)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);

            else if (player->getClass() == CLASS_PALADIN)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

            else if (player->getClass() == CLASS_HUNTER)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);

            else if (player->getClass() == CLASS_ROGUE)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);

            else if (player->getClass() == CLASS_PRIEST)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

            else if (player->getClass() == CLASS_DEATH_KNIGHT)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);

            else if (player->getClass() == CLASS_SHAMAN)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);

            else if (player->getClass() == CLASS_MAGE)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);

            else if (player->getClass() == CLASS_WARLOCK)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);

            else if (player->getClass() == CLASS_DRUID)
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, isSpanish ? "Comprar Glifos." : "Buy Glyphs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);


            if (player->GetSpecsCount() == 1 && player->getLevel() >= PlayerDualSpecLevel())
            {
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Aprender Doble Especializacion." : "Learn Double Specialization.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14,
                    isSpanish ? "Quieres aprender doble especializacion de talentos?" : "Do you want to learn double specialization?", 0, false);
            }

            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, isSpanish ? "Olvidar mis talentos." : "Unlearn my talents.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15,
                isSpanish ? "Quieres olvidar todos tus talentos? Esto retirara cualquier mascota controlada." : "Do you want to unlearn all of your talents? This will unsummon any controlled pet.", 0, false);

            SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
            break;
        }

        return true;
    };

    struct custom_npc_global_trainerAI : public ScriptedAI
    {
        custom_npc_global_trainerAI(Creature* creature) : ScriptedAI(creature) { }
    };

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new custom_npc_global_trainerAI(creature);
	}
};

void AddSC_custom_npc_global_trainer()
{
	new custom_npc_global_trainer();
}
