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
                Player* leader = GetLeaderOfGroup(player);

                if (leader)
                {
                    std::ostringstream stream;
                    std::string boss_name = boss->GetName();
                    std::string ManMode = (player->GetMap()->Is25ManRaid()) ? "25" : "10";
                    std::string NormalOrHeroicMode = (player->GetMap()->IsHeroic()) ? " |cffff0000Heroico|r." : " |cff00ff00Normal|r.";

                    if (leader->GetGuild())
                    {
                        stream << "La banda liderada por |cff7bbef7" << leader->GetName() << "|r|cff00ff00 < " << leader->GetGuildName()
                            << " >|r ha derrotado a |cffff0000[" << boss_name << "]|r en modo " << ManMode << NormalOrHeroicMode;
                    }
                    else
                    {
                        stream << "La banda liderada por |cff7bbef7" << leader->GetName()
                            << "|r ha derrotado a |cffff0000[" << boss_name << "]|r en modo " << ManMode << NormalOrHeroicMode;
                    }
                    sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
                }
            }
            else
            {
                // Mazmorra custom para instances
                if (boss->GetMap()->IsDungeon() && player->GetMap()->IsNonRaidDungeon() && player->GetMapId() == 542)
                {
                    Player* leader = GetLeaderOfGroup(player);
                    if (leader)
                    {
                        std::ostringstream stream;
                        std::string boss_name = boss->GetName();

                        if (leader->GetGuild())
                        {
                            stream << "El grupo liderado por |cff7bbef7" << leader->GetName() << "|r|cff00ff00 < " << leader->GetGuildName()
                                << " >|r ha derrotado a |cffff0000[" << boss_name << "]|r en |cffff0000El Horno de Sangre 5 Heroico|r.";
                        }
                        else
                        {
                            stream << "El grupo liderado por |cff7bbef7" << leader->GetName()
                                << "|r ha derrotado a |cffff0000[" << boss_name << "]|r en |cffff0000El Horno de Sangre 5 Heroico|r.";
                        }
                        sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
                    }
                }
                else
                {
                    // Jefes de mundo
                    uint32 bossEntry = boss->GetEntry();
                    if (bossEntry == 2300005 || bossEntry == 2300006)
                    {
                        Player* leader = GetLeaderOfGroup(player);
                        if (leader)
                        {
                            std::ostringstream stream;
                            std::string boss_name = boss->GetName();

                            if (leader->GetGuild())
                            {
                                stream << "La banda liderada por |cff7bbef7" << leader->GetName() << "|r|cff00ff00 < " << leader->GetGuildName()
                                    << " >|r ha derrotado al Jefe de Mundo |cffff0000[" << boss_name << "]|r.";
                            }
                            else
                            {
                                stream << "La banda liderada por |cff7bbef7" << leader->GetName()
                                    << "|r ha derrotado al Jefe de Mundo |cffff0000[" << boss_name << "]|r.";
                            }
                            sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str().c_str());
                        }
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
        {
            leader = ObjectAccessor::FindPlayerInOrOutOfWorld(player->GetGroup()->GetLeaderGUID());
            if (!leader) leader = player;
        }

        if (leader /*&& AccountMgr::IsPlayerAccount(leader->GetSession()->GetSecurity())*/)
        {
            return leader;
        }

        return nullptr;
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
