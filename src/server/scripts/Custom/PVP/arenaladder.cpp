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

#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "ObjectAccessor.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include "AccountMgr.h"
#include <cstring>
#include <string.h>

enum ArenaRankActionIds {
    ARENA_2V2_LADDER = GOSSIP_ACTION_INFO_DEF + 1,
    ARENA_3V3_LADDER = GOSSIP_ACTION_INFO_DEF + 2,
    ARENA_5V5_LADDER = GOSSIP_ACTION_INFO_DEF + 3,
    /* Cuando haya soloq */
    //ARENA_3v3SOLO_LADDER = GOSSIP_ACTION_INFO_DEF + 4,
    ARENA_GOODBYE = GOSSIP_ACTION_INFO_DEF + 5,
    ARENA_NOOP = 1,
    ARENA_START_TEAM_LOOKUP = GOSSIP_ACTION_INFO_DEF + 6,    
};

enum ArenaRankOptions {
    ARENA_MAX_RESULTS = 30,
};

enum ArenaGossipText {
    ARENA_GOSSIP_HELLO = 60009,
    ARENA_GOSSIP_NOTEAMS = 11202,
    ARENA_GOSSIP_TEAMS = 11203,
    ARENA_GOSSIP_TEAM_LOOKUP = 11204,    
};

class ArenaTeamRanks : public CreatureScript
{
private:
    static uint32 optionToTeamType(uint32 option) {
        uint32 teamType;
        switch (option) {
        case ARENA_2V2_LADDER: teamType = 2; break;
        case ARENA_3V3_LADDER: teamType = 3; break;
        case ARENA_5V5_LADDER: teamType = 5; break;
            /* Cuando haya soloq */
        //case ARENA_3v3SOLO_LADDER: teamType = 5; break;
        }
        return teamType;
    }
    static uint32 teamTypeToOption(uint32 teamType) {
        uint32 option;
        switch (teamType) {
        case 2: option = ARENA_2V2_LADDER; break;
        case 3: option = ARENA_3V3_LADDER; break;
        case 5: option = ARENA_5V5_LADDER; break;
            /* Cuando haya soloq */
        //case 5: option = ARENA_3v3SOLO_LADDER; break;
        }
        return option;
    }

    static std::string raceToString(uint8 race) {
        std::string race_s = "Unknown";
        switch (race)
        {
        case RACE_HUMAN:            race_s = "Human";       break;
        case RACE_ORC:              race_s = "Orc";         break;
        case RACE_DWARF:            race_s = "Dwarf";       break;
        case RACE_NIGHTELF:         race_s = "Night Elf";   break;
        case RACE_UNDEAD_PLAYER:    race_s = "Undead";      break;
        case RACE_TAUREN:           race_s = "Tauren";      break;
        case RACE_GNOME:            race_s = "Gnome";       break;
        case RACE_TROLL:            race_s = "Troll";       break;
        case RACE_BLOODELF:         race_s = "Blood Elf";   break;
        case RACE_DRAENEI:          race_s = "Draenei";     break;
        }
        return race_s;
    }

    static std::string classToString(uint8 Class) {
        std::string Class_s = "Unknown";
        switch (Class)
        {
        case CLASS_WARRIOR:         Class_s = "Warrior";        break;
        case CLASS_PALADIN:         Class_s = "Paladin";        break;
        case CLASS_HUNTER:          Class_s = "Hunter";         break;
        case CLASS_ROGUE:           Class_s = "Rogue";          break;
        case CLASS_PRIEST:          Class_s = "Priest";         break;
        case CLASS_DEATH_KNIGHT:    Class_s = "Death Knight";   break;
        case CLASS_SHAMAN:          Class_s = "Shaman";         break;
        case CLASS_MAGE:            Class_s = "Mage";           break;
        case CLASS_WARLOCK:         Class_s = "Warlock";        break;
        case CLASS_DRUID:           Class_s = "Druid";          break;
        }
        return Class_s;
    }

    static std::string getPlayerStatus(uint64 guid) {
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player)
            return "Offline";
        if (player->isAFK())
            //  return "Online, <AFK> " + player->afkMsg; 
            if (player->isDND())
                //  return "Online, <Busy> " + player->dndMsg;
                return "Online";
        return "Offline";
    }

    static std::string getWinPercent(uint32 wins, uint32 losses) {
        uint32 totalGames = wins + losses;
        if (totalGames == 0)
            return "0%";

        std::stringstream buf;
        uint32 percentage = (wins * 100) / totalGames;
        buf << percentage << "%";
        return buf.str();
    }

public:

    ArenaTeamRanks() : CreatureScript("ArenaTeamRanks") { }

    bool OnGossipHello(Player* player, Creature* creature)  override
    {
        bool isSpanish = player->hasSpanishClient();

        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "|TInterface\\icons\\Achievement_Arena_5v5_7:35:35:-30:0|tTOP 1v1" : "|TInterface\\icons\\Achievement_Arena_5v5_7:35:35:-30:0|tTOP 1v1", GOSSIP_SENDER_MAIN, ARENA_5V5_LADDER);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "|TInterface\\icons\\Achievement_Arena_2v2_7:35:35:-30:0|tTOP 2v2" : "|TInterface\\icons\\Achievement_Arena_2v2_7:35:35:-30:0|tTOP 2v2", GOSSIP_SENDER_MAIN, ARENA_2V2_LADDER);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "|TInterface\\icons\\Achievement_Arena_3v3_7:35:35:-30:0|tTOP 3v3" : "|TInterface\\icons\\Achievement_Arena_3v3_7:35:35:-30:0|tTOP 3v3", GOSSIP_SENDER_MAIN, ARENA_3V3_LADDER);
        /* Cuando haya soloq */
        /*AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|TInterface\\icons\\Achievement_Arena_5v5_7:35:35:-30:0|tTop teams in 3vs3 SoloQueue", GOSSIP_SENDER_MAIN, ARENA_3v3SOLO_LADDER);*/

        SendGossipMenuFor(player, ARENA_GOSSIP_HELLO, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        bool isSpanish = player->hasSpanishClient();
        switch (action) {
        case 0:
            // Here should the magic happend
        case ARENA_GOODBYE:
        {
            player->PlayerTalkClass->SendCloseGossip();
            break;
        }
        case ARENA_2V2_LADDER:
        case ARENA_3V3_LADDER:
            /* Cuando haya soloq */
        //case ARENA_3v3SOLO_LADDER:
        case ARENA_5V5_LADDER:
        {
            uint32 teamType = optionToTeamType(action);
            QueryResult result = CharacterDatabase.PQuery(
                "SELECT arenaTeamid, name, rating, seasonWins, seasonGames - seasonWins "
                "FROM `arena_team` WHERE `type` = '%u' ORDER BY rating DESC LIMIT %u;", teamType, ARENA_MAX_RESULTS
            );

            if (!result) {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Adios" : "Goodbye", GOSSIP_SENDER_MAIN, ARENA_GOODBYE);
                SendGossipMenuFor(player, ARENA_GOSSIP_NOTEAMS, creature->GetGUID());
            }
            else {
                //uint64 rowCount = result->GetRowCount();
                std::string name;
                uint32 teamId, rating, seasonWins, seasonLosses, rank = 1;
                do {
                    Field* fields = result->Fetch();
                    teamId = fields[0].GetUInt32();
                    name = fields[1].GetString();
                    rating = fields[2].GetUInt32();
                    seasonWins = fields[3].GetUInt32();
                    seasonLosses = fields[4].GetUInt32();

                    std::stringstream buffer;
                    buffer << rank << ". [" << rating << "] " << name;
                    buffer << " (" << seasonWins << "-" << seasonLosses << ")";
                    AddGossipItemFor(player, GOSSIP_ICON_BATTLE, buffer.str(), GOSSIP_SENDER_MAIN, ARENA_START_TEAM_LOOKUP + teamId);

                    rank++;
                } while (result->NextRow());

                SendGossipMenuFor(player, ARENA_GOSSIP_TEAMS, creature->GetGUID());
            }
            break;
        }
        default:
        {
            if (action > ARENA_START_TEAM_LOOKUP)
            {
                uint32 teamId = action - ARENA_START_TEAM_LOOKUP;

                // lookup team
                QueryResult result = CharacterDatabase.PQuery(
                    //       0       1         2           3
                    "SELECT name, rating, seasonWins, seasonGames - seasonWins, "
                    //  4                 5            6        7         8
                    "weekWins, weekGames - weekWins, rank, captainGuid , type "
                    "FROM `arena_team` WHERE `arenaTeamId` = '%u'", teamId);

                // no team found
                if (!result) {
                    player->GetSession()->SendNotification(isSpanish ? "Equipo de arenas no encontrado..." : "Arena team not found...");
                    player->PlayerTalkClass->SendCloseGossip();
                    return true;
                }

                // populate the results
                Field* fields = result->Fetch();
                std::string name = fields[0].GetString();
                uint32 rating = fields[1].GetUInt32();
                uint32 seasonWins = fields[2].GetUInt32();
                uint32 seasonLosses = fields[3].GetUInt32();
                uint32 weekWins = fields[4].GetUInt32();
                uint32 weekLosses = fields[5].GetUInt32();
                uint32 rank = fields[6].GetUInt32();
                uint32 captainGuid = fields[7].GetUInt32();
                uint32 type = fields[8].GetUInt32();
                uint32 parentOption = teamTypeToOption(type);

                if (type == 5) type = 1;

                std::string seasonWinPercentage = getWinPercent(seasonWins, seasonLosses);
                std::string weekWinPercentage = getWinPercent(weekWins, weekLosses);

                std::stringstream buf;
                if (isSpanish)
                {
                    buf << "Nombre Equipo: " << name;
                    AddGossipItemFor(player, GOSSIP_ICON_BATTLE, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                    buf.str("");
                    buf << "Rating: " << rating << " (Rango " << rank << ", Grupo " << type << "v" << type << ")";
                    AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                    buf.str("");
                    buf << "Total Semana: " << weekWins << "-" << weekLosses << " (" << weekWinPercentage << " ganadas), " << (weekWins + weekLosses) << " jugadas";
                    AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                    buf.str("");
                    buf << "Total Season: " << seasonWins << "-" << seasonLosses << " (" << seasonWinPercentage << " ganadas), " << (seasonWins + seasonLosses) << " jugadas";
                    AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                }
                else
                {
                    buf << "Team Name: " << name;
                    AddGossipItemFor(player, GOSSIP_ICON_BATTLE, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                    buf.str("");
                    buf << "Rating: " << rating << " (Rank " << rank << ", Bracket " << type << "v" << type << ")";
                    AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                    buf.str("");
                    buf << "Total Week: " << weekWins << "-" << weekLosses << " (" << weekWinPercentage << " win), " << (weekWins + weekLosses) << " played";
                    AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                    buf.str("");
                    buf << "Total Season: " << seasonWins << "-" << seasonLosses << " (" << seasonWinPercentage << " win), " << (seasonWins + seasonLosses) << " played";
                    AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                }

                QueryResult members = CharacterDatabase.PQuery(
                    "SELECT "
                    // 0     1                 2           3
                    "a.guid, a.personalRating, a.weekWins, a.weekGames - a.weekWins, "
                    // 4           5
                    "a.seasonWins, a.seasonGames - seasonWins, "
                    // 6     7       8        9
                    "c.name, c.race, c.class, c.level "
                    "FROM arena_team_member a LEFT JOIN characters c ON a.guid = c.guid "
                    "WHERE arenaTeamId = '%u' "
                    "ORDER BY a.guid = '%u' DESC, a.seasonGames DESC, c.name ASC",
                    teamId, captainGuid);

                if (!members) {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "No se han encontrado miembros...?" : "No team members found...?", GOSSIP_SENDER_MAIN, parentOption);
                }
                else {
                    uint32 memberPos = 1;
                    uint32 memberCount = members->GetRowCount();
                    uint32 guid, personalRating, level;
                    std::string name, race, Class;

                    buf.str("");
                    if (isSpanish)
                    {
                        buf << memberCount << ((memberCount == 1) ? " miembro encontrado:" : " miembros encontrados:");
                    }
                    else
                    {
                        buf << memberCount << " team " << ((memberCount == 1) ? "member" : " members") << " found:";
                    }

                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);

                    do {
                        // populate fields
                        fields = members->Fetch();
                        guid = fields[0].GetUInt32();
                        personalRating = fields[1].GetUInt32();
                        weekWins = fields[2].GetUInt32();
                        weekLosses = fields[3].GetUInt32();
                        seasonWins = fields[4].GetUInt32();
                        seasonLosses = fields[5].GetUInt32();
                        name = fields[6].GetString();
                        race = raceToString(fields[7].GetUInt8());
                        Class = classToString(fields[8].GetUInt8());
                        level = fields[9].GetUInt32();
                        seasonWinPercentage = getWinPercent(seasonWins, seasonLosses);
                        weekWinPercentage = getWinPercent(weekWins, weekLosses);

                        // TODO: add output
                        buf.str(""); // clear it
                        buf << memberPos << ". ";

                        if (isSpanish)
                        {
                            if (guid == captainGuid)
                                buf << "Capitan ";
                            buf << name << ". ";

                            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            buf.str("");
                            buf << race << " " << Class << ", " << personalRating << " rating personal.";
                            AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            buf.str("");
                            buf << "Semana: " << weekWins << "-" << weekLosses << " (" << weekWinPercentage << " ganadas), " << (weekWins + weekLosses) << " jugadas.";
                            AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            buf.str("");
                            buf << "Season: " << seasonWins << "-" << seasonLosses << " (" << seasonWinPercentage << " ganadas), " << (seasonWins + seasonLosses) << " jugadas.";
                            AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            memberPos++;
                        }
                        else
                        {
                            if (guid == captainGuid)
                                buf << "Team Captain ";
                            buf << name << ". ";

                            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            buf.str("");
                            buf << race << " " << Class << ", " << personalRating << " personal rating.";
                            AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            buf.str("");
                            buf << "Week: " << weekWins << "-" << weekLosses << " (" << weekWinPercentage << " win), " << (weekWins + weekLosses) << " played.";
                            AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            buf.str("");
                            buf << "Season: " << seasonWins << "-" << seasonLosses << " (" << seasonWinPercentage << " win), " << (seasonWins + seasonLosses) << " played.";
                            AddGossipItemFor(player, GOSSIP_ICON_DOT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                            memberPos++;
                        }
                    } while (members->NextRow());
                }

                buf.str("");

                if (isSpanish)
                {
                    buf << "Volver a rankings " << type << "v" << type << "!";
                }
                else
                {
                    buf << "Return to " << type << "v" << type << " rankings!";
                }

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, buf.str(), GOSSIP_SENDER_MAIN, parentOption);
                SendGossipMenuFor(player, ARENA_GOSSIP_TEAM_LOOKUP, creature->GetGUID());
            }
        }
        }

        return true;
    }

    struct myAI : public ScriptedAI
    {
        myAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new myAI(creature);
    }
};

void AddSC_arenateamranks()
{
    new ArenaTeamRanks();
}
