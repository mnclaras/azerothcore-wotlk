#ifndef MOD_GUILD_POINTS_H
#define MOD_GUILD_POINTS_H

#include "DBCEnums.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "DBCStores.h"
#include "Language.h"
#include "Player.h"
#include "Group.h"
#include "GroupMgr.h"
#include "InstanceScript.h"
#include "Define.h"
#include "Item.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "WorldSession.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "GossipDef.h"
#include "Creature.h"
#include "ObjectMgr.h"
#include "Guild.h"
#include "GuildMgr.h"

#define DEFAULT_MESSAGE 907

enum ArenaRankActionIds {
    RANK_ACTION_GOODBYE = GOSSIP_ACTION_INFO_DEF + 1,
    RANK_ACTION_NOOP = 1,
};

struct GuildBossRewardInfo
{
    uint32      entry;
    uint32      points;
    uint32      mode;
    std::string difficulty;
};

struct GuildHousePointsInfo
{
    uint32 guildHousePoints;
    //std::vector<uint32> guildHouseAllowedMembers;
};

struct GuildHouseSpawnInfo
{
    uint32 id;
    uint32 parent;
    uint32 entry;
    uint32 points;
    bool isCreature;
    bool isMenu;
    bool isVisible;
    bool isInitialSpawn;
    std::string name;
    uint32 map;
    uint32 guildPosition;
    float posX;
    float posY;
    float posZ;
    float orientation;
};

typedef std::vector<GuildBossRewardInfo*> BossRewardInfoContainer;
typedef std::vector<GuildHouseSpawnInfo*> GuildHouseSpawnInfoContainer;

class sModGuildPoints
{
public:
    static sModGuildPoints* instance()
    {
        static sModGuildPoints* instance = new sModGuildPoints();
        return instance;
    }

    void LoadBossRewardInfo();
    void LoadCreaturesAndObjectsPurchasables();

    GuildHousePointsInfo* GetGuildHousePointsInfo(uint32 guildId);
    std::vector<uint32> GetGuildHousePointsAllowedMembers(uint32 guildId);
    bool MemberHaveGuildHousePointsPermission(Player* player);
    void AddGuildHousePointsAllowedMember(Player* player);
    void RemoveGuildHousePointsAllowedMember(Player* player);
    bool CheckEnoughGuildHousePoints(uint32 guildId, int32 points);
    bool SpendGuildHousePoints(Player* player, uint32 points);
    bool AddGuildHousePoints(Player* player, uint32 points);
    int32 GetGuildHousePoints(uint32 guildId);

    void SaveBossRewardToDB(uint32 entry, uint32 points, uint32 mode, std::string difficulty);
    void PurgeBossReward(uint32 entry, uint32 mode, std::string difficulty);
    void InsertBossReward(uint32 entry, uint32 points, uint32 mode, std::string difficulty);
    void UpdateGuildPoints(uint32 guildId, uint32 points);

    void DeleteGuild(Guild* guild);

    bool IsSpanishPlayer(Player* player);
   
    BossRewardInfoContainer m_BossRewardInfoContainer;
    GuildHouseSpawnInfoContainer m_GuildHouseSpawnInfoContainer;
};
#define sModGuildPointsMgr sModGuildPoints::instance()
#endif

