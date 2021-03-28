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

#include "mod_guild_points.h"
#include "Group.h"
#include "GroupMgr.h"


void sModGuildPoints::LoadBossRewardInfo()
{
    for (BossRewardInfoContainer::const_iterator itr = m_BossRewardInfoContainer.begin(); itr != m_BossRewardInfoContainer.end(); ++itr)
        delete* itr;

    m_BossRewardInfoContainer.clear();

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    QueryResult result = CharacterDatabase.Query("SELECT entry, points, mode, difficulty FROM guild_points_boss_reward;");

    if (!result)
    {
        sLog->outString(">>MOD GUILD POINTS: Loaded 0 boss rewards. DB table `guild_points_boss_reward` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        GuildBossRewardInfo* pBossReward = new GuildBossRewardInfo;

        pBossReward->entry = fields[0].GetUInt32();
        pBossReward->points = fields[1].GetUInt32();
        pBossReward->mode = fields[2].GetUInt32();
        pBossReward->difficulty = fields[3].GetString();

        m_BossRewardInfoContainer.push_back(pBossReward);
        ++count;
    } while (result->NextRow());
    sLog->outString(">>MOD GUILD POINTS: Loaded %u boss rewards in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sModGuildPoints::PurgeBossReward(uint32 entry, uint32 mode, std::string difficulty)
{
    CharacterDatabase.PExecute("DELETE FROM guild_points_boss_reward WHERE entry = '%u' AND mode = '%u' AND difficulty = '%s'", entry, mode, difficulty.c_str());
}

void sModGuildPoints::InsertBossReward(uint32 entry, uint32 points, uint32 mode, std::string difficulty)
{
    CharacterDatabase.PExecute("INSERT INTO guild_points_boss_reward (entry, points, mode, difficulty) VALUES ('%u', '%u', '%u', '%s');", entry, points, mode, difficulty.c_str());
}

void sModGuildPoints::UpdateGuildPoints(uint32 guildId, uint32 points)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT points FROM guild_points_ranking WHERE guildId = '%u';", guildId);
    if (!result)
    {
        CharacterDatabase.PExecute("INSERT INTO guild_points_ranking (guildId, points) VALUES ('%u', '%u');", guildId, points);
    }
    else
    {
        uint32 currentPoints = (*result)[0].GetUInt32();
        CharacterDatabase.PExecute("UPDATE guild_points_ranking SET points = '%u' WHERE guildId = '%u';", currentPoints + points, guildId);
    }
}

void sModGuildPoints::SaveBossRewardToDB(uint32 entry, uint32 points, uint32 mode, std::string difficulty)
{
    // Purge current points
    PurgeBossReward(entry, mode, difficulty);
    // Insert reward points
    InsertBossReward(entry, points, mode, difficulty);
}

class mod_guild_points : public PlayerScript
{
public:
    mod_guild_points() : PlayerScript("mod_guild_points") { }

    void OnCreatureKill(Player* player, Creature* boss) override
    {
        if (!player)
            return;

        if (!boss || !boss->GetMap()->IsDungeon())
            return;

        uint32 mode = 10;
        if (player->GetMap()->IsNonRaidDungeon())
            mode = 5;
        else if (player->GetMap()->Is25ManRaid())
            mode = 25;

        std::string difficulty = (player->GetMap()->IsHeroic()) ? "H" : "N";

        uint32 bossEntry = boss->GetEntry();
        for (BossRewardInfoContainer::const_iterator itr = sModGuildPointsMgr->m_BossRewardInfoContainer.begin(); itr != sModGuildPointsMgr->m_BossRewardInfoContainer.end(); ++itr)
        {
            if (bossEntry == (*itr)->entry && mode == (*itr)->mode && difficulty == (*itr)->difficulty)
            {
                uint32 points = (*itr)->points;

                if (points && points > 0)
                {
                    Player* leader = GetLeaderOfGroup(player);
                    Guild* guild = leader ? leader->GetGuild() : nullptr;

                    if (leader && guild)
                    {
                        sModGuildPointsMgr->UpdateGuildPoints(leader->GetGuildId(), points);
                        std::ostringstream stream;
                        stream << "La hermandad |CFF00FF00" << leader->GetGuildName() << "|r ha sumado |CFF00FF00[" << std::to_string(points) << "]|r puntos!";
                        sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
                        break;
                    }
                }
            }
        }
    }

    Player* GetLeaderOfGroup(Player* player)
    {
        Player* leader = player;
        uint64 leaderGuid = player->GetGroup() ? player->GetGroup()->GetLeaderGUID() : player->GetGUID();

        if (leaderGuid != player->GetGUID() && player->GetGroup())
            leader = ObjectAccessor::FindPlayerInOrOutOfWorld(player->GetGroup()->GetLeaderGUID());

        if (!leader) leader = player;
        if (leader /*&& AccountMgr::IsPlayerAccount(leader->GetSession()->GetSecurity())*/)
        {
            return leader;
        }

        return nullptr;
    }
};

class ModGuildPoints_Ranking : public CreatureScript
{
private:
    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

public:

    ModGuildPoints_Ranking() : CreatureScript("ModGuildPoints_Ranking") { }

    bool OnGossipHello(Player* player, Creature* creature)  override
    {
        bool isSpanish = IsSpanishPlayer(player);

        QueryResult result = CharacterDatabase.Query("SELECT guildId, points FROM guild_points_ranking ORDER BY points DESC LIMIT 30;");
        if (!result)
        {
            ClearGossipMenuFor(player);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Adios" : "Goodbye", GOSSIP_SENDER_MAIN, RANK_ACTION_GOODBYE);
        }
        else
        {
            std::string name;
            uint32 guildId, points, position = 1;
            std::ostringstream stream;

            do {
                Field* fields = result->Fetch();
                guildId = fields[0].GetUInt32();
                points = fields[1].GetUInt32();


                if (guildId && points && guildId > 0 && points > 0)
                {
                    if (Guild* guild = sGuildMgr->GetGuildById(guildId))
                    {
                        stream.str("");
                        stream << std::to_string(position) << ". " << "|CFF0B5C39" << guild->GetName() << "|r: " << std::to_string(points);
                        if (isSpanish) stream << " puntos.";
                        else stream << " points.";

                        position++;

                        AddGossipItemFor(player, GOSSIP_ICON_DOT, stream.str(), GOSSIP_SENDER_MAIN, RANK_ACTION_NOOP);
                    }
                }  
            } while (result->NextRow());

        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Adios" : "Goodbye", GOSSIP_SENDER_MAIN, RANK_ACTION_GOODBYE);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action) {
        case 0:
        case RANK_ACTION_NOOP:
            OnGossipHello(player, creature);
            break;
        case RANK_ACTION_GOODBYE:
            CloseGossipMenuFor(player);
            break;
        default:
            OnGossipHello(player, creature);
            break;
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

class ModGuildPoints_Command : public CommandScript
{
public:
    ModGuildPoints_Command() : CommandScript("ModGuildPoints_Command") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> GuildPointsTable = {
            {"save", SEC_ADMINISTRATOR, false, &HandleSaveBossRewards, "Add/Update a creature <entry> that will give <points> to the guild on kill. Example: `.gpoints add <entry> <points> <mode> <difficulty>`"},
            {"remove", SEC_ADMINISTRATOR, false, &HandleRemoveBossRewards, "Remove a creature <entry> that will no loger give points to the guild on kill. Example: `.gpoints remove <entry> <mode> <difficulty>`"},
            {"reload", SEC_ADMINISTRATOR, true, &HandleReloadBossRewardsCommand, "Reload all creature rewards. Example: `.gpoints reload`"},
        };

        static std::vector<ChatCommand> commandTable = {
            {"gpoints", SEC_ADMINISTRATOR, true, nullptr, "", GuildPointsTable},
        };

        return commandTable;
    }

    static bool HandleSaveBossRewards(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* entryStr = strtok((char*)args, " ");
        if (!entryStr || !atoi(entryStr))
            return false;

        char* pointsStr = strtok(nullptr, " ");
        if (!pointsStr || !atoi(pointsStr))
            return false;

        char* modeStr = strtok(nullptr, " ");
        if (!modeStr || !atoi(modeStr))
            return false;

        char* difficultyStr = strtok(nullptr, " ");
        std::string difficulty = difficultyStr ? difficultyStr : "";
        if (!difficultyStr || difficulty.empty())
            return false;

        std::transform(difficulty.begin(), difficulty.end(), difficulty.begin(), ::toupper);

        uint32 entry = atoi(entryStr);
        uint32 points = atoi(pointsStr);
        uint32 mode = atoi(modeStr);

        if (mode != 5 && mode != 10 && mode != 25)
        {
            handler->SendGlobalGMSysMessage("Mode should be 5, 10 or 25.");
        }
        else if (difficulty != "N" && difficulty != "H")
        {
            handler->SendGlobalGMSysMessage("Difficulty should be N or H.");
        }
        else
        {
            sModGuildPointsMgr->SaveBossRewardToDB(entry, points, mode, difficulty);
            handler->SendGlobalGMSysMessage("Boss reward saved to DB. You might want to type \".gpoints reload\".");
        }

        return true;
    }

    static bool HandleRemoveBossRewards(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* entryStr = strtok((char*)args, " ");
        if (!entryStr || !atoi(entryStr))
            return false;

        char* modeStr = strtok(nullptr, " ");
        if (!modeStr || !atoi(modeStr))
            return false;

        char* difficultyStr = strtok(nullptr, " ");
        std::string difficulty = difficultyStr ? difficultyStr : "";
        if (!difficultyStr || difficulty.empty())
            return false;

        uint32 entry = atoi(entryStr);
        uint32 mode = atoi(modeStr);

        sModGuildPointsMgr->PurgeBossReward(entry, mode, difficulty);

        handler->SendGlobalGMSysMessage("Boss reward removed from DB. You might want to type \".gpoints reload\".");

        return true;
    }

    static bool HandleReloadBossRewardsCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog->outString("Reloading boss rewards...");
        sModGuildPointsMgr->LoadBossRewardInfo();
        handler->SendGlobalGMSysMessage("Boss rewards reloaded.");
        return true;
    }
};

class ModGuildPoints_World : public WorldScript
{
public:
    ModGuildPoints_World() : WorldScript("ModGuildPoints_World") { }

    void OnStartup() override
    {
        // Load templates for Template NPC #1
        sLog->outString("== MOD GUILD POINTS ===========================================================================");

        sLog->outString("Loading boss rewards...");
        sModGuildPointsMgr->LoadBossRewardInfo();

        sLog->outString("== MOD GUILD POINTS ===========================================================================");
    }
};

void AddSC_mod_guild_points()
{
    new mod_guild_points();
    new ModGuildPoints_World();
    new ModGuildPoints_Command();
    new ModGuildPoints_Ranking();
}
