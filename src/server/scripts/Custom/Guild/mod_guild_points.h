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

typedef std::vector<GuildBossRewardInfo*> BossRewardInfoContainer;

class sModGuildPoints
{
public:
    static sModGuildPoints* instance()
    {
        static sModGuildPoints* instance = new sModGuildPoints();
        return instance;
    }

    void LoadBossRewardInfo();
    void SaveBossRewardToDB(uint32 entry, uint32 points, uint32 mode, std::string difficulty);
    void PurgeBossReward(uint32 entry, uint32 mode, std::string difficulty);
    void InsertBossReward(uint32 entry, uint32 points, uint32 mode, std::string difficulty);
    void UpdateGuildPoints(uint32 guildId, uint32 points);
   
    BossRewardInfoContainer m_BossRewardInfoContainer;
};
#define sModGuildPointsMgr sModGuildPoints::instance()
#endif

