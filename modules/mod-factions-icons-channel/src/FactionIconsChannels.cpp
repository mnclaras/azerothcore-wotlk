/*
 * This file is part of the WarheadCore Project. See AUTHORS file for Copyright information
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

#include "Log.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "Player.h"
#include "Channel.h"
#include "AccountMgr.h"
#include "Configuration/Config.h"

class FactionsIconsChannel_Player : public PlayerScript
{
public:
    FactionsIconsChannel_Player() : PlayerScript("FactionsIconsChannel_Player") { }

    void OnChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& msg, Channel* channel) override
    {
        if (!player || !channel)
            return;

        if (!sConfigMgr->GetBoolDefault("ChannelIconFaction.Enable", false))
            return;

        if (sConfigMgr->GetBoolDefault("ChannelIconFaction.OnlyLFG", false) && !channel->IsLFG())
            return;

        if (!sConfigMgr->GetBoolDefault("ChannelIconFaction.GM", false) && !AccountMgr::IsPlayerAccount(player->GetSession()->GetSecurity()))
            return;

        std::string iconHorde = "|TInterface\\PVPFrame\\PVP-Currency-Horde:18:18:-3:-3|t";
        std::string iconAlliance = "|TInterface\\PVPFrame\\PVP-Currency-Alliance:18:18:-3:-3|t";

        std::stringstream ssMsg;
        ssMsg << ((player->GetTeamId() == TEAM_HORDE) ? iconHorde : iconAlliance) << msg;
        msg = ssMsg.str();
    }
};

class FactionsIconsChannel_World : public WorldScript
{
public:
    FactionsIconsChannel_World() : WorldScript("FactionsIconsChannel_World") { }

	void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/factioniconschannels.conf";
            std::string cfg_def_file = cfg_file + ".dist";

            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }
};

// Group all custom scripts
void AddSC_FactionIconsChannels()
{
    new FactionsIconsChannel_Player();
    new FactionsIconsChannel_World();
}
