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

#define CREATURE_VENDOR_GEMS_RED        601592          
#define CREATURE_VENDOR_GEMS_YELLOW     601571          
#define CREATURE_VENDOR_GEMS_PURPLE     601573          
#define CREATURE_VENDOR_GEMS_ORANGE     601567          
#define CREATURE_VENDOR_GEMS_META       601559          
#define CREATURE_VENDOR_GEMS_GREEN      601541          
#define CREATURE_VENDOR_GEMS_BLUE       601514

#define CREATURE_VENDOR_PROFESSION_ENCHANTING       601531          
#define CREATURE_VENDOR_PROFESSION_JEWELCRAFTING    601540          
#define CREATURE_VENDOR_PROFESSION_LEATHERWORKING   601554          
#define CREATURE_VENDOR_PROFESSION_INSCRIPTION      601548          
#define CREATURE_VENDOR_PROFESSION_HERBALISM        601543          
#define CREATURE_VENDOR_PROFESSION_TAILORING        601518  
#define CREATURE_VENDOR_PROFESSION_BLACKSMITHING    601560       
#define CREATURE_VENDOR_PROFESSION_ENGINEERING      601522    
#define CREATURE_VENDOR_PROFESSION_ELEMENTALS       601552   
#define CREATURE_VENDOR_PROFESSION_ALCHEMY          601561

#define CREATURE_VENDOR_PVP_S7              601511        
#define CREATURE_VENDOR_PVP_S7OFFPARTS      601615  
#define CREATURE_VENDOR_PVP_S7WEAPONS       601512          
#define CREATURE_VENDOR_PVP_S8              104002          
#define CREATURE_VENDOR_PVP_S8OFFPARTS      104000             
#define CREATURE_VENDOR_PVP_3V3WEAPONS      104003

#define CREATURE_VENDOR_PVP_PVEGEARBYARENA_OFFSET      604000
#define CREATURE_VENDOR_PVP_PVEGEARBYARENA_WEAPONS     604001
#define CREATURE_VENDOR_PVP_PVEGEARBYARENA_TRINKETS    604002          

#define CREATURE_VENDOR_PVE_T9_HORDE        601632
#define CREATURE_VENDOR_PVE_T9_ALLIANCE     601585
#define CREATURE_VENDOR_PVE_OFFSET          100008          
#define CREATURE_VENDOR_PVE_RINGS           601594          
#define CREATURE_VENDOR_PVE_NECKS           601501          
#define CREATURE_VENDOR_PVE_CLOAKS          601517          
#define CREATURE_VENDOR_PVE_BRACERS         601644          
#define CREATURE_VENDOR_PVE_BELTS           601642          
#define CREATURE_VENDOR_PVE_BOOTS           601640          
#define CREATURE_VENDOR_PVE_1HWEAPONS       601566          
#define CREATURE_VENDOR_PVE_2HWEAPONS       601608          
#define CREATURE_VENDOR_PVE_RANGEDWEAPONS   601542         
#define CREATURE_VENDOR_PVE_OFFHANDS        601563          
#define CREATURE_VENDOR_PVE_SHIELDS         601598          
#define CREATURE_VENDOR_PVE_RELICS          601593          
#define CREATURE_VENDOR_PVE_TRINKETS        601605

#define CREATURE_VENDOR_TRANSMOG_PVP_S1     601502          
#define CREATURE_VENDOR_TRANSMOG_PVP_S2     601503     
#define CREATURE_VENDOR_TRANSMOG_PVP_S3     601504     
#define CREATURE_VENDOR_TRANSMOG_PVP_S4     601505          
#define CREATURE_VENDOR_TRANSMOG_PVP_S5     601506          
#define CREATURE_VENDOR_TRANSMOG_PVP_S52    601507      
#define CREATURE_VENDOR_TRANSMOG_PVP_S53    601508      
#define CREATURE_VENDOR_TRANSMOG_PVP_S6     601509
#define CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S1    601616         
#define CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S2    601617         
#define CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S3    601618          
#define CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S4    601619          
#define CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S5    601620          
#define CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S6    601621

#define CREATURE_VENDOR_TRANSMOG_PVE_T1     601575          
#define CREATURE_VENDOR_TRANSMOG_PVE_T2     601576          
#define CREATURE_VENDOR_TRANSMOG_PVE_T3     601577          
#define CREATURE_VENDOR_TRANSMOG_PVE_T4     601578          
#define CREATURE_VENDOR_TRANSMOG_PVE_T5     601579          
#define CREATURE_VENDOR_TRANSMOG_PVE_T6     601580          
#define CREATURE_VENDOR_TRANSMOG_PVE_T7     601581          
#define CREATURE_VENDOR_TRANSMOG_PVE_T75    601582          
#define CREATURE_VENDOR_TRANSMOG_PVE_T8     601583          
#define CREATURE_VENDOR_TRANSMOG_PVE_T85    601584          

#define CREATURE_VENDOR_ICC_T10_251         601588
#define CREATURE_VENDOR_ICC_T10_264         601589      
#define CREATURE_VENDOR_ICC_T10_277         601590

#define CREATURE_VENDOR_MOUNTS_PVE          101000
#define CREATURE_VENDOR_MOUNTS_PVP          101001
#define CREATURE_VENDOR_MOUNTS_EPIC         601533

#define CREATURE_VENDOR_PETS_PVE            601000
#define CREATURE_VENDOR_PETS_PVP            601001

//#define CREATURE_VENDOR_               
//#define CREATURE_VENDOR_               
//#define CREATURE_VENDOR_               

#define DEFAULT_MESSAGE 907

bool IsSpanishPlayer(Player* player)
{
    LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
    return (locale == LOCALE_esES || locale == LOCALE_esMX);
}
void ShowVendor(Player* player, Creature* creature, uint32 entry)
{
    player->GetSession()->SendListInventory(creature->GetGUID(), entry);
    player->PlayerTalkClass->SendCloseGossip();
}
void ShowOption(Player* player, std::string const& text, uint32 order)
{
    AddGossipItemFor(player, GOSSIP_ICON_VENDOR, text, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + order);
}

class custom_shop_collapsed_vendor_gems : public CreatureScript
{
public:
    custom_shop_collapsed_vendor_gems() : CreatureScript("npc_collapsed_vendor_gems") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ShowOption(player, isSpanish ? "[Rojas]" : "[Red]", 1);
        ShowOption(player, isSpanish ? "[Amarillas]" : "[Yellow]", 2);
        ShowOption(player, isSpanish ? "[Azules]" : "[Blue]", 3);
        ShowOption(player, isSpanish ? "[Naranjas]" : "[Orange]", 4);
        ShowOption(player, isSpanish ? "[Verdes]" : "[Green]", 5);
        ShowOption(player, isSpanish ? "[Moradas]" : "[Purple]", 6);
        ShowOption(player, isSpanish ? "[Meta]" : "[Meta]", 7);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1: ShowVendor(player, creature, CREATURE_VENDOR_GEMS_RED); break;
        case GOSSIP_ACTION_INFO_DEF + 2: ShowVendor(player, creature, CREATURE_VENDOR_GEMS_YELLOW); break;
        case GOSSIP_ACTION_INFO_DEF + 3: ShowVendor(player, creature, CREATURE_VENDOR_GEMS_BLUE); break;
        case GOSSIP_ACTION_INFO_DEF + 4: ShowVendor(player, creature, CREATURE_VENDOR_GEMS_ORANGE); break;
        case GOSSIP_ACTION_INFO_DEF + 5: ShowVendor(player, creature, CREATURE_VENDOR_GEMS_GREEN); break;
        case GOSSIP_ACTION_INFO_DEF + 6: ShowVendor(player, creature, CREATURE_VENDOR_GEMS_PURPLE); break;
        case GOSSIP_ACTION_INFO_DEF + 7: ShowVendor(player, creature, CREATURE_VENDOR_GEMS_META); break;
        default: OnGossipHello(player, creature); break;
        }

        return true;
    };

    struct custom_shop_collapsed_vendor_gemsAI : public ScriptedAI
    {
        custom_shop_collapsed_vendor_gemsAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_shop_collapsed_vendor_gemsAI(creature);
    }
};

class custom_shop_collapsed_vendor_professions : public CreatureScript
{
public:
    custom_shop_collapsed_vendor_professions() : CreatureScript("npc_collapsed_vendor_professions") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ShowOption(player, isSpanish ? "[Encantamiento]" : "[Enchanting]", 1);
        ShowOption(player, isSpanish ? "[Joyeria]" : "[Jewelcrafting]", 2);
        ShowOption(player, isSpanish ? "[Peleteria]" : "[Leatherworking]", 3);
        ShowOption(player, isSpanish ? "[Inscripcion]" : "[Inscription]", 4);
        ShowOption(player, isSpanish ? "[Herboristeria]" : "[Herbalism]", 5);
        ShowOption(player, isSpanish ? "[Sastreria]" : "[Tailoring]", 6);
        ShowOption(player, isSpanish ? "[Herreria]" : "[Blacksmithing]", 7);
        ShowOption(player, isSpanish ? "[Ingenieria]" : "[Engineering]", 8);
        ShowOption(player, isSpanish ? "[Elementales]" : "[Elementals]", 9);
        ShowOption(player, isSpanish ? "[Alquimia]" : "[Alchemy]", 10);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_ENCHANTING); break;
        case GOSSIP_ACTION_INFO_DEF + 2: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_JEWELCRAFTING); break;
        case GOSSIP_ACTION_INFO_DEF + 3: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_LEATHERWORKING); break;
        case GOSSIP_ACTION_INFO_DEF + 4: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_INSCRIPTION); break;
        case GOSSIP_ACTION_INFO_DEF + 5: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_HERBALISM); break;
        case GOSSIP_ACTION_INFO_DEF + 6: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_TAILORING); break;
        case GOSSIP_ACTION_INFO_DEF + 7: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_BLACKSMITHING); break;
        case GOSSIP_ACTION_INFO_DEF + 8: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_ENGINEERING); break;
        case GOSSIP_ACTION_INFO_DEF + 9: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_ELEMENTALS); break;
        case GOSSIP_ACTION_INFO_DEF + 10: ShowVendor(player, creature, CREATURE_VENDOR_PROFESSION_ALCHEMY); break;
        default: OnGossipHello(player, creature); break;
        }

        return true;
    };

    struct custom_shop_collapsed_vendor_professionsAI : public ScriptedAI
    {
        custom_shop_collapsed_vendor_professionsAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_shop_collapsed_vendor_professionsAI(creature);
    }
};

class custom_shop_collapsed_vendor_pvp : public CreatureScript
{
public:
    custom_shop_collapsed_vendor_pvp() : CreatureScript("npc_collapsed_vendor_pvp") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ShowOption(player, isSpanish ? "[S7]" : "[S7]", 1);
        ShowOption(player, isSpanish ? "[S7 Offset]" : "[S7 Offset]", 2);
        ShowOption(player, isSpanish ? "[S7 Armas]" : "[S7 Weapons]", 3);
        ShowOption(player, isSpanish ? "[S8]" : "[S8]", 4);
        ShowOption(player, isSpanish ? "[S8 Offset]" : "[S8 Offset]", 5);
        ShowOption(player, isSpanish ? "[Armas 3v3]" : "[3v3 Weapons]", 6);
        ShowOption(player, isSpanish ? "---- Equipo PVE por Rating de Arenas ----" : "---- PVE Gear by Arena Rating ----", 100);
        ShowOption(player, isSpanish ? "[1850 - Offset]" : "[1850 - Offset]", 7);
        ShowOption(player, isSpanish ? "[1950 - Armas]" : "[1950 - Weapons]", 8);
        ShowOption(player, isSpanish ? "[2050 - Abalorios]" : "[2050 - Trinkets]", 9);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1: ShowVendor(player, creature, CREATURE_VENDOR_PVP_S7); break;
        case GOSSIP_ACTION_INFO_DEF + 2: ShowVendor(player, creature, CREATURE_VENDOR_PVP_S7OFFPARTS); break;
        case GOSSIP_ACTION_INFO_DEF + 3: ShowVendor(player, creature, CREATURE_VENDOR_PVP_S7WEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 4: ShowVendor(player, creature, CREATURE_VENDOR_PVP_S8); break;
        case GOSSIP_ACTION_INFO_DEF + 5: ShowVendor(player, creature, CREATURE_VENDOR_PVP_S8OFFPARTS); break;
        case GOSSIP_ACTION_INFO_DEF + 6: ShowVendor(player, creature, CREATURE_VENDOR_PVP_3V3WEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 7: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_OFFSET); break;
        case GOSSIP_ACTION_INFO_DEF + 8: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_WEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 9: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_TRINKETS); break;
        default: OnGossipHello(player, creature); break;
        }

        return true;
    };

    struct custom_shop_collapsed_vendor_pvpAI : public ScriptedAI
    {
        custom_shop_collapsed_vendor_pvpAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_shop_collapsed_vendor_pvpAI(creature);
    }
};

class custom_shop_collapsed_vendor_pve : public CreatureScript
{
public:
    custom_shop_collapsed_vendor_pve() : CreatureScript("npc_collapsed_vendor_pve") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ShowOption(player, isSpanish ? "[T9]" : "[T9]", 1);
        ShowOption(player, isSpanish ? "[Offset]" : "[Offset]", 2);
        ShowOption(player, isSpanish ? "[Anillos]" : "[Rings]", 3);
        ShowOption(player, isSpanish ? "[Collares]" : "[Necks]", 4);
        ShowOption(player, isSpanish ? "[Capas]" : "[Cloaks]", 5);
        ShowOption(player, isSpanish ? "[Brazales]" : "[Bracers]", 6);
        ShowOption(player, isSpanish ? "[Cinturones]" : "[Belts]", 7);
        ShowOption(player, isSpanish ? "[Botas]" : "[Boots]", 8);
        ShowOption(player, isSpanish ? "[Armas 1 Mano]" : "[1 Hand Weapons]", 9);
        ShowOption(player, isSpanish ? "[Armas 2 Manos]" : "[2 Hand Weapons]", 10);
        ShowOption(player, isSpanish ? "[Armas a distancia]" : "[Ranged Weapons]", 11);
        ShowOption(player, isSpanish ? "[Mano izquierda]" : "[Offhands]", 12);
        ShowOption(player, isSpanish ? "[Escudos]" : "[Shields]", 13);
        ShowOption(player, isSpanish ? "[Reliquias]" : "[Relics]", 14);
        ShowOption(player, isSpanish ? "[Abalorios]" : "[Trinkets]", 15);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (player->GetTeamId() == TEAM_ALLIANCE) ShowVendor(player, creature, CREATURE_VENDOR_PVE_T9_ALLIANCE);
            else if (player->GetTeamId() == TEAM_HORDE) ShowVendor(player, creature, CREATURE_VENDOR_PVE_T9_HORDE);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2: ShowVendor(player, creature, CREATURE_VENDOR_PVE_OFFSET); break;
        case GOSSIP_ACTION_INFO_DEF + 3: ShowVendor(player, creature, CREATURE_VENDOR_PVE_RINGS); break;
        case GOSSIP_ACTION_INFO_DEF + 4: ShowVendor(player, creature, CREATURE_VENDOR_PVE_NECKS); break;
        case GOSSIP_ACTION_INFO_DEF + 5: ShowVendor(player, creature, CREATURE_VENDOR_PVE_CLOAKS); break;
        case GOSSIP_ACTION_INFO_DEF + 6: ShowVendor(player, creature, CREATURE_VENDOR_PVE_BRACERS); break;
        case GOSSIP_ACTION_INFO_DEF + 7: ShowVendor(player, creature, CREATURE_VENDOR_PVE_BELTS); break;
        case GOSSIP_ACTION_INFO_DEF + 8: ShowVendor(player, creature, CREATURE_VENDOR_PVE_BOOTS); break;
        case GOSSIP_ACTION_INFO_DEF + 9: ShowVendor(player, creature, CREATURE_VENDOR_PVE_1HWEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 10: ShowVendor(player, creature, CREATURE_VENDOR_PVE_2HWEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 11: ShowVendor(player, creature, CREATURE_VENDOR_PVE_RANGEDWEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 12: ShowVendor(player, creature, CREATURE_VENDOR_PVE_OFFHANDS); break;
        case GOSSIP_ACTION_INFO_DEF + 13: ShowVendor(player, creature, CREATURE_VENDOR_PVE_SHIELDS); break;
        case GOSSIP_ACTION_INFO_DEF + 14: ShowVendor(player, creature, CREATURE_VENDOR_PVE_RELICS); break;
        case GOSSIP_ACTION_INFO_DEF + 15: ShowVendor(player, creature, CREATURE_VENDOR_PVE_TRINKETS); break;
        default: OnGossipHello(player, creature); break;
        }

        return true;
    };

    struct custom_shop_collapsed_vendor_pveAI : public ScriptedAI
    {
        custom_shop_collapsed_vendor_pveAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_shop_collapsed_vendor_pveAI(creature);
    }
};

class custom_shop_collapsed_vendor_transmog_pvp : public CreatureScript
{
public:
    custom_shop_collapsed_vendor_transmog_pvp() : CreatureScript("npc_collapsed_vendor_transmog_pvp") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ShowOption(player, isSpanish ? "S1 (Gladiador)." : "S1 (Gladiator).", 1);
        ShowOption(player, isSpanish ? "S2 (Gladiador Despiadado)." : "S2 (Merciless Gladiator).", 2);
        ShowOption(player, isSpanish ? "S3 (Gladiador Vengativo)." : "S3 (Vengeful Gladiator).", 3);
        ShowOption(player, isSpanish ? "S4 (Gladiador Brutal)." : "S4 (Brutal Gladiator).", 4);
        ShowOption(player, isSpanish ? "S5 (Gladiador Mortal)." : "S5 (Deadly Gladiator).", 5);
        ShowOption(player, isSpanish ? "S5 (Gladiador Odioso)." : "S5 (Hateful Gladiator)", 6);
        ShowOption(player, isSpanish ? "S5 (Gladiador Salvaje)." : "S5 (Savage Gladiator).", 7);
        ShowOption(player, isSpanish ? "S6 (Gladiador Furioso)." : "S6 (Furious Gladiator).", 8);
        ShowOption(player, isSpanish ? "Armas S1 (Gladiador)." : "S1 Weapons (Gladiator).", 9);
        ShowOption(player, isSpanish ? "Armas S2 (Gladiador Despiadado)." : "S2 Weapons (Merciless Gladiator).", 10);
        ShowOption(player, isSpanish ? "Armas S3 (Gladiador Vengativo)." : "S3 Weapons (Vengeful Gladiator).", 11);
        ShowOption(player, isSpanish ? "Armas S4 (Gladiador Brutal)." : "S4 Weapons (Brutal Gladiator).", 12);
        ShowOption(player, isSpanish ? "Armas S5 (Gladiador Mortal/Odioso/Salvaje)." : "S5 Weapons (Deadly/Hateful/Savage).", 13);
        ShowOption(player, isSpanish ? "Armas S6 (Gladiador Furioso)." : "S6 Weapons (Furious Gladiator).", 14);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_S1); break;
        case GOSSIP_ACTION_INFO_DEF + 2: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_S2); break;
        case GOSSIP_ACTION_INFO_DEF + 3: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_S3); break;
        case GOSSIP_ACTION_INFO_DEF + 4: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_S4); break;
        case GOSSIP_ACTION_INFO_DEF + 5: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_S5); break;
        case GOSSIP_ACTION_INFO_DEF + 6: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_S52); break;
        case GOSSIP_ACTION_INFO_DEF + 7: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_S53); break;
        case GOSSIP_ACTION_INFO_DEF + 8: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_S6); break;
        case GOSSIP_ACTION_INFO_DEF + 9: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S1); break;
        case GOSSIP_ACTION_INFO_DEF + 10: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S2); break;
        case GOSSIP_ACTION_INFO_DEF + 11: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S3); break;
        case GOSSIP_ACTION_INFO_DEF + 12: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S4); break;
        case GOSSIP_ACTION_INFO_DEF + 13: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S5); break;
        case GOSSIP_ACTION_INFO_DEF + 14: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S6); break;
        default: OnGossipHello(player, creature); break;
        }

        return true;
    };

    struct custom_shop_collapsed_vendor_transmog_pvpAI : public ScriptedAI
    {
        custom_shop_collapsed_vendor_transmog_pvpAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_shop_collapsed_vendor_transmog_pvpAI(creature);
    }
};

class custom_shop_collapsed_vendor_transmog_pve : public CreatureScript
{
public:
    custom_shop_collapsed_vendor_transmog_pve() : CreatureScript("npc_collapsed_vendor_transmog_pve") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ShowOption(player, isSpanish ? "[T1]" : "[T1]", 1);
        ShowOption(player, isSpanish ? "[T2]" : "[T2]", 2);
        ShowOption(player, isSpanish ? "[T3]" : "[T3]", 3);
        ShowOption(player, isSpanish ? "[T4]" : "[T4]", 4);
        ShowOption(player, isSpanish ? "[T5]" : "[T5]", 5);
        ShowOption(player, isSpanish ? "[T6]" : "[T6]", 6);
        ShowOption(player, isSpanish ? "[T7]" : "[T7]", 7);
        ShowOption(player, isSpanish ? "[T7.5]" : "[T7.5]", 8);
        ShowOption(player, isSpanish ? "[T8]" : "[T8]", 9);
        ShowOption(player, isSpanish ? "[T8.5]" : "[T8.5]", 10);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T1); break;
        case GOSSIP_ACTION_INFO_DEF + 2: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T2); break;
        case GOSSIP_ACTION_INFO_DEF + 3: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T3); break;
        case GOSSIP_ACTION_INFO_DEF + 4: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T4); break;
        case GOSSIP_ACTION_INFO_DEF + 5: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T5); break;
        case GOSSIP_ACTION_INFO_DEF + 6: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T6); break;
        case GOSSIP_ACTION_INFO_DEF + 7: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T7); break;
        case GOSSIP_ACTION_INFO_DEF + 8: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T75); break;
        case GOSSIP_ACTION_INFO_DEF + 9: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T8); break;
        case GOSSIP_ACTION_INFO_DEF + 10: ShowVendor(player, creature, CREATURE_VENDOR_TRANSMOG_PVE_T85); break;
        default: OnGossipHello(player, creature); break;
        }

        return true;
    };

    struct custom_shop_collapsed_vendor_transmog_pveAI : public ScriptedAI
    {
        custom_shop_collapsed_vendor_transmog_pveAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_shop_collapsed_vendor_transmog_pveAI(creature);
    }
};

class custom_shop_collapsed_vendor_tier10 : public CreatureScript
{
public:
    custom_shop_collapsed_vendor_tier10() : CreatureScript("npc_collapsed_vendor_tier10") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ShowOption(player, isSpanish ? "[T10 251]" : "[T10 251]", 1);
        ShowOption(player, isSpanish ? "[T10.5 264]" : "[T10.5 264]", 2);
        ShowOption(player, isSpanish ? "[T10.5H 277]" : "[T10.5H 277]", 3);
      
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1: ShowVendor(player, creature, CREATURE_VENDOR_ICC_T10_251); break;
        case GOSSIP_ACTION_INFO_DEF + 2: ShowVendor(player, creature, CREATURE_VENDOR_ICC_T10_264); break;
        case GOSSIP_ACTION_INFO_DEF + 3: ShowVendor(player, creature, CREATURE_VENDOR_ICC_T10_277); break;
        default: OnGossipHello(player, creature); break;
        }

        return true;
    };

    struct custom_shop_collapsed_vendor_tier10AI : public ScriptedAI
    {
        custom_shop_collapsed_vendor_tier10AI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_shop_collapsed_vendor_tier10AI(creature);
    }
};

class custom_shop_collapsed_vendor_mounts : public CreatureScript
{
public:
    custom_shop_collapsed_vendor_mounts() : CreatureScript("npc_collapsed_vendor_mounts") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ShowOption(player, isSpanish ? "[Monturas PVE]" : "[PVE Mounts]", 1);
        ShowOption(player, isSpanish ? "[Monturas PVP]" : "[PVP Mounts]", 2);
        ShowOption(player, isSpanish ? "[Monturas Epicas]" : "[Epic Mounts]", 3);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1: ShowVendor(player, creature, CREATURE_VENDOR_MOUNTS_PVE); break;
        case GOSSIP_ACTION_INFO_DEF + 2: ShowVendor(player, creature, CREATURE_VENDOR_MOUNTS_PVP); break;
        case GOSSIP_ACTION_INFO_DEF + 3: ShowVendor(player, creature, CREATURE_VENDOR_MOUNTS_EPIC); break;
        default: OnGossipHello(player, creature); break;
        }

        return true;
    };

    struct custom_shop_collapsed_vendor_mountsAI : public ScriptedAI
    {
        custom_shop_collapsed_vendor_mountsAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_shop_collapsed_vendor_mountsAI(creature);
    }
};

class custom_shop_collapsed_vendor_pets : public CreatureScript
{
public:
    custom_shop_collapsed_vendor_pets() : CreatureScript("npc_collapsed_vendor_pets") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ShowOption(player, isSpanish ? "[Mascotas PVE]" : "[PVE Pets]", 1);
        ShowOption(player, isSpanish ? "[Mascotas PVP]" : "[PVP Pets]", 2);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1: ShowVendor(player, creature, CREATURE_VENDOR_PETS_PVE); break;
        case GOSSIP_ACTION_INFO_DEF + 2: ShowVendor(player, creature, CREATURE_VENDOR_PETS_PVP); break;
        default: OnGossipHello(player, creature); break;
        }

        return true;
    };

    struct custom_shop_collapsed_vendor_petsAI : public ScriptedAI
    {
        custom_shop_collapsed_vendor_petsAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_shop_collapsed_vendor_petsAI(creature);
    }
};

void AddSC_custom_shop_collapsed_vendor()
{
    new custom_shop_collapsed_vendor_gems();            // npc_collapsed_vendor_gems
    new custom_shop_collapsed_vendor_professions();     // npc_collapsed_vendor_professions
    new custom_shop_collapsed_vendor_pvp();             // npc_collapsed_vendor_pvp
    new custom_shop_collapsed_vendor_pve();             // npc_collapsed_vendor_pve
    new custom_shop_collapsed_vendor_transmog_pvp();    // npc_collapsed_vendor_transmog_pvp
    new custom_shop_collapsed_vendor_transmog_pve();    // npc_collapsed_vendor_transmog_pve
    new custom_shop_collapsed_vendor_tier10();          // npc_collapsed_vendor_tier10
    new custom_shop_collapsed_vendor_mounts();          // npc_collapsed_vendor_mounts
    new custom_shop_collapsed_vendor_pets();            // npc_collapsed_vendor_pets
}
