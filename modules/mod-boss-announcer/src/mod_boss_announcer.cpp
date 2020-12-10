//by SymbolixDEV
//Reworked by Talamortis
#include "ScriptMgr.h"
#include "Config.h"
#include <Player.h>
#include "Group.h"
#include "GroupMgr.h"
#include "InstanceScript.h"

static bool removeAura, BossAnnouncerEnable, BossAnnounceToPlayerOnLogin;

class Boss_Announcer : public PlayerScript
{
public:
    Boss_Announcer() : PlayerScript("Boss_Announcer") {}

    void OnLogin(Player* player)
    {
        if (BossAnnouncerEnable)
        {
            if (BossAnnounceToPlayerOnLogin)
            {
                ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00BossAnnouncer |rmodule.");
            }
        }
    }

    void OnCreatureKill(Player* player, Creature* boss)
    {
        if (BossAnnouncerEnable)
        {
            if (boss->GetMap()->IsRaid() && boss->getLevel() > 80 && boss->IsDungeonBoss())
            {
                //lets get the info we want
                Map* map = player->GetMap();
                std::string g_name = "< Sin Hermandad >";
                std::string boss_name = boss->GetName();
                std::string IsHeroicMode;
                std::string IsNormal;
                std::string tag_colour = "7bbef7";
                std::string plr_colour = "7bbef7";
                std::string guild_colour = "00ff00";
                std::string boss_colour = "ff0000";
                std::string alive_text = "00ff00";

                if (player->GetMap()->Is25ManRaid())
                    IsNormal = "25";
                else
                    IsNormal = "10";

                if (player->GetMap()->IsHeroic())
                    IsHeroicMode = "|cffff0000Heroico|r";
                else
                    IsHeroicMode = "|cff00ff00Normal|r";

                std::ostringstream stream;

                Player* leader = player;
                uint64 leaderGuid = player->GetGroup() ? player->GetGroup()->GetLeaderGUID() : player->GetGUID();

                if (leaderGuid != player->GetGUID() && player->GetGroup())
                    leader = ObjectAccessor::FindPlayerInOrOutOfWorld(player->GetGroup()->GetLeaderGUID());

                if (!leader) leader = player;

                if (leader && leader->GetGuild()) g_name = leader->GetGuildName();

                if (leader && AccountMgr::IsPlayerAccount(leader->GetSession()->GetSecurity()))
                {
                    stream << "La hermandad |cff" << guild_colour << "" << g_name <<
                        "|r ha derrotado a |CFF" << boss_colour << "[" << boss_name <<
                        "]|r en modo |cff" << alive_text << IsNormal << "|r " << IsHeroicMode;
                    sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
                }
            }
        }
    }
};

class Boss_Announcer_World : public WorldScript
{
public:
    Boss_Announcer_World() : WorldScript("Boss_Announcer_World") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/mod_boss_announcer.conf";
#ifdef WIN32
            cfg_file = "mod_boss_announcer.conf";
#endif
            std::string cfg_def_file = cfg_file + ".dist";
            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());
            SetInitialWorldSettings();
        }
    }
    void  SetInitialWorldSettings()
    {
        removeAura = sConfigMgr->GetBoolDefault("Boss.Announcer.RemoveAuraUponKill", false);
        BossAnnouncerEnable = sConfigMgr->GetBoolDefault("Boss.Announcer.Enable", true);
        BossAnnounceToPlayerOnLogin = sConfigMgr->GetBoolDefault("Boss.Announcer.Announce", true);
    }
};

void AddBoss_AnnouncerScripts()
{
    new Boss_Announcer_World;
    new Boss_Announcer;
}
