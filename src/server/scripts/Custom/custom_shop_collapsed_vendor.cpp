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

enum Spells
{

    CREATURE_VENDOR_GEMS_RED        = 601592,          
    CREATURE_VENDOR_GEMS_YELLOW     = 601571,          
    CREATURE_VENDOR_GEMS_PURPLE     = 601573,          
    CREATURE_VENDOR_GEMS_ORANGE     = 601567,          
    CREATURE_VENDOR_GEMS_META       = 601559,          
    CREATURE_VENDOR_GEMS_GREEN      = 601541,          
    CREATURE_VENDOR_GEMS_BLUE       = 601514,
    
    CREATURE_VENDOR_PROFESSION_ENCHANTING       = 601531,          
    CREATURE_VENDOR_PROFESSION_JEWELCRAFTING    = 601540,          
    CREATURE_VENDOR_PROFESSION_LEATHERWORKING   = 601554,          
    CREATURE_VENDOR_PROFESSION_INSCRIPTION      = 601548,          
    CREATURE_VENDOR_PROFESSION_HERBALISM        = 601543,          
    CREATURE_VENDOR_PROFESSION_TAILORING        = 601518,  
    CREATURE_VENDOR_PROFESSION_BLACKSMITHING    = 601560,       
    CREATURE_VENDOR_PROFESSION_ENGINEERING      = 601522,    
    CREATURE_VENDOR_PROFESSION_ELEMENTALS       = 601552,   
    CREATURE_VENDOR_PROFESSION_ALCHEMY          = 601561,
    
    CREATURE_VENDOR_PVP_S7              = 601511,        
    CREATURE_VENDOR_PVP_S7OFFPARTS      = 601615,  
    CREATURE_VENDOR_PVP_S7WEAPONS       = 601512,          
    CREATURE_VENDOR_PVP_S8              = 104002,          
    CREATURE_VENDOR_PVP_S8OFFPARTS      = 104000,             
    CREATURE_VENDOR_PVP_3V3WEAPONS      = 104003,
    
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_WEAPONS                          = 604000,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_TRINKETS                         = 604001,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_OFFSET_BELTS_BOOTS_BRACERS       = 604002,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_OFFSET_RINGS_NECKS               = 604003,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_OFFSET_CLOAKS                    = 604004,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_OFFTIER                          = 604005,

    CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_WEAPONS                      = 604006,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_TRINKETS                     = 604007,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_OFFSET_BELTS_BOOTS_BRACERS   = 604008,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_OFFSET_RINGS_NECKS           = 604009,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_OFFSET_CLOAKS                = 604010,
    CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_OFFTIER                      = 604011,

    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_TRINKETS                = 240000,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_RINGS                   = 240001,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_NECKS                   = 240002,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_CLOAKS                  = 240003,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_BRACERS                 = 240004,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_WAISTS                  = 240005,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_BOOTS                   = 240006,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_1HWEAPONS               = 240007,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_2HWEAPONS               = 240008,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_RWEAPONS                = 240009,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_OFFHANDS                = 240010,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_SHIELDS                 = 240011,
    CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_OFFTIER                 = 240013,

    CREATURE_VENDOR_PVE_T9_HORDE        = 601632,
    CREATURE_VENDOR_PVE_T9_ALLIANCE     = 601585,
    CREATURE_VENDOR_PVE_OFFSET          = 100008,          
    CREATURE_VENDOR_PVE_RINGS           = 601594,          
    CREATURE_VENDOR_PVE_NECKS           = 601501,          
    CREATURE_VENDOR_PVE_CLOAKS          = 601517,          
    CREATURE_VENDOR_PVE_BRACERS         = 601644,          
    CREATURE_VENDOR_PVE_BELTS           = 601642,          
    CREATURE_VENDOR_PVE_BOOTS           = 601640,          
    CREATURE_VENDOR_PVE_1HWEAPONS       = 601566,          
    CREATURE_VENDOR_PVE_2HWEAPONS       = 601608,          
    CREATURE_VENDOR_PVE_RANGEDWEAPONS   = 601542,         
    CREATURE_VENDOR_PVE_OFFHANDS        = 601563,          
    CREATURE_VENDOR_PVE_SHIELDS         = 601598,          
    CREATURE_VENDOR_PVE_RELICS          = 601593,          
    CREATURE_VENDOR_PVE_TRINKETS        = 601605,
    
    CREATURE_VENDOR_TRANSMOG_PVP_S1     = 601502,          
    CREATURE_VENDOR_TRANSMOG_PVP_S2     = 601503,     
    CREATURE_VENDOR_TRANSMOG_PVP_S3     = 601504,     
    CREATURE_VENDOR_TRANSMOG_PVP_S4     = 601505,          
    CREATURE_VENDOR_TRANSMOG_PVP_S5     = 601506,          
    CREATURE_VENDOR_TRANSMOG_PVP_S52    = 601507,      
    CREATURE_VENDOR_TRANSMOG_PVP_S53    = 601508,      
    CREATURE_VENDOR_TRANSMOG_PVP_S6     = 601509,
    CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S1    = 601616,         
    CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S2    = 601617,         
    CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S3    = 601618,          
    CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S4    = 601619,          
    CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S5    = 601620,          
    CREATURE_VENDOR_TRANSMOG_PVP_WEAP_S6    = 601621,
    
    CREATURE_VENDOR_TRANSMOG_PVE_T1     = 601575,          
    CREATURE_VENDOR_TRANSMOG_PVE_T2     = 601576,          
    CREATURE_VENDOR_TRANSMOG_PVE_T3     = 601577,          
    CREATURE_VENDOR_TRANSMOG_PVE_T4     = 601578,          
    CREATURE_VENDOR_TRANSMOG_PVE_T5     = 601579,          
    CREATURE_VENDOR_TRANSMOG_PVE_T6     = 601580,          
    CREATURE_VENDOR_TRANSMOG_PVE_T7     = 601581,          
    CREATURE_VENDOR_TRANSMOG_PVE_T75    = 601582,          
    CREATURE_VENDOR_TRANSMOG_PVE_T8     = 601583,          
    CREATURE_VENDOR_TRANSMOG_PVE_T85    = 601584,          
    
    CREATURE_VENDOR_ICC_T10_251         = 601588,
    CREATURE_VENDOR_ICC_T10_264         = 601589,      
    CREATURE_VENDOR_ICC_T10_277         = 601590,
    
    CREATURE_VENDOR_MOUNTS_PVE          = 101000,
    CREATURE_VENDOR_MOUNTS_PVP          = 101001,
    CREATURE_VENDOR_MOUNTS_EPIC         = 601533,
    
    CREATURE_VENDOR_PETS_PVE            = 601000,
    CREATURE_VENDOR_PETS_PVP            = 601001
};

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

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
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

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
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
        ShowOption(player, isSpanish ? "-- (264) Equipo PVE Rating de Arenas --"            : "-- (264) PVE Gear Arena Rating --", 100);
        ShowOption(player, isSpanish ? "[1750: (264) Cinturones - Botas - Brazales]"        : "[1750: (264) Belts - Boots - Bracers]", 7);
        ShowOption(player, isSpanish ? "[1750: (264) Anillos - Collares]"                   : "[1750: (264) Rings - Necks]", 8);
        ShowOption(player, isSpanish ? "[1750: (264) Capas]"                                : "[1750: (264) Cloaks]", 9);
        ShowOption(player, isSpanish ? "[1750: (264) Off-Tier]"                             : "[1750: (264) Off-Tier]", 10);
        ShowOption(player, isSpanish ? "[1850: (264) Armas]"                                : "[1850: (264) Weapons]", 11);
        ShowOption(player, isSpanish ? "[1950: (264) Abalorios]"                            : "[1950: (264) Trinkets]", 12);
        ShowOption(player, isSpanish ? "-- (271/277) Equipo PVE Rating de Arenas --"        : "-- (271/277) PVE Gear Arena Rating --", 100);
        ShowOption(player, isSpanish ? "[2050: (271/277) Cinturones - Botas - Brazales]"    : "[2050: (271/277) Belts - Boots - Bracers]", 13);
        ShowOption(player, isSpanish ? "[2050: (271/277) Anillos - Collares]"               : "[2050: (271/277) Rings - Necks]", 14);
        ShowOption(player, isSpanish ? "[2050: (271/277) Capas]"                            : "[2050: (271/277) Cloaks]", 15);
        ShowOption(player, isSpanish ? "[2050: (271/277) Off-Tier]"                         : "[2050: (271/277) Off-Tier]", 16);
        ShowOption(player, isSpanish ? "[2200: (271/277) Armas]"                            : "[2200: (271/277) Weapons]", 17);
        ShowOption(player, isSpanish ? "[2350: (271/277) Abalorios]"                        : "[2350: (271/277) Trinkets]", 18);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
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

        case GOSSIP_ACTION_INFO_DEF + 7: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_OFFSET_BELTS_BOOTS_BRACERS); break;
        case GOSSIP_ACTION_INFO_DEF + 8: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_OFFSET_RINGS_NECKS); break;
        case GOSSIP_ACTION_INFO_DEF + 9: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_OFFSET_CLOAKS); break;
        case GOSSIP_ACTION_INFO_DEF + 10: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_OFFTIER); break;
        case GOSSIP_ACTION_INFO_DEF + 11: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_WEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 12: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_264_TRINKETS); break;

        case GOSSIP_ACTION_INFO_DEF + 13: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_OFFSET_BELTS_BOOTS_BRACERS); break;
        case GOSSIP_ACTION_INFO_DEF + 14: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_OFFSET_RINGS_NECKS); break;
        case GOSSIP_ACTION_INFO_DEF + 15: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_OFFSET_CLOAKS); break;
        case GOSSIP_ACTION_INFO_DEF + 16: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_OFFTIER); break;
        case GOSSIP_ACTION_INFO_DEF + 17: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_WEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 18: ShowVendor(player, creature, CREATURE_VENDOR_PVP_PVEGEARBYARENA_271_277_TRINKETS); break;
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
        ShowOption(player, isSpanish ? "[Off-Tier]" : "[Off-Tier]", 2);
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
        ShowOption(player, isSpanish ? "[251-284] Equipo PVE por AmnesiaCoins" : "[251-284] PVE Gear by AmnesiaCoins", 100);

        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
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

        case GOSSIP_ACTION_INFO_DEF + 16: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_OFFTIER); break;
        case GOSSIP_ACTION_INFO_DEF + 17: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_RINGS); break;
        case GOSSIP_ACTION_INFO_DEF + 18: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_NECKS); break;
        case GOSSIP_ACTION_INFO_DEF + 19: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_CLOAKS); break;
        case GOSSIP_ACTION_INFO_DEF + 20: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_BRACERS); break;
        case GOSSIP_ACTION_INFO_DEF + 21: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_WAISTS); break;
        case GOSSIP_ACTION_INFO_DEF + 22: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_BOOTS); break;
        case GOSSIP_ACTION_INFO_DEF + 23: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_1HWEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 24: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_2HWEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 25: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_RWEAPONS); break;
        case GOSSIP_ACTION_INFO_DEF + 26: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_OFFHANDS); break;
        case GOSSIP_ACTION_INFO_DEF + 27: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_SHIELDS); break;
        case GOSSIP_ACTION_INFO_DEF + 30: ShowVendor(player, creature, CREATURE_VENDOR_PVE_PVEGEARBYAMNESIA_251_TO_284_TRINKETS); break;

        case GOSSIP_ACTION_INFO_DEF + 100:
        {
            bool isSpanish = IsSpanishPlayer(player);
            ClearGossipMenuFor(player);
            ShowOption(player, isSpanish ? "[Off-Tier]" : "[Off-Tier]", 16);
            ShowOption(player, isSpanish ? "[Anillos]" : "[Rings]", 17);
            ShowOption(player, isSpanish ? "[Collares]" : "[Necks]", 18);
            ShowOption(player, isSpanish ? "[Capas]" : "[Cloaks]", 19);
            ShowOption(player, isSpanish ? "[Brazales]" : "[Bracers]", 20);
            ShowOption(player, isSpanish ? "[Cinturones]" : "[Belts]", 21);
            ShowOption(player, isSpanish ? "[Botas]" : "[Boots]", 22);
            ShowOption(player, isSpanish ? "[Armas 1 Mano]" : "[1 Hand Weapons]", 23);
            ShowOption(player, isSpanish ? "[Armas 2 Manos]" : "[2 Hand Weapons]", 24);
            ShowOption(player, isSpanish ? "[Armas a distancia]" : "[Ranged Weapons]", 25);
            ShowOption(player, isSpanish ? "[Mano izquierda]" : "[Offhands]", 26);
            ShowOption(player, isSpanish ? "[Escudos]" : "[Shields]", 27);
            ShowOption(player, isSpanish ? "[Abalorios]" : "[Trinkets]", 28);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 101);
            SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
            break;
        }
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

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
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

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
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

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
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

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
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

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
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
