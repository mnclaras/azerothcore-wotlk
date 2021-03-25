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

enum GuildPointMapId
{
    MAP_ICC = 631,
    MAP_RS = 724,
    MAP_ULDUAR = 603,
    MAP_TOC = 649
};

struct GuildBossRewardInfo
{
    uint32     entry;
    uint32     points;
//public:
//    uint32 getEntry() const { return entry; }
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
    void SaveBossRewardToDB(uint32 entry, uint32 points);
    void PurgeBossReward(uint32 entry);
    void InsertBossReward(uint32 entry, uint32 points);
    void UpdateGuildPoints(uint32 guildId, uint32 points);
   
    BossRewardInfoContainer m_BossRewardInfoContainer;
};
#define sModGuildPointsMgr sModGuildPoints::instance()
#endif

