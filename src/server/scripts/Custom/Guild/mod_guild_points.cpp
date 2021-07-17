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

bool sModGuildPoints::IsSpanishPlayer(Player* player)
{
    LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
    return (locale == LOCALE_esES || locale == LOCALE_esMX);
}

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
    CharacterDatabase.PExecute("DELETE FROM guild_points_boss_reward WHERE entry = '%u' AND mode = '%u' AND difficulty = '%s';", entry, mode, difficulty.c_str());
}

void sModGuildPoints::InsertBossReward(uint32 entry, uint32 points, uint32 mode, std::string difficulty)
{
    CharacterDatabase.PExecute("INSERT INTO guild_points_boss_reward (entry, points, mode, difficulty) VALUES ('%u', '%u', '%u', '%s');", entry, points, mode, difficulty.c_str());
}

void sModGuildPoints::UpdateGuildPoints(uint32 guildId, uint32 points)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT seasonPoints, guildHousePoints, totalPoints FROM guild_points_ranking WHERE guildId = '%u';", guildId);
    if (!result)
    {
        CharacterDatabase.PExecute("INSERT INTO guild_points_ranking (guildId, seasonPoints, guildHousePoints, totalPoints) VALUES ('%u','%u','%u','%u');", guildId, points, points, points);
    }
    else
    {
        uint32 currentSeasonPoints = (*result)[0].GetUInt32();
        uint32 currentGuildHousePoints = (*result)[1].GetUInt32();
        uint32 totalPoints = (*result)[2].GetUInt32();
        CharacterDatabase.PExecute("UPDATE guild_points_ranking SET seasonPoints = '%u', guildHousePoints = '%u', totalPoints = '%u' WHERE guildId = '%u';",
            currentSeasonPoints + points, currentGuildHousePoints + points, totalPoints + points, guildId);
    }
}

void sModGuildPoints::SaveBossRewardToDB(uint32 entry, uint32 points, uint32 mode, std::string difficulty)
{
    // Purge current points
    PurgeBossReward(entry, mode, difficulty);
    // Insert reward points
    InsertBossReward(entry, points, mode, difficulty);
}




void sModGuildPoints::LoadCreaturesAndObjectsPurchasables()
{
    for (GuildHouseSpawnInfoContainer::const_iterator itr = m_GuildHouseSpawnInfoContainer.begin(); itr != m_GuildHouseSpawnInfoContainer.end(); ++itr)
        delete* itr;

    for (GuildHouseSpawnLinkInfoContainer::const_iterator itr = m_GuildHouseSpawnLinkInfoContainer.begin(); itr != m_GuildHouseSpawnLinkInfoContainer.end(); ++itr)
        delete* itr;

    m_GuildHouseSpawnInfoContainer.clear();
    m_GuildHouseSpawnLinkInfoContainer.clear();

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    // Spawns

    QueryResult result = WorldDatabase.Query("SELECT id, entry, name, parent, points, map, guild_position, is_creature, is_menu, "
        "is_visible, is_initial_spawn, posX, posY, posZ, orientation FROM guild_house_spawns ORDER BY item_order ASC, id ASC;");

    if (!result)
    {
        sLog->outString(">>MOD GUILD POINTS: Loaded 0 guild house spawns. DB table `guild_house_spawns` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        GuildHouseSpawnInfo* pGuildHouseSpawn = new GuildHouseSpawnInfo;

        pGuildHouseSpawn->id = fields[0].GetUInt32();
        pGuildHouseSpawn->entry = fields[1].GetUInt32();
        pGuildHouseSpawn->name = fields[2].GetString();
        pGuildHouseSpawn->parent = fields[3].GetUInt32();
        pGuildHouseSpawn->points = fields[4].GetUInt32();
        pGuildHouseSpawn->map = fields[5].GetUInt32();
        pGuildHouseSpawn->guildPosition = fields[6].GetUInt32();
        pGuildHouseSpawn->isCreature = fields[7].GetBool();
        pGuildHouseSpawn->isMenu = fields[8].GetBool();
        pGuildHouseSpawn->isVisible = fields[9].GetBool();
        pGuildHouseSpawn->isInitialSpawn = fields[10].GetBool();
        pGuildHouseSpawn->posX = fields[11].GetFloat();
        pGuildHouseSpawn->posY = fields[12].GetFloat();
        pGuildHouseSpawn->posZ = fields[13].GetFloat();
        pGuildHouseSpawn->orientation = fields[14].GetFloat();

        m_GuildHouseSpawnInfoContainer.push_back(pGuildHouseSpawn);
        ++count;
    } while (result->NextRow());
    sLog->outString(">>MOD GUILD POINTS: Loaded %u guild house spawns in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));

    // Spawn links

    oldMSTime = getMSTime();
    count = 0;

    result = WorldDatabase.Query("SELECT id, entry, spawn, is_creature, map, posX, posY, posZ, orientation FROM guild_house_spawn_linked;");

    if (!result)
    {
        sLog->outString(">>MOD GUILD POINTS: Loaded 0 guild house linked spawns. DB table `guild_house_spawn_linked` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        GuildHouseSpawnLinkInfo* pGuildHouseSpawnLink = new GuildHouseSpawnLinkInfo;

        pGuildHouseSpawnLink->id = fields[0].GetUInt32();
        pGuildHouseSpawnLink->entry = fields[1].GetUInt32();
        pGuildHouseSpawnLink->spawn = fields[2].GetUInt32();
        pGuildHouseSpawnLink->isCreature = fields[3].GetUInt32();
        pGuildHouseSpawnLink->map = fields[4].GetUInt32();
        pGuildHouseSpawnLink->posX = fields[5].GetFloat();
        pGuildHouseSpawnLink->posY = fields[6].GetFloat();
        pGuildHouseSpawnLink->posZ = fields[7].GetFloat();
        pGuildHouseSpawnLink->orientation = fields[8].GetFloat();

        m_GuildHouseSpawnLinkInfoContainer.push_back(pGuildHouseSpawnLink);
        ++count;
    } while (result->NextRow());
    sLog->outString(">>MOD GUILD POINTS: Loaded %u guild house linked spawns in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

GuildHousePointsInfo* sModGuildPoints::GetGuildHousePointsInfo(uint32 guildId)
{
    GuildHousePointsInfo* guildHousePointsInfo = new GuildHousePointsInfo;

    QueryResult result = CharacterDatabase.PQuery("SELECT guildHousePoints FROM guild_points_ranking WHERE guildId = '%u';", guildId);

    if (!result)
    {
        return nullptr;
    }

    guildHousePointsInfo->guildHousePoints = (*result)[0].GetUInt32();
    //guildHousePointsInfo->guildHouseAllowedMembers = GetGuildHousePointsAllowedMembers(guildId);

    return guildHousePointsInfo;
}

std::vector<uint32> sModGuildPoints::GetGuildHousePointsAllowedMembers(uint32 guildId)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT member FROM guild_points_allowed_members WHERE guildId = '%u' and member = '%u';", guildId);

    std::vector<uint32> allowedMembers;

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 member = fields[0].GetUInt32();
            if (member && member > 0)
            {
                allowedMembers.push_back(member);
            }
        } while (result->NextRow());
    }

    return allowedMembers;
}

bool sModGuildPoints::MemberHaveGuildHousePointsPermission(Player* player)
{
    if (!player)
        return false;

    if (Guild* guild = player->GetGuild())
    {
        if (guild->GetLeaderGUID() == player->GetGUID())
            return true;

        QueryResult result = CharacterDatabase.PQuery("SELECT member FROM guild_points_allowed_members WHERE guildId = '%u' and member = '%u';",
            player->GetGuildId(), player->GetGUID());
        if (result)
        {
            return (*result)[0].GetUInt32() > 0;
        }
    }

    return false;
}

void sModGuildPoints::AddGuildHousePointsAllowedMember(Player* player)
{
    RemoveGuildHousePointsAllowedMember(player);
    CharacterDatabase.PExecute("INSERT INTO guild_points_allowed_members (guildId, member) VALUES ('%u', '%u');", player->GetGuildId(), player->GetGUID());
}

void sModGuildPoints::RemoveGuildHousePointsAllowedMember(Player* player)
{
    CharacterDatabase.PExecute("DELETE FROM guild_points_allowed_members WHERE guildId = '%u' AND member = '%u';", player->GetGuildId(), player->GetGUID());
}

int32 sModGuildPoints::GetGuildHousePoints(uint32 guildId)
{
    if (!guildId || guildId < 1)
        return -1;

    QueryResult result = CharacterDatabase.PQuery("SELECT guildHousePoints FROM guild_points_ranking WHERE guildId = '%u';", guildId);
    if (!result)
    {
        return -1;
    }
    else
    {
        return (*result)[0].GetInt32();
    }
}

bool sModGuildPoints::CheckEnoughGuildHousePoints(uint32 guildId, int32 points)
{
    return GetGuildHousePoints(guildId) >= points;
}

void sModGuildPoints::SpendGuildHousePoints(Player* player, uint32 points)
{
    if (!player)
        return;

    if (!points)
        points = 0;

    int32 guildHousePoints = GetGuildHousePoints(player->GetGuildId());
    CharacterDatabase.PExecute("UPDATE guild_points_ranking SET guildHousePoints = '%u' WHERE guildId = '%u';",
        guildHousePoints - points, player->GetGuildId());
}

bool sModGuildPoints::CheckCanSpendGuildHousePoints(Player* player, uint32 points)
{
    if (!player)
        return false;

    if (!points)
        points = 0;

    if (!MemberHaveGuildHousePointsPermission(player))
    {
        ChatHandler(player->GetSession()).PSendSysMessage(
            IsSpanishPlayer(player) ? "No tienes permiso para utilizar los puntos de hermandad!" : "You don\'t have permission to use guild points.");
        return false;
    }

    int32 guildHousePoints = GetGuildHousePoints(player->GetGuildId());
    if (guildHousePoints < (int32)points)
    {
        if (guildHousePoints < 0) guildHousePoints = 0;
        ChatHandler(player->GetSession()).PSendSysMessage(
            IsSpanishPlayer(player) ? "No tienes suficientes puntos de hermandad para comprar eso. Puntos actuales: %u. Requeridos: %u"
            : "You don\'t have enough guild points to buy that. Current Points: %u. Required: %u.", guildHousePoints, points);
        return false;
    }

    return true;
}

bool sModGuildPoints::AddGuildHousePoints(Player* player, uint32 points)
{
    if (!player || points <= 0)
        return false;

    int32 guildHousePoints = GetGuildHousePoints(player->GetGuildId());
    CharacterDatabase.PExecute("UPDATE guild_points_ranking SET guildHousePoints = '%u' WHERE guildId = '%u';",
        guildHousePoints + points, player->GetGuildId());

    return true;
}

void sModGuildPoints::DeleteGuild(uint32 guildId, bool removeRanking)
{
    if (removeRanking)
    {
        CharacterDatabase.PExecute("DELETE FROM guild_points_ranking WHERE guildId = %u;", guildId);
    }
    // Delete actual guild_house data from characters database
    CharacterDatabase.PExecute("DELETE FROM `guild_house` WHERE `guild` = '%u'", guildId);
    CharacterDatabase.PExecute("DELETE FROM `guild_house_purchased_spawns` WHERE `guild` = '%u'", guildId);

    if (!removeRanking)
    {
        uint32 pointsToReturn = 2500;
        QueryResult result = CharacterDatabase.PQuery("SELECT DISTINCT GROUP_CONCAT(spawn) FROM guild_house_purchased_spawns WHERE guild = '%u';", guildId);
        if (result)
        {
            std::string purchasedSpawns = (*result)[0].GetString();

            if (!purchasedSpawns.empty())
            {
                QueryResult purchasedPointsResult = WorldDatabase.PQuery("SELECT SUM(points) FROM guild_house_spawns WHERE id IN (%s);", purchasedSpawns.c_str());

                if (purchasedPointsResult)
                {
                    uint32 purchasedPoints = (*purchasedPointsResult)[0].GetUInt32();
                    if (purchasedPoints && purchasedPoints > 0)
                    {
                        pointsToReturn += (uint32)(purchasedPoints/2);
                    }
                }
            }
        }

        // Return 5000/2 (Guild House Cost) + purchased boughts/2
        int32 guildHousePoints = GetGuildHousePoints(guildId);
        CharacterDatabase.PExecute("UPDATE guild_points_ranking SET guildHousePoints = '%u' WHERE guildId = '%u';",
            guildHousePoints + pointsToReturn, guildId);
    }
}

uint32 sModGuildPoints::GetGuildPosition(Player* player)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT `guild_position` FROM guild_house WHERE `guild` = %u", player->GetGuildId());
    if (!result)
    {
        return 0;
    }
    else
    {
        return (*result)[0].GetUInt32();
    }
}

bool sModGuildPoints::CheckSpawnAlreadyPurchased(Player* player, uint32 spawn)
{
    if (!spawn) return false;

    QueryResult result = CharacterDatabase.PQuery("SELECT id FROM guild_house_purchased_spawns WHERE guild = '%u' AND spawn = '%u';", player->GetGuildId(), spawn);
    if (result)
    {
        return true;
    }
    return false;
}

void sModGuildPoints::AddPurchasedSpawn(Player* player, uint32 spawn)
{
    if (spawn && !CheckSpawnAlreadyPurchased(player, spawn))
    {
        CharacterDatabase.PExecute("INSERT INTO guild_house_purchased_spawns (guild, spawn) VALUES ('%u', '%u');", player->GetGuildId(), spawn);
    }
}

void sModGuildPoints::DeleteAllPurchasedSpawns(uint32 spawn)
{
    if (spawn)
    {
        CharacterDatabase.PExecute("DELETE FROM guild_house_purchased_spawns WHERE spawn = '%u';", spawn);
    }
}

uint32 sModGuildPoints::GetSpawnParent(uint32 spawn)
{
    QueryResult result = WorldDatabase.PQuery("SELECT parent FROM guild_house_spawns WHERE id = '%u';", spawn);
    if (result)
    {
        return (*result)[0].GetUInt32();
    }
    else
    {
        return 0;
    }
}

class mod_guild_points : public PlayerScript
{
public:
    mod_guild_points() : PlayerScript("mod_guild_points") { }

    void OnCreatureKill(Player* player, Creature* boss) override
    {
        if (!player)
            return;

        if (!boss)
            return;

        uint32 mode = 1; // World
        std::string difficulty = "N";
        if (boss->GetMap()->IsDungeon())
        {
            difficulty = (player->GetMap()->IsHeroic()) ? "H" : "N";
            if (player->GetMap()->IsNonRaidDungeon())
                mode = 5; // Instances
            else if (player->GetMap()->Is25ManRaid())
                mode = 25; // Raid 25
            else
                mode = 10; // Raid 10
        }

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
        if (leader && AccountMgr::IsPlayerAccount(leader->GetSession()->GetSecurity()))
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

        QueryResult result = CharacterDatabase.Query("SELECT guildId, seasonPoints FROM guild_points_ranking ORDER BY seasonPoints DESC LIMIT 30;");
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

        if (mode != 1 && mode != 5 && mode != 10 && mode != 25)
        {
            handler->SendGlobalGMSysMessage("Mode should be 1 (World) 5 (Dungeons), 10 or 25 (Raids).");
        }
        else if (difficulty != "N" && difficulty != "H")
        {
            handler->SendGlobalGMSysMessage("Difficulty should be N or H. Use N for World.");
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
        //sLog->outString("Loading guild points...");
        //sModGuildPointsMgr->LoadGuildPointsInfo();

        handler->SendGlobalGMSysMessage("Boss rewards reloaded.");
        return true;
    }
};

class ModGuildHousePoints_Command : public CommandScript
{
public:
    ModGuildHousePoints_Command() : CommandScript("ModGuildHousePoints_Command") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> GuildPointsTable = {
            {"addmember", SEC_PLAYER, true, &HandleAddGuildHouseAllowedMemberCommand, "Grants a player privileges to spend guild points. Example: `.guildhouse addmember`"},
            {"removemember", SEC_PLAYER, true, &HandleRemoveGuildHouseAllowedMemberCommand, "Remove the privileges to spend guild points on a guild player. Example: `.guildhouse removemember`"},
            {"reloadspawns", SEC_ADMINISTRATOR, true, &HandleReloadGuildHouseAvailableSpawnsCommand, "Reload all available GObjects and creatures to spawn on the guild house. Example: `.guildhouse reloadspawns`"},
            {"addspawn", SEC_ADMINISTRATOR, true, &HandleAddGuildHouseAvailableSpawnCommand, "Adds a spawn to DB in the player position. Example: `.guildhouse addspawn #ENTRY #POINTS #PARENT #ISMENU #ISCREATURE #ISINITIALSPAWN #GUILDPOSITION #ISLINKED #NAME`"},
            {"addlinkedspawn", SEC_ADMINISTRATOR, true, &HandleAddGuildHouseLinkedSpawnCommand, "Adds a linked spawn to DB in the player position. Example: `.guildhouse addlinkedspawn #ENTRY #SPAWN #ISCREATURE`"},
        };

        static std::vector<ChatCommand> commandTable = {
            {"guildhouse", SEC_PLAYER, true, nullptr, "", GuildPointsTable},
        };

        return commandTable;
    }

    static bool HandleAddGuildHouseAllowedMemberCommand(ChatHandler* handler, const char* args)
    {
        Player* target = nullptr;

        if (args && strlen(args) > 0)
        {
            target = ObjectAccessor::FindPlayerByName(args);
            if (!target)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!target)
        {
            if (!handler->extractPlayerTarget((char*)args, &target))
            {
                handler->SendSysMessage(sModGuildPointsMgr->IsSpanishPlayer(player) ?
                    "Debes escribir un nombre de personaje existente, o seleccionar a un jugador para poder utilizar este comando!"
                    : "You must type the name of a valid character, or select a player in order to use this command!");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        if (target && player)
        {
            if (player->GetGuild() && target->GetGuild() && player->GetGuildId() == target->GetGuildId() && player->GetGuild()->GetLeaderGUID() == player->GetGUID())
            {
                sModGuildPointsMgr->AddGuildHousePointsAllowedMember(target);
                handler->PSendSysMessage(sModGuildPointsMgr->IsSpanishPlayer(player) ? "Privilegios ACTIVADOS. El jugador: %s PUEDE hacer uso de los puntos de hermandad."
                    : "Privileges ENABLED. Player: %s CAN make use of the guild points.", target->GetName().c_str());
            }
            else
            {
                handler->SendSysMessage(sModGuildPointsMgr->IsSpanishPlayer(player) ?
                    "Debes ser el lider de tu hermandad para utilizar este comando, y el jugador debe pertenecer a tu hermandad."
                    : "You must be the Guild Master of a guild to use this command!");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            handler->SendSysMessage(sModGuildPointsMgr->IsSpanishPlayer(player) ?
                "Debes escribir un nombre de personaje existente, o seleccionar a un jugador para poder utilizar este comando!"
                : "You must type the name of a valid character, or select a player in order to use this command!");
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleRemoveGuildHouseAllowedMemberCommand(ChatHandler* handler, const char* args)
    {
        Player* target = nullptr;

        if (args && strlen(args) > 0)
        {
            target = ObjectAccessor::FindPlayerByName(args);
            if (!target)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!target)
        {
            if (!handler->extractPlayerTarget((char*)args, &target))
            {
                handler->SendSysMessage(sModGuildPointsMgr->IsSpanishPlayer(player) ?
                    "Debes escribir un nombre de personaje existente, o seleccionar a un jugador para poder utilizar este comando!"
                    : "You must type the name of a valid character, or select a player in order to use this command!");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        if (target && player)
        {
            if (player->GetGuild() && target->GetGuild() && player->GetGuildId() == target->GetGuildId() && player->GetGuild()->GetLeaderGUID() == player->GetGUID())
            {
                sModGuildPointsMgr->RemoveGuildHousePointsAllowedMember(target);
                handler->PSendSysMessage(sModGuildPointsMgr->IsSpanishPlayer(player) ? "Privilegios DESACTIVADOS. El jugador: %s NO PUEDE hacer uso de los puntos de hermandad."
                    : "Privileges DISABLED. Player: %s CANNOT use guild points.", target->GetName().c_str());
            }
            else
            {
                handler->SendSysMessage(sModGuildPointsMgr->IsSpanishPlayer(player) ?
                    "Debes ser el lider de tu hermandad para utilizar este comando, y el jugador debe pertenecer a tu hermandad."
                    : "You must be the Guild Master of a guild to use this command!");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            handler->SendSysMessage(sModGuildPointsMgr->IsSpanishPlayer(player) ?
                "Debes escribir un nombre de personaje existente, o seleccionar a un jugador para poder utilizar este comando!"
                : "You must type the name of a valid character, or select a player in order to use this command!");
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleAddGuildHouseAvailableSpawnCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // Mandatory params

        char* entryStr = strtok((char*)args, " ");
        if (!entryStr) return false;
        if (atoi(entryStr) < 0) return false;

        char* pointsStr = strtok(nullptr, " ");
        if (!pointsStr) return false;
        if (atoi(pointsStr) < 0) return false;

        char* parentStr = strtok(nullptr, " ");
        if (!parentStr) return false;
        if (atoi(parentStr) < 0) return false;

        char* isMenuStr = strtok(nullptr, " ");
        if (!isMenuStr) return false;
        if (atoi(isMenuStr) != 0 && atoi(isMenuStr) != 1) return false;

        char* isCreatureStr = strtok(nullptr, " ");
        if (!isCreatureStr) return false;
        if (atoi(isCreatureStr) != 0 && atoi(isCreatureStr) != 1) return false;

        char* isInitialSpawnStr = strtok(nullptr, " ");
        if (!isInitialSpawnStr) return false;
        if (atoi(isInitialSpawnStr) != 0 && atoi(isInitialSpawnStr) != 1) return false;

        char* guildPositionStr = strtok(nullptr, " ");
        if (!guildPositionStr) return false;
        if (atoi(guildPositionStr) < 0) return false;

        char* isLinkedStr = strtok(nullptr, " ");
        if (!isLinkedStr) return false;
        if (atoi(isLinkedStr) != 0 && atoi(isLinkedStr) != 1) return false;

        char* nameStr = strtok(nullptr, "");
        if (!nameStr) return false;
        char* textExtracted = handler->extractQuotedArg(nameStr);
        if (!textExtracted) return false;
        std::string name = textExtracted;
        if (name.empty()) return false;

        Player* player = handler->GetSession()->GetPlayer();

        uint32 entry = atoi(entryStr);
        uint32 points = atoi(pointsStr);
        uint32 parent = atoi(parentStr);
        uint32 isMenu = atoi(isMenuStr);
        uint32 isCreature = atoi(isCreatureStr);
        uint32 isVisible = 1;
        uint32 isInitialSpawn = isInitialSpawnStr ? atoi(isInitialSpawnStr) : 0;
        uint32 guildPosition = guildPositionStr ? atoi(guildPositionStr) : 0;
        uint32 isLinked = isLinkedStr ? atoi(isLinkedStr) : 0;

        WorldDatabase.EscapeString(name);

        uint32 mapId = player->GetMapId();
        float posX = player->GetPositionX();
        float posY = player->GetPositionY();
        float posZ = player->GetPositionZ();
        float ori = player->GetOrientation();

        bool checkTemplate = true;
        if (isLinked)
        {
            mapId = 0;
            posX = 0.0f;
            posY = 0.0f;
            posZ = 0.0f;
            ori = 0.0f;
            entry = 0;
            isMenu = 0;
            guildPosition = 0;
            isInitialSpawn = false;
            isCreature = false;
            checkTemplate = false;
        }

        if (isMenu)
        {
            mapId = 0;
            posX = 0.0f;
            posY = 0.0f;
            posZ = 0.0f;
            ori = 0.0f;
            entry = 0;
            guildPosition = 0;
            isInitialSpawn = false;
            isCreature = false;
            checkTemplate = false;
        }

        if (checkTemplate)
        {
            if (isCreature)
            {
                if (!sObjectMgr->GetCreatureTemplate(entry))
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("NPC couldn't be added. (INVALID ENTRY - CREATURE_TEMPLATE)");
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }
            else
            {
                uint32 objectId = entry;
                const GameObjectTemplate* objectInfo = sObjectMgr->GetGameObjectTemplate(objectId);

                if (!objectInfo)
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("Object couldn't be added. (INVALID ENTRY - GAMEOBJECT_TEMPLATE)");
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                if (objectInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(objectInfo->displayId))
                {
                    ChatHandler(player->GetSession()).PSendSysMessage("Object couldn't be added. (INVALID DISPLAYID - GAMEOBJECT_TEMPLATE)");
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }
        }

        if (isInitialSpawn && guildPosition < 1)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Spawn couldn't be added. (INVALID GUILDPOSITION, SHOULD BE > 0 WITH INITIALSPAWN = 1)");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (guildPosition)
        {
            QueryResult result = CharacterDatabase.PQuery("SELECT `id` FROM guild_house_position WHERE `id` = '%u';", guildPosition);
            if (!result)
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Spawn couldn't be added. (INVALID POSITION - NOT FOUND)");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        if (parent)
        {
            QueryResult result = WorldDatabase.PQuery("SELECT id FROM guild_house_spawns WHERE id = '%u';", parent);

            if (!result)
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Spawn couldn't be added. (INVALID PARENT)");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        QueryResult rowResult = WorldDatabase.PQuery("SELECT id FROM guild_house_spawns WHERE "
            "entry = '%u' AND "
            "parent = '%u' AND "
            "is_menu = '%u' AND "
            "is_creature = '%u' AND "
            "is_visible = '%u' AND "
            "is_initial_spawn = '%u' AND "
            "guild_position = '%u' AND "
            "points = '%u' AND "
            "name = '%s';",
            entry, parent, isMenu, isCreature, isVisible, isInitialSpawn, guildPosition, points, name.c_str());

        if (!rowResult)
        {
            WorldDatabase.PExecute(
                "INSERT INTO guild_house_spawns (entry, parent, is_menu, is_creature, is_visible, is_initial_spawn, guild_position, points, name, map, posX, posY, posZ, orientation) VALUES ('%u','%u','%u','%u','%u','%u','%u','%u','%s','%u','%f','%f','%f','%f');"
                , entry, parent, isMenu, isCreature, isVisible, isInitialSpawn, guildPosition, points, name.c_str(), mapId, posX, posY, posZ, ori);

            QueryResult rowInsertResult = WorldDatabase.PQuery("SELECT id FROM guild_house_spawns WHERE "
                "entry = '%u' AND "
                "parent = '%u' AND "
                "is_menu = '%u' AND "
                "is_creature = '%u' AND "
                "is_visible = '%u' AND "
                "is_initial_spawn = '%u' AND "
                "guild_position = '%u' AND "
                "points = '%u' AND "
                "name = '%s';",
                entry, parent, isMenu, isCreature, isVisible, isInitialSpawn, guildPosition, points, name.c_str());

            if (rowInsertResult)
            {
                uint32 idInsert = (*rowInsertResult)[0].GetUInt32();
                std::stringstream tmp;
                tmp << "Spawn saved to DB. ID: " << idInsert << ". You might want to type \".guildhouse reloadspawns\".";
                handler->SendGlobalGMSysMessage(tmp.str().c_str());
            }
            else
            {
                std::stringstream tmp;
                tmp << "Spawn saved to DB. You might want to type \".guildhouse reloadspawns\".";
                handler->SendGlobalGMSysMessage(tmp.str().c_str());
            }

            // Delete parent purchased spawns of the guild if a new one is added
            uint32 searchParent = parent;
            if (searchParent && searchParent > 0)
            {
                std::stringstream tmp;
                tmp << "Deleting all associated parent purchases for Spawn: " << searchParent;
                handler->SendGlobalGMSysMessage(tmp.str().c_str());
            }

            while (searchParent && searchParent > 0)
            {
                sModGuildPointsMgr->DeleteAllPurchasedSpawns(searchParent);
                uint32 parentSpawn = sModGuildPointsMgr->GetSpawnParent(searchParent);
                searchParent = parentSpawn;          
            }
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Spawn couldn't be added. (ALREADY EXISTS)");
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleAddGuildHouseLinkedSpawnCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        // Mandatory params

        char* entryStr = strtok((char*)args, " ");
        if (!entryStr) return false;
        if (atoi(entryStr) < 2) return false;

        char* spawnStr = strtok(nullptr, " ");
        if (!spawnStr) return false;
        if (atoi(spawnStr) < 1) return false;

        char* isCreatureStr = strtok(nullptr, " ");
        if (!isCreatureStr) return false;
        if (atoi(isCreatureStr) != 0 && atoi(isCreatureStr) != 1) return false;

        Player* player = handler->GetSession()->GetPlayer();

        uint32 entry = atoi(entryStr);
        uint32 spawn = atoi(spawnStr);
        uint32 isCreature = atoi(isCreatureStr);

        uint32 mapId = player->GetMapId();
        float posX = player->GetPositionX();
        float posY = player->GetPositionY();
        float posZ = player->GetPositionZ();
        float ori = player->GetOrientation();

        if (isCreature)
        {
            if (!sObjectMgr->GetCreatureTemplate(entry))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("NPC couldn't be added. (INVALID ENTRY - CREATURE_TEMPLATE)");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            uint32 objectId = entry;
            const GameObjectTemplate* objectInfo = sObjectMgr->GetGameObjectTemplate(objectId);

            if (!objectInfo)
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Object couldn't be added. (INVALID ENTRY - GAMEOBJECT_TEMPLATE)");
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (objectInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(objectInfo->displayId))
            {
                ChatHandler(player->GetSession()).PSendSysMessage("Object couldn't be added. (INVALID DISPLAYID - GAMEOBJECT_TEMPLATE)");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        QueryResult result = WorldDatabase.PQuery("SELECT id FROM guild_house_spawns WHERE id = '%u';", spawn);

        if (!result)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Linked spawn couldn't be added. (INVALID SPAWN)");
            handler->SetSentErrorMessage(true);
            return false;
        }

        WorldDatabase.PExecute(
            "INSERT INTO guild_house_spawn_linked (entry, spawn, is_creature, map, posX, posY, posZ, orientation) VALUES ('%u','%u','%u','%u','%f','%f','%f','%f');"
            , entry, spawn, isCreature, mapId, posX, posY, posZ, ori);

        handler->SendGlobalGMSysMessage("Linked spawn saved to DB. You might want to type \".guildhouse reloadspawns\".");

        return true;
    }

    static bool HandleReloadGuildHouseAvailableSpawnsCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog->outString("Reloading guild house spawns...");
        sModGuildPointsMgr->LoadCreaturesAndObjectsPurchasables();
        handler->SendGlobalGMSysMessage("Guild house spawns reloaded.");
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

        sLog->outString("Loading guild house spawns...");
        sModGuildPointsMgr->LoadCreaturesAndObjectsPurchasables();

        sLog->outString("== MOD GUILD POINTS ===========================================================================");
    }
};

void AddSC_mod_guild_points()
{
    new mod_guild_points();
    new ModGuildPoints_World();
    new ModGuildPoints_Command();
    new ModGuildHousePoints_Command();
    new ModGuildPoints_Ranking();
}
