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
#include "Guild.h"


void LoadGuildPoints()
{
    for (BossRewardInfoContainer::const_iterator itr = sModGuildPointsMgr->m_BossRewardInfoContainer.begin(); itr != sModGuildPointsMgr->m_BossRewardInfoContainer.end(); ++itr)
        delete* itr;

    sModGuildPointsMgr->m_BossRewardInfoContainer.clear();

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    QueryResult result = CharacterDatabase.Query("SELECT entry, points FROM guild_points_boss_reward;");

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

        sModGuildPointsMgr->m_BossRewardInfoContainer.push_back(pBossReward);
        ++count;
    } while (result->NextRow());
    sLog->outString(">>MOD GUILD POINTS: Loaded %u boss rewards in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sModGuildPoints::LoadBossRewardInfo()
{
    for (BossRewardInfoContainer::const_iterator itr = m_BossRewardInfoContainer.begin(); itr != m_BossRewardInfoContainer.end(); ++itr)
        delete* itr;

    m_BossRewardInfoContainer.clear();

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    QueryResult result = CharacterDatabase.Query("SELECT entry, points FROM guild_points_boss_reward;");

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

        m_BossRewardInfoContainer.push_back(pBossReward);
        ++count;
    } while (result->NextRow());
    sLog->outString(">>MOD GUILD POINTS: Loaded %u boss rewards in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sModGuildPoints::PurgeBossReward(uint32 entry)
{
    CharacterDatabase.PQuery("DELETE FROM guild_points_boss_reward WHERE entry = '%u'", entry);
}

void sModGuildPoints::InsertBossReward(uint32 entry, uint32 points)
{
    CharacterDatabase.PExecute("INSERT INTO guild_points_boss_reward (entry, points) VALUES ('%u', '%u');", entry, points);
}

void sModGuildPoints::UpdateGuildPoints(uint32 guildId, uint32 points)
{
    QueryResult result = CharacterDatabase.Query("SELECT points FROM guild_points_ranking;");
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

void sModGuildPoints::SaveBossRewardToDB(uint32 entry, uint32 points)
{
    // Purge current points
    PurgeBossReward(entry);
    // Insert reward points
    InsertBossReward(entry, points);
}

class mod_guild_points : public PlayerScript
{
public:
    mod_guild_points() : PlayerScript("mod_guild_points") { }


    void OnCreatureKill(Player* player, Creature* boss) override
    {
        if (!player)
            return;

        uint32 entryToFind = 123;

        for (BossRewardInfoContainer::const_iterator itr = sModGuildPointsMgr->m_BossRewardInfoContainer.begin(); itr != sModGuildPointsMgr->m_BossRewardInfoContainer.end(); ++itr)
        {
            if (entryToFind == (*itr)->entry)
            {
                uint32 points = (*itr)->points;

                if (points && points > 0 && boss->GetMap()->IsDungeon())
                {
                    Player* leader = GetLeaderOfGroup(player);
                    Guild* guild = leader ? leader->GetGuild() : nullptr;

                    if (leader && guild)
                    {
                        sModGuildPointsMgr->UpdateGuildPoints(leader->GetGuildId(), points);
                        std::ostringstream stream;
                        stream << "La hermandad |CFF00FF00" << leader->GetGuildName() << "|r ha sumado |CFF00FF00[" << std::to_string(points) << "] puntos!";
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
        if (leader && AccountMgr::IsPlayerAccount(leader->GetSession()->GetSecurity()))
        {
            return leader;
        }

        return nullptr;
    }
};

class ModGuildPoints_Command : public CommandScript
{
public:
    ModGuildPoints_Command() : CommandScript("ModGuildPoints_Command") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> GuildPointsTable = {
            {"save", SEC_ADMINISTRATOR, false, &HandleSaveBossRewards, "Add/Update a creature <entry> that will give <points> to the guild on kill. Example: `.gpoints add <entry> <points>`"},
            {"remove", SEC_ADMINISTRATOR, false, &HandleRemoveBossRewards, "Remove a creature <entry> that will no loger give points to the guild on kill. Example: `.gpoints remove <entry>`"},
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

        uint32 entry = atoi(entryStr);
        uint32 points = atoi(pointsStr);

        sModGuildPointsMgr->SaveBossRewardToDB(entry, points);

        handler->SendGlobalGMSysMessage("Boss reward saved to DB. You might want to type \".gpoints reload\".");

        return true;
    }

    static bool HandleRemoveBossRewards(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        char* entryStr = strtok((char*)args, " ");
        if (!entryStr || !atoi(entryStr))
            return false;

        uint32 entry = atoi(entryStr);

        sModGuildPointsMgr->PurgeBossReward(entry);

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
}
