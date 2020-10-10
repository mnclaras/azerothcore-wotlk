/*

# Custom Login Modifications #

#### A module for AzerothCore by [StygianTheBest](https://github.com/StygianTheBest/AzerothCore-Content/tree/master/Modules)
------------------------------------------------------------------------------------------------------------------


### Description ###
------------------------------------------------------------------------------------------------------------------
This module performs several actions on new players. It has the option to give new players BOA starting gear,
additional weapon skills, and special abilities such as custom spells. It can also set the reputation of the player
to exalted with all capital cities for their faction granting them the Ambassador title. This is typically done in
the core's config file, but it's bugged (as of 2017.08.23) in AzerothCore. It can also announce when players login
or logoff the server.


### Features ###
------------------------------------------------------------------------------------------------------------------
- Player ([ Faction ] - Name - Logon/Logoff message) notification can be announced to the world
- New characters can receive items, bags, and class-specific heirlooms
- New characters can receive additional weapon skills
- New characters can receive special abilities
- New characters can receive exalted rep with capital cities (Title: Ambassador) on first login


### Data ###
------------------------------------------------------------------------------------------------------------------
- Type: Player/Server
- Script: CustomLogin
- Config: Yes
    - Enable Module
    - Enable Module Announce
    - Enable Announce Player Login/Logoff
    - Enable Starting Gear for new players
    - Enable Additional Weapon Skills for new players
    - Enable Special Abilities for new players
    - Enable Reputation Boost for new players
- SQL: No


### Version ###
------------------------------------------------------------------------------------------------------------------
- v2017.07.26 - Release
- v2017.07.29 - Clean up code, Add rep gain, Add config options


### Credits ###
------------------------------------------------------------------------------------------------------------------
- [Blizzard Entertainment](http://blizzard.com)
- [TrinityCore](https://github.com/TrinityCore/TrinityCore/blob/3.3.5/THANKS)
- [SunwellCore](http://www.azerothcore.org/pages/sunwell.pl/)
- [AzerothCore](https://github.com/AzerothCore/azerothcore-wotlk/graphs/contributors)
- [AzerothCore Discord](https://discord.gg/gkt4y2x)
- [EMUDevs](https://youtube.com/user/EmuDevs)
- [AC-Web](http://ac-web.org/)
- [ModCraft.io](http://modcraft.io/)
- [OwnedCore](http://ownedcore.com/)
- [OregonCore](https://wiki.oregon-core.net/)
- [Wowhead.com](http://wowhead.com)
- [AoWoW](https://wotlk.evowow.com/)


### License ###
------------------------------------------------------------------------------------------------------------------
- This code and content is released under the [GNU AGPL v3](https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3).

*/


#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "ScriptMgr.h"
#include "GuildMgr.h"

class CustomLogin : public PlayerScript
{

public:
    CustomLogin() : PlayerScript("CustomLogin") { }

    void OnFirstLogin(Player* player)
    {

        // Learn skills

        // Equitacion

        // Riding 75
        if (!player->HasActiveSpell(33388))
        {
            player->learnSpell(33388);
        }
        // Riding 150
        if (!player->HasActiveSpell(33391))
        {
            player->learnSpell(33391);
        }
        // Riding 225
        if (!player->HasActiveSpell(34090))
        {
            player->learnSpell(34090);
        }
        // Riding 300
        if (!player->HasActiveSpell(34091))
        {
            player->learnSpell(34091);
        }
        // Cold Weather Flying
        if (!player->HasActiveSpell(54197))
        {
            player->learnSpell(54197);
        }

        switch (player->getClass())
        {

            /*
                // Skill Reference

                player->learnSpell(204);	// Defense
                player->learnSpell(264);	// Bows
                player->learnSpell(5011);	// Crossbow
                player->learnSpell(674);	// Dual Wield
                player->learnSpell(15590);	// Fists
                player->learnSpell(266);	// Guns
                player->learnSpell(196);	// Axes
                player->learnSpell(198);	// Maces
                player->learnSpell(201);	// Swords
                player->learnSpell(750);	// Plate Mail
                player->learnSpell(200);	// PoleArms
                player->learnSpell(9116);	// Shields
                player->learnSpell(197);	// 2H Axe
                player->learnSpell(199);	// 2H Mace
                player->learnSpell(202);	// 2H Sword
                player->learnSpell(227);	// Staves
                player->learnSpell(2567);	// Thrown
                player->learnSpell(1180);	// Daggers
            */

        case CLASS_PALADIN:
            player->learnSpell(196);	// Axes
            player->learnSpell(750);	// Plate Mail
            player->learnSpell(200);	// PoleArms
            player->learnSpell(197);	// 2H Axe
            player->learnSpell(199);	// 2H Mace
            break;

        case CLASS_SHAMAN:
            player->learnSpell(15590);	// Fists
            player->learnSpell(8737);	// Mail
            player->learnSpell(196);	// Axes
            player->learnSpell(197);	// 2H Axe
            player->learnSpell(199);	// 2H Mace
			player->learnSpell(1180);	// Daggers
            break;

        case CLASS_WARRIOR:
            player->learnSpell(264);	// Bows
            player->learnSpell(5011);	// Crossbow
            player->learnSpell(674);	// Dual Wield
            player->learnSpell(15590);	// Fists
            player->learnSpell(266);	// Guns
            player->learnSpell(750);	// Plate Mail
            player->learnSpell(200);	// PoleArms
            player->learnSpell(199);	// 2H Mace
            player->learnSpell(227);	// Staves
            break;

        case CLASS_HUNTER:
            player->learnSpell(674);	// Dual Wield
            player->learnSpell(15590);	// Fists
            player->learnSpell(266);	// Guns
            player->learnSpell(8737);	// Mail
            player->learnSpell(200);	// PoleArms
            player->learnSpell(227);	// Staves
            player->learnSpell(202);	// 2H Sword
            player->learnSpell(5011);	// Crossbow
            break;

        case CLASS_ROGUE:
            player->learnSpell(264);	// Bows
            player->learnSpell(5011);	// Crossbow
            player->learnSpell(15590);	// Fists
            player->learnSpell(266);	// Guns
            player->learnSpell(196);	// Axes
            player->learnSpell(198);	// Maces
            player->learnSpell(201);	// Swords
            break;

        case CLASS_DRUID:
            player->learnSpell(1180);	// Daggers
            player->learnSpell(15590);	// Fists
            player->learnSpell(198);	// Maces
            player->learnSpell(200);	// PoleArms
            player->learnSpell(227);	// Staves
            player->learnSpell(199);	// 2H Mace
            break;

        case CLASS_MAGE:
            player->learnSpell(201);	// Swords
            player->learnSpell(1180);	// Daggers
            break;

        case CLASS_WARLOCK:
            player->learnSpell(201);	// Swords
            break;

        case CLASS_PRIEST:
            player->learnSpell(1180);	// Daggers
            break;

        case CLASS_DEATH_KNIGHT:
            player->learnSpell(198);	// Maces
            player->learnSpell(199);	// 2H Mace
            break;

        default:
            break;
        }

        player->UpdateSkillsForLevel();

        // Inform the player they have new skills
        std::ostringstream ss1;
        ss1 << "|cffFF0000[CustomLogin]:|cffFF8000 You have been granted additional weapon skills.";
        ChatHandler(player->GetSession()).SendSysMessage(ss1.str().c_str());


        // Set exalted factions
        switch (player->GetTeamId())
        {

            // Alliance Capital Cities
        case TEAM_ALLIANCE:
            player->SetReputation(47, 999999);	// IronForge
            player->SetReputation(72, 999999);	// Stormwind
            player->SetReputation(69, 999999);	// Darnassus
            player->SetReputation(389, 999999);	// Gnomeregan
            player->SetReputation(930, 999999);	// Exodar
            break;

            // Horde Capital Cities
        case TEAM_HORDE:
            player->SetReputation(68, 999999);	// Undercity
            player->SetReputation(76, 999999);	// Orgrimmar
            player->SetReputation(81, 999999);	// Thunder Bluff
            player->SetReputation(530, 999999);	// DarkSpear
            player->SetReputation(911, 999999);	// Silvermoon
            break;

        default:
            break;
        }

        // Inform the player they have exalted reputations
        std::ostringstream ss2;
        ss2 << "|cffFF0000[CustomLogin]:|cffFF8000 Your are now Exalted with your faction's capital cities " << player->GetName() << ".";
        ChatHandler(player->GetSession()).SendSysMessage(ss2.str().c_str());

        // if (sConfigMgr->GetBoolDefault("CustomLogin.ReputationAshenVerdict", false))
        // {
            // player->SetReputation(1156, 999999);	// The Ashen Veredict

            // std::ostringstream ss;
            // ss << "|cffFF0000[CustomLogin]:|cffFF8000 Your are now Exalted with The Ashen Veredict " << player->GetName() << ".";
            // ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());		
        // }
    }

    void OnLogin(Player* player)
    {
        // If enabled..
        // if (sConfigMgr->GetBoolDefault("CustomLogin.Enable", true))
        // {
            // // Announce Module
            // if (sConfigMgr->GetBoolDefault("CustomLogin.Announce", false))
            // {
                // ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00CustomLogin |rmodule.");
            // }

            // // If enabled..
            // if (sConfigMgr->GetBoolDefault("CustomLogin.PlayerAnnounce", false))
            // {
                // // Announce Player Login
                // if (player->GetTeamId() == TEAM_ALLIANCE)
                // {
                    // std::ostringstream ss;
                    // ss << "|cffFFFFFF[|cff2897FF Alliance |cffFFFFFF]:|cff4CFF00 " << player->GetName() << "|cffFFFFFF has come online.";
                    // sWorld->SendServerMessage(SERVER_MSG_STRING, ss.str().c_str());
                // }
                // else
                // {
                    // std::ostringstream ss;
                    // ss << "|cffFFFFFF[|cffFF0000 Horde |cffFFFFFF]:|cff4CFF00 " << player->GetName() << "|cffFFFFFF has come online.";
                    // sWorld->SendServerMessage(SERVER_MSG_STRING, ss.str().c_str());
                // }
            // }
        // }
    }

    void OnLogout(Player* player)
    {
        // if (sConfigMgr->GetBoolDefault("CustomLogin.Enable", true))
        // {
            // // If enabled..
            // if (sConfigMgr->GetBoolDefault("CustomLogin.PlayerAnnounce", false))
            // {
                // // Announce Player Login
                // if (player->GetTeamId() == TEAM_ALLIANCE)
                // {
                    // std::ostringstream ss;
                    // ss << "|cffFFFFFF[|cff2897FF Alliance |cffFFFFFF]|cff4CFF00 " << player->GetName() << "|cffFFFFFF has left the game.";
                    // sWorld->SendServerMessage(SERVER_MSG_STRING, ss.str().c_str());
                // }
                // else
                // {
                    // std::ostringstream ss;
                    // ss << "|cffFFFFFF[|cffFF0000 Horde |cffFFFFFF]|cff4CFF00 " << player->GetName() << "|cffFFFFFF has left the game.";
                    // sWorld->SendServerMessage(SERVER_MSG_STRING, ss.str().c_str());
                // }
            // }
        // }
    }
};

void AddCustomLoginScripts()
{
    new CustomLogin();
}
