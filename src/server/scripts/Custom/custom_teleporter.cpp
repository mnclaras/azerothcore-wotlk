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
#include "Group.h"
#include "GroupMgr.h"
#include "Guild.h"

#define DEFAULT_MESSAGE 907

class custom_npc_teleporter : public CreatureScript
{
public:
    custom_npc_teleporter() : CreatureScript("npc_teleport") { }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    static void TeleportGuildHouse(Guild* guild, Player* player)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT `phase`, `map`, `posX`, `posY`, `posZ` FROM guild_house WHERE `guild` = '%u'", guild->GetId());

        if (!result)
        {
            return;
        }
        else
        {
            player->TeleportTo((*result)[1].GetUInt32(), (*result)[2].GetFloat(), (*result)[3].GetFloat(), (*result)[4].GetFloat(), player->GetOrientation());
        }
    }


    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        if (player->IsInCombat())
        {
            player->GetSession()->SendNotification(isSpanish ? "Error! Estas en combate." : "Failure! You are in combat.");
            CloseGossipMenuFor(player);
            return false;
        }

        AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop Tanaris" : "Shop Tanaris", GOSSIP_SENDER_MAIN, 5000);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop Utgarde" : "Shop Utgarde", GOSSIP_SENDER_MAIN, 5001);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop PvP" : "Shop PvP", GOSSIP_SENDER_MAIN, 5002);

        QueryResult result = CharacterDatabase.PQuery("SELECT AccountId FROM premium WHERE active = 1 AND AccountId = %u", player->GetSession()->GetAccountId());

        bool isVipPlayer = false;
        if (result) {
            isVipPlayer = true;
        }
        if (isVipPlayer)
        {
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop VIP" : "Shop VIP", GOSSIP_SENDER_MAIN, 14);
        }

        QueryResult has_gh = CharacterDatabase.PQuery("SELECT id, `guild` FROM `guild_house` WHERE guild = %u", player->GetGuildId());
        if (has_gh)
        {
            AddGossipItemFor(player, GOSSIP_ICON_TABARD, isSpanish ? "Casa de Hermandad" : "Guild House", GOSSIP_SENDER_MAIN, 5003);
        }

        AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Mazmorra custom (5HC)" : "Custom dungeon (5HC)", GOSSIP_SENDER_MAIN, 5004);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Circuitos escalada" : "Climbing circuits", GOSSIP_SENDER_MAIN, 15);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Ciudades principales" : "Main Cities", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Localizaciones Azeroth" : "Azeroth Locations", GOSSIP_SENDER_MAIN, 2);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mazmorras Clasicas" : "Classic Dungeons", GOSSIP_SENDER_MAIN, 3);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Bandas Clasicas" : "Classic Raids", GOSSIP_SENDER_MAIN, 4);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Localizaciones Terrallende" : "Outland Locations", GOSSIP_SENDER_MAIN, 5);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mazmorras TBC" : "TBC Dungeons", GOSSIP_SENDER_MAIN, 6);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Bandas TBC" : "TBC Raids", GOSSIP_SENDER_MAIN, 7);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Localizaciones Rasganorte" : "Northrend Locations", GOSSIP_SENDER_MAIN, 8);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mazmorras WOTLK" : "WOTLK Dungeons", GOSSIP_SENDER_MAIN, 9);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Bandas WOTLK" : "WOTLK Raids", GOSSIP_SENDER_MAIN, 10);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Gurubashi Arena" : "Gurubashi Arena", GOSSIP_SENDER_MAIN, 13);
        AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Isla PvP" : "PvP Island", GOSSIP_SENDER_MAIN, 16);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        bool isSpanish = IsSpanishPlayer(player);
        if (player->IsInCombat())
        {
            player->GetSession()->SendNotification(isSpanish ? "Error! Estas en combate." : "Failure! You are in combat.");
            CloseGossipMenuFor(player);
            return true;
        }

        bool isVipPlayer = false;

        //if (sender == GOSSIP_SENDER_MAIN)
        //{
            

            uint8 plyr = player->getRace();
            // float x = player->GetPositionX();
            // float y = player->GetPositionY();
            // float z = player->GetPositionZ();
            // float o = player->GetOrientation();

            QueryResult result;
            QueryResult has_gh;

            bool isOkGroup = true;
            Group* group = player->GetGroup();

            switch (action)
            {
            case 999:
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop Tanaris" : "Shop Tanaris", GOSSIP_SENDER_MAIN, 5000);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop Utgarde" : "Shop Utgarde", GOSSIP_SENDER_MAIN, 5001);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop PvP" : "Shop PvP", GOSSIP_SENDER_MAIN, 5002);

                result = CharacterDatabase.PQuery("SELECT AccountId FROM premium WHERE active = 1 AND AccountId = %u", player->GetSession()->GetAccountId());
                if (result) {
                    isVipPlayer = true;
                }
                if (isVipPlayer)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop VIP" : "Shop VIP", GOSSIP_SENDER_MAIN, 14);
                }

                has_gh = CharacterDatabase.PQuery("SELECT id, `guild` FROM `guild_house` WHERE guild = %u", player->GetGuildId());
                if (has_gh)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_TABARD, isSpanish ? "Casa de Hermandad" : "Guild House", GOSSIP_SENDER_MAIN, 5003);
                }

                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Mazmorra custom (5HC)" : "Custom dungeon (5HC)", GOSSIP_SENDER_MAIN, 5004);

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Circuitos escalada" : "Climbing circuits", GOSSIP_SENDER_MAIN, 15);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Ciudades principales" : "Main Cities", GOSSIP_SENDER_MAIN, 1);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Localizaciones Azeroth" : "Azeroth Locations", GOSSIP_SENDER_MAIN, 2);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mazmorras Clasicas" : "Classic Dungeons", GOSSIP_SENDER_MAIN, 3);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Bandas Clasicas" : "Classic Raids", GOSSIP_SENDER_MAIN, 4);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Localizaciones Terrallende" : "Outland Locations", GOSSIP_SENDER_MAIN, 5);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mazmorras TBC" : "TBC Dungeons", GOSSIP_SENDER_MAIN, 6);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Bandas TBC" : "TBC Raids", GOSSIP_SENDER_MAIN, 7);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Localizaciones Rasganorte" : "Northrend Locations", GOSSIP_SENDER_MAIN, 8);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mazmorras WOTLK" : "WOTLK Dungeons", GOSSIP_SENDER_MAIN, 9);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Bandas WOTLK" : "WOTLK Raids", GOSSIP_SENDER_MAIN, 10);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Gurubashi Arena" : "Gurubashi Arena", GOSSIP_SENDER_MAIN, 13);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Isla PvP" : "PvP Island", GOSSIP_SENDER_MAIN, 16);

                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 1: // Main cities
                if ((plyr == 1) || (plyr == 3) || (plyr == 4) || (plyr == 7) || (plyr == 11)) {
                    // Alliance cities
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Ventormenta" : "Stormwind", GOSSIP_SENDER_MAIN, 19);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Forjaz" : "Ironforge", GOSSIP_SENDER_MAIN, 20);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Darnassus" : "Darnassus", GOSSIP_SENDER_MAIN, 21);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Exodar" : "Exodar", GOSSIP_SENDER_MAIN, 22);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shattrath" : "Shattrath", GOSSIP_SENDER_MAIN, 11);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Dalaran" : "Dalaran", GOSSIP_SENDER_MAIN, 202);
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                    SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                }
                else if ((plyr == 2) || (plyr == 5) || (plyr == 6) || (plyr == 8) || (plyr == 10)) {
                    // Horde cities
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Orgimmar" : "Orgimmar", GOSSIP_SENDER_MAIN, 23);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Cima del Trueno" : "Thunderbluff", GOSSIP_SENDER_MAIN, 24);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Entranas" : "Undercity", GOSSIP_SENDER_MAIN, 25);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Lunargenta" : "Silvermoon", GOSSIP_SENDER_MAIN, 26);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shattrath" : "Shattrath", GOSSIP_SENDER_MAIN, 11);
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Dalaran" : "Dalaran", GOSSIP_SENDER_MAIN, 202);
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                    SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                }
                break;

            case 2: // Azeroth Continents
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Reinos del Este" : "Eastern Kingdoms", GOSSIP_SENDER_MAIN, 27);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Kalimdor" : "Kalimdor", GOSSIP_SENDER_MAIN, 28);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 27: // Eastern Kingdoms
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Montanas de Alterac" : "Alterac Mountains", GOSSIP_SENDER_MAIN, 112);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Tierras Altas de Arati" : "Arathi Highlands", GOSSIP_SENDER_MAIN, 113);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Tierras Hinospitas" : "Badlands", GOSSIP_SENDER_MAIN, 114);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Las Tierras Devastadas" : "Blasted Lands", GOSSIP_SENDER_MAIN, 115);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Estepas Ardientes" : "Burning Steppes", GOSSIP_SENDER_MAIN, 116);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Paso de la Muerte" : "Deadwind Pass", GOSSIP_SENDER_MAIN, 117);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Dun Morogh" : "Dun Morogh", GOSSIP_SENDER_MAIN, 118);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Bosque del Ocaso" : "Duskwood", GOSSIP_SENDER_MAIN, 119);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Tierras de la Peste del Este" : "Eastern Plaguelands", GOSSIP_SENDER_MAIN, 120);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Bosque de Elwynn" : "Elwynn Forest", GOSSIP_SENDER_MAIN, 121);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Bosque Cancion Eterna" : "Eversong Woods", GOSSIP_SENDER_MAIN, 122);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Tierras Fantasma" : "Ghostlands", GOSSIP_SENDER_MAIN, 123);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Laderas de Trabalomas" : "Hillsbrad Foothills", GOSSIP_SENDER_MAIN, 124);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Isla de Quel'danas" : "Isle of Quel'Danas", GOSSIP_SENDER_MAIN, 125);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Pagina Siguiente" : "Next Page", GOSSIP_SENDER_MAIN, 996);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 996: // Eastern Kingdoms 2
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Loch Modan" : "Loch Modan", GOSSIP_SENDER_MAIN, 126);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Montanas Crestagrana" : "Redridge Mountains", GOSSIP_SENDER_MAIN, 127);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "La Garganta de Fuego" : "Searing Gorge", GOSSIP_SENDER_MAIN, 128);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Bosque de Argenteos" : "Silverpine Forest", GOSSIP_SENDER_MAIN, 129);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Vega de Tuercespina" : "Stranglethorn Vale", GOSSIP_SENDER_MAIN, 130);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Pantano de las Penas" : "Swamp of Sorrows", GOSSIP_SENDER_MAIN, 131);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Tierras del Interior" : "The Hinterlands", GOSSIP_SENDER_MAIN, 132);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Claros de Trisfal" : "Tirisfal Glades", GOSSIP_SENDER_MAIN, 133);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Tierras de la Peste del Oeste" : "Western Plaguelands", GOSSIP_SENDER_MAIN, 134);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Paramos de Poniente" : "Westfall", GOSSIP_SENDER_MAIN, 135);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Los Humedales" : "Wetlands", GOSSIP_SENDER_MAIN, 136);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Pagina Anterior" : "Previous Page", GOSSIP_SENDER_MAIN, 27);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 28: // Kalimdor
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Vallefresno" : "Ashenvale", GOSSIP_SENDER_MAIN, 137);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Azshara" : "Azshara", GOSSIP_SENDER_MAIN, 138);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Isla Bruma Azur" : "Azuremyst Isle", GOSSIP_SENDER_MAIN, 139);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Isla Bruma de Sangre" : "Bloodmyst Isle", GOSSIP_SENDER_MAIN, 140);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Costa Oscura" : "Darkshore", GOSSIP_SENDER_MAIN, 141);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Desolace" : "Desolace", GOSSIP_SENDER_MAIN, 142);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Durotar" : "Durotar", GOSSIP_SENDER_MAIN, 143);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Marjal Revolcafango" : "Dustwallow Marsh", GOSSIP_SENDER_MAIN, 144);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Frondavil" : "Felwood", GOSSIP_SENDER_MAIN, 145);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Feralas" : "Feralas", GOSSIP_SENDER_MAIN, 146);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Claro de la Luna" : "Moonglade", GOSSIP_SENDER_MAIN, 147);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mulgore" : "Mulgore", GOSSIP_SENDER_MAIN, 148);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Silithus" : "Silithus", GOSSIP_SENDER_MAIN, 149);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Pagina Siguiente" : "Next Page", GOSSIP_SENDER_MAIN, 995);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 995: // Kalimdor 2
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Sierra Espolon" : "Stonetalon Mountains", GOSSIP_SENDER_MAIN, 150);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Tanaris" : "Tanaris", GOSSIP_SENDER_MAIN, 151);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Teldrassil" : "Teldrassil", GOSSIP_SENDER_MAIN, 152);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Los Baldios" : "The Barrens", GOSSIP_SENDER_MAIN, 153);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Las Mil Agujas" : "Thousand Needles", GOSSIP_SENDER_MAIN, 154);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Crater de Un'Goro" : "Un'Goro Crater", GOSSIP_SENDER_MAIN, 155);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Cuna del Invierno" : "Winterspring", GOSSIP_SENDER_MAIN, 156);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Pagina Anterior" : "Previous Page", GOSSIP_SENDER_MAIN, 28);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 3: // Azeroth Instances
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Cavernas de Brazanegra" : "Blackfathom Deeps", GOSSIP_SENDER_MAIN, 29);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Profundidades de Roca Negra" : "Blackrock Depths", GOSSIP_SENDER_MAIN, 30);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "La Masacre" : "Dire Maul", GOSSIP_SENDER_MAIN, 31);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Gnomeregan" : "Gnomeregan", GOSSIP_SENDER_MAIN, 32);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Maraudon" : "Maraudon", GOSSIP_SENDER_MAIN, 33);
                if ((plyr == 2) || (plyr == 5) || (plyr == 6) || (plyr == 8) || (plyr == 10)) {
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Sima Ignea" : "Ragefire Chasm", GOSSIP_SENDER_MAIN, 34);
                }
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Zahurda Rajacieno" : "Razorfen Downs", GOSSIP_SENDER_MAIN, 35);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Horado Rajacieno" : "Razorfen Kraul", GOSSIP_SENDER_MAIN, 36);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Monasterio Escarlata" : "Scarlet Monastery", GOSSIP_SENDER_MAIN, 37);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Scholomance" : "Scholomance", GOSSIP_SENDER_MAIN, 38);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Castillo de Colmillo Oscuro" : "Shadowfang Keep", GOSSIP_SENDER_MAIN, 39);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Stratholme" : "Stratholme", GOSSIP_SENDER_MAIN, 40);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Templo Sumergido" : "Sunken Temple", GOSSIP_SENDER_MAIN, 41);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Las Minas de la Muerte" : "The Deadmines", GOSSIP_SENDER_MAIN, 42);
                if ((plyr == 1) || (plyr == 3) || (plyr == 4) || (plyr == 7) || (plyr == 11)) {
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Las Mazmorras de Ventormenta" : "The Stockade", GOSSIP_SENDER_MAIN, 43);
                } 

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Pagina Siguiente" : "Next Page", GOSSIP_SENDER_MAIN, 994);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 994: // Azeroth Instances Cont.
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Uldaman" : "Uldaman", GOSSIP_SENDER_MAIN, 44);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Cuevas de los Lamentos" : "Wailing Caverns", GOSSIP_SENDER_MAIN, 45);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Zul'Farrak" : "Zul'Farrak", GOSSIP_SENDER_MAIN, 46);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Pagina Anterior" : "Previous Page", GOSSIP_SENDER_MAIN, 3);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 4: // Azeroth Raids
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Guarida Alanegra" : "Blackwing Lair", GOSSIP_SENDER_MAIN, 47);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Nucleo de Magma" : "Molten Core", GOSSIP_SENDER_MAIN, 48);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Guarida de Onyxia" : "Onyxia's Lair", GOSSIP_SENDER_MAIN, 49);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Ruinas de Ahn'Qiraj" : "Ruins of Ahn'Qiraj", GOSSIP_SENDER_MAIN, 50);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Templo de Ahn'Qiraj" : "Temple of Ahn'Qiraj", GOSSIP_SENDER_MAIN, 51);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Zul'Gurub" : "Zul'Gurub", GOSSIP_SENDER_MAIN, 52);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 5: // Outland Locations
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Montanas Filospada" : "Blade's Edge Mountains", GOSSIP_SENDER_MAIN, 53);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Peninsula Infernal" : "Hellfire Peninsula", GOSSIP_SENDER_MAIN, 54);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Nagrand" : "Nagrand", GOSSIP_SENDER_MAIN, 55);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Tormenta Abisal" : "Netherstorm", GOSSIP_SENDER_MAIN, 56);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Valle Sombraluna" : "Shadowmoon Valley", GOSSIP_SENDER_MAIN, 57);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Bosque de Terokkar" : "Terokkar Forest", GOSSIP_SENDER_MAIN, 58);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Marisma de Zangar" : "Zangarmarsh", GOSSIP_SENDER_MAIN, 59);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 6: // Outland Instances
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Auchindoun" : "Auchindoun", GOSSIP_SENDER_MAIN, 60);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Cavernas del tiempo" : "Caverns of Time", GOSSIP_SENDER_MAIN, 61);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Reserva Colmillo Torcido" : "Coilfang Reservoir", GOSSIP_SENDER_MAIN, 62);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Ciudadela del Fuego Infernal" : "Hellfire Citadel", GOSSIP_SENDER_MAIN, 63);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Bancal del Magister" : "Magisters' Terrace", GOSSIP_SENDER_MAIN, 64);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Castillo de la Tempestad" : "Tempest Keep", GOSSIP_SENDER_MAIN, 65);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 7: // Outland Raids
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Templo Oscuro" : "Black Temple", GOSSIP_SENDER_MAIN, 66);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "La Cima Hyjal" : "Hyjal Summit", GOSSIP_SENDER_MAIN, 67);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Caverna Santuario Serpiente" : "Serpentshrine Cavern", GOSSIP_SENDER_MAIN, 68);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Guarida de Gruul" : "Gruul's Lair", GOSSIP_SENDER_MAIN, 69);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Guarida de Magtheridon" : "Magtheridon's Lair", GOSSIP_SENDER_MAIN, 70);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Karazhan" : "Karazhan", GOSSIP_SENDER_MAIN, 71);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Meseta de La Fuente del Sol" : "Sunwell Plateau", GOSSIP_SENDER_MAIN, 72);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "El ojo" : "The Eye", GOSSIP_SENDER_MAIN, 73);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Zul'Aman" : "Zul'Aman", GOSSIP_SENDER_MAIN, 74);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 8: // Northrend Locations
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Tundra Boreal" : "Borean Tundra", GOSSIP_SENDER_MAIN, 75);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Bosque Canto de Cristal" : "Crystalsong Forest", GOSSIP_SENDER_MAIN, 76);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Cementerio de Dragones" : "DragonBlight", GOSSIP_SENDER_MAIN, 77);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Colinas Pardas" : "Grizzly Hills", GOSSIP_SENDER_MAIN, 78);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Fiordo Aquilonal" : "Howling Fjord", GOSSIP_SENDER_MAIN, 79);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Corona de Hielo" : "Icecrown", GOSSIP_SENDER_MAIN, 80);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Cuenca de Sholazar" : "Sholazar Basin", GOSSIP_SENDER_MAIN, 81);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Las Cumbres Tormentosas" : "The Storm Peaks", GOSSIP_SENDER_MAIN, 82);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Conquista del Invierno" : "WinterGrasp", GOSSIP_SENDER_MAIN, 83);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Zul'Drak" : "Zul'Drak", GOSSIP_SENDER_MAIN, 84);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 9: // Northrend Instances
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Azjol-Nerub" : "Azjol-Nerub", GOSSIP_SENDER_MAIN, 85);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Fortaleza de Drak'Tharon" : "Drak'Tharon Keep", GOSSIP_SENDER_MAIN, 86);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Gundrak" : "Gundrak", GOSSIP_SENDER_MAIN, 87);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "La Matanza de Stratholme" : "The Culling of Stratholme", GOSSIP_SENDER_MAIN, 88);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Camaras de Relampagos" : "The Halls of Lightning", GOSSIP_SENDER_MAIN, 89);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Camaras de Piedra" : "The Halls of Stone", GOSSIP_SENDER_MAIN, 90);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "El Nexo" : "The Nexus", GOSSIP_SENDER_MAIN, 91);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "El Bastion Violeta" : "The Violet Hold", GOSSIP_SENDER_MAIN, 92);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Fortaleza de Utgarde" : "Utgarde Keep", GOSSIP_SENDER_MAIN, 93);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Pinaculo de Utgarde" : "Utgarde Pinnacle", GOSSIP_SENDER_MAIN, 94);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Las Camaras Heladas" : "The Frozen Halls", GOSSIP_SENDER_MAIN, 204);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 10: // Northrend Raids
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Naxxramas" : "Naxxramas", GOSSIP_SENDER_MAIN, 95);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "El ojo de la Eternidad" : "The Eye of Eternity", GOSSIP_SENDER_MAIN, 96);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Sagrario Obsidiana" : "The Obsidian Sanctum", GOSSIP_SENDER_MAIN, 97);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Ulduar" : "Ulduar", GOSSIP_SENDER_MAIN, 98);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "La Camara de Archavon" : "Vault of Archavon", GOSSIP_SENDER_MAIN, 99);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Ciudadela de la Corona de Hielo" : "Icecrown Citadel", GOSSIP_SENDER_MAIN, 201);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Sagrario Rubi" : "Rubi Sanctum", GOSSIP_SENDER_MAIN, 205);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Prueba del Cruzado" : "Trial of the Crusader", GOSSIP_SENDER_MAIN, 203);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;


            case 11:  // Shattrath
                player->TeleportTo(530, -1817.82, 5453.04, -12.42, 0);
                CloseGossipMenuFor(player);
                break;

            case 13: // Gurubashi Arena
                player->TeleportTo(0, -13261.30, 164.45, 35.78, 0);
                CloseGossipMenuFor(player);
                break;

            case 14: // Shop VIP
                player->TeleportTo(1, -8501.3398, 2022.63, 104.75, 3.5727);
                CloseGossipMenuFor(player);
                break;

            case 15: // Climbing circuits
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Circuito 1" : "Circuit 1", GOSSIP_SENDER_MAIN, 1000);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Circuito 2" : "Circuit 2", GOSSIP_SENDER_MAIN, 1001);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Circuito 3 (Contrarreloj 25s)" : "Circuit 3 (Time trial 25s)", GOSSIP_SENDER_MAIN, 1002);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Circuito 4 (Contrarreloj 35s)" : "Circuit 4 (Time trial 35s)", GOSSIP_SENDER_MAIN, 1003);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Circuito 5 (Imposible)" : "Circuit 5 (Imposible)", GOSSIP_SENDER_MAIN, 1004);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Inicio" : "Home Page", GOSSIP_SENDER_MAIN, 999);
                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;

            case 16: // PvP Zone
                if (group)
                {
                    if (group->GetMembersCount() > 3)
                    {
                        isOkGroup = false;
                    }
                }

                if (isOkGroup)
                {
                    player->CastSpell(player, 35517, false);
                    player->TeleportTo(0, -14599.00f, -237.766f, 24.85f, 2.43732f);
                    ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ?
                        "|cffff6060[Informacion]:|r Has sido teletransportado a la |cFFFF4500Isla PvP|r!"
                        : "|cffff6060[Information]:|r You have been teleported to the |cFFFF4500PvP Island|r!");
                }
                else
                {
                    ChatHandler(player->GetSession()).PSendSysMessage(IsSpanishPlayer(player) ?
                        "|cffff6060[Informacion]:|r La Isla PvP no acepta grupos de mas de 3 personas!"
                        : "|cffff6060[Information]:|r Groups of 3 or more people are not allowed in PvP Island!");
                }

                CloseGossipMenuFor(player);
                break;

            case 1000: // Climbing circuits 1
                player->TeleportTo(1, -3841.865f, 1113.265f, 154.789f, 0.899f);
                CloseGossipMenuFor(player);
                break;
            case 1001: // Climbing circuits 2
                player->TeleportTo(1, -4470.526f, 3748.295f, 0.4675f, 1.5844f);
                CloseGossipMenuFor(player);
                break;
            case 1002: // Climbing circuits 3
                player->TeleportTo(1, 3268.3798f, -6882.5f, 2.2341f, 2.6153f);
                CloseGossipMenuFor(player);
                break;
            case 1003: // Climbing circuits 4
                player->TeleportTo(1, -652.3859f, -5694.25f, 3.63647f, 5.60288f);
                CloseGossipMenuFor(player);
                break;
            case 1004: // Climbing circuits 5
                player->TeleportTo(0, -106.801003f, -4658.04f, 6.17067f, 4.52734f);
                CloseGossipMenuFor(player);
                break;

                // Alliance Cities

            case 19: // Stormwind
                player->TeleportTo(0, -8913.23, 554.63, 93.79, 0);
                CloseGossipMenuFor(player);
                break;
            case 20: // Ironforge
                player->TeleportTo(0, -4982.16, -880.75, 501.65, 0);
                CloseGossipMenuFor(player);
                break;
            case 21: // Darnassus
                player->TeleportTo(1, 9945.49, 2609.89, 1316.26, 0);
                CloseGossipMenuFor(player);
                break;
            case 22: // Exodar
                player->TeleportTo(530, -4002.67, -11875.54, -0.71, 0);
                CloseGossipMenuFor(player);
                break;

                // Horde Cities

            case 23: // Orgimmar
                player->TeleportTo(1, 1502.71, -4415.41, 21.77, 0);
                CloseGossipMenuFor(player);
                break;
            case 24: // Thunderbluff
                player->TeleportTo(1, -1273.2671f, 119.81665f, 131.250702f, 0);
                CloseGossipMenuFor(player);
                break;
            case 25: // Undercity
                player->TeleportTo(0, 1831.26, 238.52, 60.52, 0);
                CloseGossipMenuFor(player);
                break;
            case 26: // Silvermoon
                player->TeleportTo(530, 9398.75, -7277.41, 14.21, 0);
                CloseGossipMenuFor(player);
                break;


            case 29: // Blackfathom Deeps
                player->TeleportTo(1, 4247.34, 744.05, -24.71, 0);
                CloseGossipMenuFor(player);
                break;
            case 30: // Blackrock Depths
                player->TeleportTo(0, -7576.74, -1126.68, 262.26, 0);
                CloseGossipMenuFor(player);
                break;
            case 31: // Dire Maul
                player->TeleportTo(1, -3879.52, 1095.26, 154.78, 0);
                CloseGossipMenuFor(player);
                break;
            case 32: // Gnomeregan
                player->TeleportTo(0, -5162.63, 923.21, 257.17, 0);
                CloseGossipMenuFor(player);
                break;
            case 33: // Maraudon
                player->TeleportTo(1, -1412.73, 2816.92, 112.64, 0);
                CloseGossipMenuFor(player);
                break;
            case 34: // Ragefire Chasm
                player->TeleportTo(1, 1814.17, -4401.13, -17.67, 0);
                CloseGossipMenuFor(player);
                break;
            case 35: // Razorfen Downs
                player->TeleportTo(1, -4378.32, -1949.14, 88.57, 0);
                CloseGossipMenuFor(player);
                break;
            case 36: // Razorfen Kraul
                player->TeleportTo(1, -4473.31, -1810.05, 86.11, 0);
                CloseGossipMenuFor(player);
                break;
            case 37: // Scarlet Monastery
                player->TeleportTo(0, 2881.84, -816.23, 160.33, 0);
                CloseGossipMenuFor(player);
                break;
            case 38: // Scholomance
                player->TeleportTo(0, 1229.45, -2576.66, 90.43, 0);
                CloseGossipMenuFor(player);
                break;
            case 39: // Shadowfang Keep
                player->TeleportTo(0, -243.85, 1517.21, 76.23, 0);
                CloseGossipMenuFor(player);
                break;
            case 40: // Stratholme
                player->TeleportTo(0, 3362.14, -3380.05, 144.78, 0);
                CloseGossipMenuFor(player);
                break;
            case 41: // Sunken Temple
                player->TeleportTo(0, -10452.32, -3817.51, 18.06, 0);
                CloseGossipMenuFor(player);
                break;
            case 42: // The Deadmines
                player->TeleportTo(0, -11084.10, 1556.17, 48.12, 0);
                CloseGossipMenuFor(player);
                break;
            case 43: // The Stockade
                player->TeleportTo(0, -8797.29, 826.67, 97.63, 0);
                CloseGossipMenuFor(player);
                break;
            case 44: // Uldaman
                player->TeleportTo(0, -6072.23, -2955.94, 209.61, 0);
                CloseGossipMenuFor(player);
                break;
            case 45: // Wailing Caverns
                player->TeleportTo(1, -735.11, -2214.21, 16.83, 0);
                CloseGossipMenuFor(player);
                break;
            case 46: // Zul'Farrak
                player->TeleportTo(1, -6825.69, -2882.77, 8.91, 0);
                CloseGossipMenuFor(player);
                break;

                // Azeroth Raids

            case 47: // Blackwing Lair
                player->TeleportTo(469, -7666.11, -1101.53, 399.67, 0);
                CloseGossipMenuFor(player);
                break;
            case 48: // Molten Core
                player->TeleportTo(230, 1117.61, -457.36, -102.49, 0);
                CloseGossipMenuFor(player);
                break;
            case 49: // Onyxia's Lair
                player->TeleportTo(1, -4697.81, -3720.44, 50.35, 0);
                CloseGossipMenuFor(player);
                break;
            case 50: // Ruins of Ahn'Qiraj
                player->TeleportTo(1, -8380.47, 1480.84, 14.35, 0);
                CloseGossipMenuFor(player);
                break;
            case 51: // Temple of Ahn'Qiraj
                player->TeleportTo(1, -8258.27, 1962.73, 129.89, 0);
                CloseGossipMenuFor(player);
                break;
            case 52: // Zul'Gurub
                player->TeleportTo(0, -11916.74, -1203.32, 92.28, 0);
                CloseGossipMenuFor(player);
                break;

                // Outland Locations

            case 53: // Blade's Edge Mountains
                player->TeleportTo(530, 2039.24, 6409.27, 134.30, 0);
                CloseGossipMenuFor(player);
                break;
            case 54: // Hellfire Peninsula
                player->TeleportTo(530, -247.37, 964.77, 84.33, 0);
                CloseGossipMenuFor(player);
                break;
            case 55: // Nagrand
                player->TeleportTo(530, -605.84, 8442.39, 60.76, 0);
                CloseGossipMenuFor(player);
                break;
            case 56: // Netherstorm
                player->TeleportTo(530, 3055.70, 3671.63, 142.44, 0);
                CloseGossipMenuFor(player);
                break;
            case 57: // Shadowmoon Valley
                player->TeleportTo(530, -2859.75, 3184.24, 9.76, 0);
                CloseGossipMenuFor(player);
                break;
            case 58: // Terokkar Forest
                player->TeleportTo(530, -1917.17, 4879.45, 2.10, 0);
                CloseGossipMenuFor(player);
                break;
            case 59: // Zangarmarsh
                player->TeleportTo(530, -206.61, 5512.90, 21.58, 0);
                CloseGossipMenuFor(player);
                break;

                // Outland Instances

            case 60: // Auchindoun
                player->TeleportTo(530, -3323.76, 4934.31, -100.21, 0);
                CloseGossipMenuFor(player);
                break;
            case 61: // Caverns of Time
                player->TeleportTo(1, -8187.16, -4704.91, 19.33, 0);
                CloseGossipMenuFor(player);
                break;
            case 62: // Coilfang Reservoir
                player->TeleportTo(530, 731.04, 6849.35, -66.62, 0);
                CloseGossipMenuFor(player);
                break;
            case 63: // Hellfire Citadel
                player->TeleportTo(530, -331.87, 3039.30, -16.66, 0);
                CloseGossipMenuFor(player);
                break;
            case 64: // Magisters' Terrace
                player->TeleportTo(530, 12884.92, -7333.78, 65.48, 0);
                CloseGossipMenuFor(player);
                break;
            case 65: // Tempest Keep
                player->TeleportTo(530, 3088.25, 1388.17, 185.09, 0);
                CloseGossipMenuFor(player);
                break;

                // Outland Raids

            case 66: // Black Temple
                player->TeleportTo(530, -3638.16, 316.09, 35.40, 0);
                CloseGossipMenuFor(player);
                break;
            case 67: // Hyjal Summit
                player->TeleportTo(1, -8175.94, -4178.52, -166.74, 0);
                CloseGossipMenuFor(player);
                break;
            case 68: // Serpentshrine Cavern
                player->TeleportTo(530, 731.04, 6849.35, -66.62, 0);
                CloseGossipMenuFor(player);
                break;
            case 69: // Gruul's Lair
                player->TeleportTo(530, 3528.99, 5133.50, 1.31, 0);
                CloseGossipMenuFor(player);
                break;
            case 70: // Magtheridon's Lair
                player->TeleportTo(530, -337.50, 3131.88, -102.92, 0);
                CloseGossipMenuFor(player);
                break;
            case 71: // Karazhan
                player->TeleportTo(0, -11119.22, -2010.73, 47.09, 0);
                CloseGossipMenuFor(player);
                break;
            case 72: // Sunwell Plateau
                player->TeleportTo(530, 12560.79, -6774.58, 15.08, 0);
                CloseGossipMenuFor(player);
                break;
            case 73: // The Eye
                player->TeleportTo(530, 3088.25, 1388.17, 185.09, 0);
                CloseGossipMenuFor(player);
                break;
            case 74: // Zul'Aman
                player->TeleportTo(530, 6850, -7950, 170, 0);
                CloseGossipMenuFor(player);
                break;

                // Northrend Locations

            case 75: // Borean Tundra
                player->TeleportTo(571, 2920.15, 4043.40, 1.82, 0);
                CloseGossipMenuFor(player);
                break;
            case 76: // Crystalsong Forest
                player->TeleportTo(571, 5371.18, 109.11, 157.65, 0);
                CloseGossipMenuFor(player);
                break;
            case 77: // Dragonblight
                player->TeleportTo(571, 2729.59, 430.70, 66.98, 0);
                CloseGossipMenuFor(player);
                break;
            case 78: // Grizzly Hills
                player->TeleportTo(571, 3587.20, -4545.12, 198.75, 0);
                CloseGossipMenuFor(player);
                break;
            case 79: // Howling Fjord
                player->TeleportTo(571, 154.39, -4896.33, 296.14, 0);
                CloseGossipMenuFor(player);
                break;
            case 80: // Icecrown
                player->TeleportTo(571, 8406.89, 2703.79, 665.17, 0);
                CloseGossipMenuFor(player);
                break;
            case 81: // Sholazar Basin
                player->TeleportTo(571, 5569.49, 5762.99, -75.22, 0);
                CloseGossipMenuFor(player);
                break;
            case 82: // The Storm Peaks
                player->TeleportTo(571, 6180.66, -1085.65, 415.54, 0);
                CloseGossipMenuFor(player);
                break;
            case 83: // Wintergrasp
                player->TeleportTo(571, 5044.03, 2847.23, 392.64, 0);
                CloseGossipMenuFor(player);
                break;
            case 84: // Zul'Drak
                player->TeleportTo(571, 4700.09, -3306.54, 292.41, 0);
                CloseGossipMenuFor(player);
                break;

                // Northrend Instances

            case 85: // Azjol-Nerub
                player->TeleportTo(571, 3738.93, 2164.14, 37.29, 0);
                CloseGossipMenuFor(player);
                break;
            case 86: // Drak'Tharon
                player->TeleportTo(571, 4772.13, -2035.85, 229.38, 0);
                CloseGossipMenuFor(player);
                break;
            case 87: // Gundrak
                player->TeleportTo(571, 6937.12, -4450.80, 450.90, 0);
                CloseGossipMenuFor(player);
                break;
            case 88: // The Culling of Stratholme
                player->TeleportTo(1, -8746.94, -4437.69, -199.98, 0);
                CloseGossipMenuFor(player);
                break;
            case 89: // The Halls of Lightning
                player->TeleportTo(571, 9171.01, -1375.94, 1099.55, 0);
                CloseGossipMenuFor(player);
                break;
            case 90: // The Halls of Stone
                player->TeleportTo(571, 8921.35, -988.56, 1039.37, 0);
                CloseGossipMenuFor(player);
                break;
            case 91: // The Nexus
                player->TeleportTo(571, 3784.76, 6941.97, 104.49, 0);
                CloseGossipMenuFor(player);
                break;
            case 92: // The Violet Hold
                player->TeleportTo(571, 5695.19, 505.38, 652.68, 0);
                CloseGossipMenuFor(player);
                break;
            case 93: // Utgarde Keep
                player->TeleportTo(571, 1222.44, -4862.61, 41.24, 0);
                CloseGossipMenuFor(player);
                break;
            case 94: // Utgarde Pinnacle
                player->TeleportTo(571, 1251.10, -4856.31, 215.86, 0);
                CloseGossipMenuFor(player);
                break;

                // Northrend Raids

            case 95: // Naxxramas
                player->TeleportTo(571, 3669.77, -1275.48, 243.51, 0);
                CloseGossipMenuFor(player);
                break;
            case 96: // The Eye of Eternity
                player->TeleportTo(571, 3873.50, 6974.83, 152.04, 0);
                CloseGossipMenuFor(player);
                break;
            case 97: // The Obsidian Sanctum
                player->TeleportTo(571, 3458.590576f, 262.752f, -114.4711f, 3.2161f);
                CloseGossipMenuFor(player);
                break;
            case 98: // Ulduar
                player->TeleportTo(571, 9330.53, -1115.40, 1245.14, 0);
                CloseGossipMenuFor(player);
                break;
            case 99: // Vault of Archavon
                player->TeleportTo(571, 5410.21, 2842.37, 418.67, 0);
                CloseGossipMenuFor(player);
                break;

                // Eastern Kingdoms

            case 112: // Alterac Mountains
                player->TeleportTo(0, 353.79, -607.08, 150.76, 0);
                CloseGossipMenuFor(player);
                break;
            case 113: // Arathi Highlands
                player->TeleportTo(0, -2269.78, -2501.06, 79.04, 0);
                CloseGossipMenuFor(player);
                break;
            case 114: // Badlands
                player->TeleportTo(0, -6026.58, -3318.27, 260.64, 0);
                CloseGossipMenuFor(player);
                break;
            case 115: // Blasted Lands
                player->TeleportTo(0, -10797.67, -2994.29, 44.42, 0);
                CloseGossipMenuFor(player);
                break;
            case 116: // Burning Steppes
                player->TeleportTo(0, -8357.72, -2537.49, 135.01, 0);
                CloseGossipMenuFor(player);
                break;
            case 117: // Deadwind Pass
                player->TeleportTo(0, -10460.22, -1699.33, 81.85, 0);
                CloseGossipMenuFor(player);
                break;
            case 118: // Dun Morogh
                player->TeleportTo(0, -6234.99, 341.24, 383.22, 0);
                CloseGossipMenuFor(player);
                break;
            case 119: // Duskwood
                player->TeleportTo(0, -10068.30, -1501.07, 28.41, 0);
                CloseGossipMenuFor(player);
                break;
            case 120: // Eastern Plaguelands
                player->TeleportTo(0, 1924.70, -2653.54, 59.70, 0);
                CloseGossipMenuFor(player);
                break;
            case 121: // Elwynn Forest
                player->TeleportTo(0, -8939.71, -131.22, 83.62, 0);
                CloseGossipMenuFor(player);
                break;
            case 122: // Eversong Woods
                player->TeleportTo(530, 10341.73, -6366.29, 34.31, 0);
                CloseGossipMenuFor(player);
                break;
            case 123: // Ghostlands
                player->TeleportTo(530, 7969.87, -6872.63, 58.66, 0);
                CloseGossipMenuFor(player);
                break;
            case 124: // Hillsbrad Foothills
                player->TeleportTo(0, -585.70, 612.18, 83.80, 0);
                CloseGossipMenuFor(player);
                break;
            case 125: // Isle of Quel'Danas
                player->TeleportTo(530, 12916.81, -6867.82, 7.69, 0);
                CloseGossipMenuFor(player);
                break;
            case 126: // Loch Modan
                player->TeleportTo(0, -4702.59, -2698.61, 318.75, 0);
                CloseGossipMenuFor(player);
                break;
            case 127: // Redridge Mountains
                player->TeleportTo(0, -9600.62, -2123.21, 66.23, 0);
                CloseGossipMenuFor(player);
                break;
            case 128: // Searing Gorge
                player->TeleportTo(0, -6897.73, -1821.58, 241.16, 0);
                CloseGossipMenuFor(player);
                break;
            case 129: // Silverpine Forest
                player->TeleportTo(0, 1499.57, 623.98, 47.01, 0);
                CloseGossipMenuFor(player);
                break;
            case 130: // Stranglethorn Vale
                player->TeleportTo(0, -11355.90, -383.40, 65.14, 0);
                CloseGossipMenuFor(player);
                break;
            case 131: // Swamp of Sorrows
                player->TeleportTo(0, -10552.60, -2355.25, 85.95, 0);
                CloseGossipMenuFor(player);
                break;
            case 132: // The Hinterlands
                player->TeleportTo(0, 92.63, -1942.31, 154.11, 0);
                CloseGossipMenuFor(player);
                break;
            case 133: // Tirisfal Glades
                player->TeleportTo(0, 1676.13, 1669.37, 137.02, 0);
                CloseGossipMenuFor(player);
                break;
            case 134: // Western Plaguelands
                player->TeleportTo(0, 1635.57, -1068.50, 66.57, 0);
                CloseGossipMenuFor(player);
                break;
            case 135: // Westfall
                player->TeleportTo(0, -9827.95, 865.80, 25.80, 0);
                CloseGossipMenuFor(player);
                break;
            case 136: // Wetlands
                player->TeleportTo(0, -4086.32, -2620.72, 43.55, 0);
                CloseGossipMenuFor(player);
                break;

                // Kalimdor

            case 137: // Ashenvale
                player->TeleportTo(1, 3474.41, 853.47, 5.76, 0);
                CloseGossipMenuFor(player);
                break;
            case 138: // Azshara
                player->TeleportTo(1, 2763.93, -3881.34, 92.52, 0);
                CloseGossipMenuFor(player);
                break;
            case 139: // Azuremyst Isle
                player->TeleportTo(530, -3972.72, -13914.99, 98.88, 0);
                CloseGossipMenuFor(player);
                break;
            case 140: // Bloodmyst Isle
                player->TeleportTo(530, -2721.67, -12208.90, 9.08, 0);
                CloseGossipMenuFor(player);
                break;
            case 141: // Darkshore
                player->TeleportTo(1, 4336.61, 173.83, 46.84, 0);
                CloseGossipMenuFor(player);
                break;
            case 142: // Desolace
                player->TeleportTo(1, 47.28, 1684.64, 93.55, 0);
                CloseGossipMenuFor(player);
                break;
            case 143: // Durotar
                player->TeleportTo(1, -611.61, -4263.16, 38.95, 0);
                CloseGossipMenuFor(player);
                break;
            case 144: // Dustwallow Marsh
                player->TeleportTo(1, -3682.58, -2556.93, 58.43, 0);
                CloseGossipMenuFor(player);
                break;
            case 145: // Felwood
                player->TeleportTo(1, 3590.56, -1516.69, 169.98, 0);
                CloseGossipMenuFor(player);
                break;
            case 146: // Feralas
                player->TeleportTo(1, -4300.02, -631.56, -9.35, 0);
                CloseGossipMenuFor(player);
                break;
            case 147: // Moonglade
                player->TeleportTo(1, 7999.68, -2670.19, 512.09, 0);
                CloseGossipMenuFor(player);
                break;
            case 148: // Mulgore
                player->TeleportTo(1, -2931.49, -262.82, 53.25, 0);
                CloseGossipMenuFor(player);
                break;
            case 149: // Silithus
                player->TeleportTo(1, -6814.57, 833.77, 49.74, 0);
                CloseGossipMenuFor(player);
                break;
            case 150: // Stonetalon Mountains
                player->TeleportTo(1, -225.34, -765.16, 6.4, 0);
                CloseGossipMenuFor(player);
                break;
            case 151: // Tanaris
                player->TeleportTo(1, -6999.47, -3707.94, 26.44, 0);
                CloseGossipMenuFor(player);
                break;
            case 152: // Teldrassil
                player->TeleportTo(1, 8754.06, 949.62, 25.99, 0);
                CloseGossipMenuFor(player);
                break;
            case 153: // The Barrens
                player->TeleportTo(1, -948.46, -3738.60, 5.98, 0);
                CloseGossipMenuFor(player);
                break;
            case 154: // Thousand Needles
                player->TeleportTo(1, -4685.72, -1836.24, -44.04, 0);
                CloseGossipMenuFor(player);
                break;
            case 155: // Un'Goro Crater
                player->TeleportTo(1, -6162.47, -1098.74, -208.99, 0);
                CloseGossipMenuFor(player);
                break;
            case 156: // Winterspring
                player->TeleportTo(1, 6896.27, -2302.51, 586.69, 0);
                CloseGossipMenuFor(player);
                break;

            case 201: // ICC
                player->TeleportTo(571, 5793.48, 2073.17, 636.063, 3.59383);
                CloseGossipMenuFor(player);
                break;

            case 202: // Dalaran
                player->TeleportTo(571, 5804.15, 624.771, 647.767, 1.64);
                CloseGossipMenuFor(player);
                break;

            case 203: // Trial of the Crusader
                player->TeleportTo(571, 8515.2930f, 729.6298f, 558.247742f, 1.569216f);
                CloseGossipMenuFor(player);
                break;

            case 204: // FOS/POS/HOR
                player->TeleportTo(571, 5645.732422f, 2024.865479f, 798.270264f, 0);
                CloseGossipMenuFor(player);
                break;

            case 205: // Rubi Sanctum
                player->TeleportTo(571, 3599.361572f, 199.044418f, -113.959305f, 5.329687f);
                CloseGossipMenuFor(player);
                break;

            case 5000: // Shop tanaris
                player->TeleportTo(1, -11823.9, -4779.58, 5.9206, 1.1357);
                CloseGossipMenuFor(player);
                break;

            case 5001: // Shop Utgarde
                player->TeleportTo(571, 1233.63f, -4979.2002f, 173.929001f, 5.25595f);
                CloseGossipMenuFor(player);
                break;

            case 5002: // Shop PvP
                player->TeleportTo(1, -9604.881836f, -2782.960205f, 8.195462f, 0.010976f);
                CloseGossipMenuFor(player);
                break;
            case 5003: // Guild House
                TeleportGuildHouse(player->GetGuild(), player);
                CloseGossipMenuFor(player);
                break;
            case 5004: // Mazmorra custom (5HC)
                player->TeleportTo(530, -295.456421f, 3151.562744f, 32.035973f, 2.178201f);
                CloseGossipMenuFor(player);
                break;

            case 0:
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop Tanaris" : "Shop Tanaris", GOSSIP_SENDER_MAIN, 5000);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop Utgarde" : "Shop Utgarde", GOSSIP_SENDER_MAIN, 5001);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop PvP" : "Shop PvP", GOSSIP_SENDER_MAIN, 5002);

                result = CharacterDatabase.PQuery("SELECT AccountId FROM premium WHERE active = 1 AND AccountId = %u", player->GetSession()->GetAccountId());
                if (result) {
                    isVipPlayer = true;
                }
                if (isVipPlayer)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Shop VIP" : "Shop VIP", GOSSIP_SENDER_MAIN, 14);
                }

                has_gh = CharacterDatabase.PQuery("SELECT id, `guild` FROM `guild_house` WHERE guild = %u", player->GetGuildId());
                if (has_gh)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_TABARD, isSpanish ? "Casa de Hermandad" : "Guild House", GOSSIP_SENDER_MAIN, 5003);
                }

                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Mazmorra custom (5HC)" : "Custom dungeon (5HC)", GOSSIP_SENDER_MAIN, 5004);

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Circuitos escalada" : "Climbing circuits", GOSSIP_SENDER_MAIN, 15);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Ciudades principales" : "Main Cities", GOSSIP_SENDER_MAIN, 1);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Localizaciones Azeroth" : "Azeroth Locations", GOSSIP_SENDER_MAIN, 2);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mazmorras Clasicas" : "Classic Dungeons", GOSSIP_SENDER_MAIN, 3);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Bandas Clasicas" : "Classic Raids", GOSSIP_SENDER_MAIN, 4);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Localizaciones Terrallende" : "Outland Locations", GOSSIP_SENDER_MAIN, 5);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mazmorras TBC" : "TBC Dungeons", GOSSIP_SENDER_MAIN, 6);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Bandas TBC" : "TBC Raids", GOSSIP_SENDER_MAIN, 7);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Localizaciones Rasganorte" : "Northrend Locations", GOSSIP_SENDER_MAIN, 8);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Mazmorras WOTLK" : "WOTLK Dungeons", GOSSIP_SENDER_MAIN, 9);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Bandas WOTLK" : "WOTLK Raids", GOSSIP_SENDER_MAIN, 10);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Gurubashi Arena" : "Gurubashi Arena", GOSSIP_SENDER_MAIN, 13);
                AddGossipItemFor(player, GOSSIP_ICON_TAXI, isSpanish ? "Isla PvP" : "PvP Island", GOSSIP_SENDER_MAIN, 16);

                SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
                break;
            }
        //}

        return true;
    }

    struct custom_npc_teleporterAI : public ScriptedAI
    {
        custom_npc_teleporterAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_npc_teleporterAI(creature);
    }
};

void AddSC_custom_npc_teleporter()
{
	new custom_npc_teleporter();
}
