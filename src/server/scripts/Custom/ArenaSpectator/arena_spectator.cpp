/*
* Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
Name: Arena Spectator
%Complete: 100
Comment: Script allow spectate arena games
Category: Custom Script
EndScriptData */

#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "Chat.h"
#include "Language.h"
#include "ArenaTeamMgr.h"
#include "BattlegroundMgr.h"
#include "WorldSession.h"
#include "Player.h"
#include "ArenaTeam.h"
#include "Battleground.h"
#include "CreatureTextMgr.h"
#include "ObjectAccessor.h"
#include "Config.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Opcodes.h"
#include "ArenaSpectator.h"
#include "LFGMgr.h"

#include "Creature.h"
#include "Chat.h"
#include "Formulas.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "MapManager.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "Util.h"
#include "World.h"
#include "WorldPacket.h"
#include "ArenaSpectator.h"
#include "BattlegroundBE.h"
#include "BattlegroundDS.h"
#include "BattlegroundNA.h"
#include "BattlegroundRL.h"
#include "BattlegroundRV.h"
#include "Transport.h"
#include "ScriptMgr.h"
#include "GameGraveyard.h"

#define sArenaTeamMgr ArenaTeamMgr::instance()
#define HIGHGUID_PLAYER         HighGuid::HIGHGUID_PLAYER
int8 UsingGossip;

class arena_spectator_commands : public CommandScript
{
public:
    arena_spectator_commands() : CommandScript("arena_spectator_commands") { }

    static bool HandleSpectatorCommand(ChatHandler* handler, char const*  /*args*/)
    {
        handler->PSendSysMessage("Incorrect syntax.");
        handler->PSendSysMessage("Command has subcommands:");
        handler->PSendSysMessage("   spectate");
        handler->PSendSysMessage("   leave");
        
        return true;
    }

    static bool HandleSpectatorVersionCommand(ChatHandler* handler, char const* args)
    {
        if (atoi(args) < SPECTATOR_ADDON_VERSION)
            ArenaSpectator::SendCommand(handler->GetSession()->GetPlayer(), "%sOUTDATED", SPECTATOR_ADDON_PREFIX);
        return true;
    }

    static bool HandleSpectatorResetCommand(ChatHandler* handler, char const*  /*args*/)
    {
        Player* p = handler->GetSession()->GetPlayer();
        if (!p->IsSpectator())
            return true;
        ArenaSpectator::HandleResetCommand(p);
        return true;
    }

    static bool HandleSpectatorLeaveCommand(ChatHandler* handler, char const*  /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player->IsSpectator() || !player->FindMap() || !player->FindMap()->IsBattleArena())
        {
            bool isSpanish = IsSpanishPlayer(player);
            handler->SendSysMessage(isSpanish ? "No eres un espectador." : "You are not a spectator.");
            return true;
        }

        //player->SetIsSpectator(false);
        player->TeleportToEntryPoint();
        return true;
    }

    static bool HandleSpectatorSpectateCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        bool isSpanish = IsSpanishPlayer(player);
        std::list<std::string> errors;
        if (!*args)
        {
            handler->SendSysMessage(isSpanish ? "Nombre de jugador no encontrado." : "Missing player name.");
            return true;
        }
        if (player->IsSpectator())
        {
            if (player->FindMap() && player->FindMap()->IsBattleArena())
            {
                HandleSpectatorWatchCommand(handler, args);
                return true;
            }
            handler->PSendSysMessage(isSpanish ? "Ya estas espectando una arena." : "You are already spectacting arena.");
            return true;
        }
        if (player->getClass() == CLASS_DEATH_KNIGHT && player->GetMapId() == 609)
        {
            handler->PSendSysMessage(isSpanish ? "Los DK no pueden espectar antes de acabar la cadena de misiones." : "Death Knights can't spectate before finishing questline.");
            return true;
        }

        std::string name = std::string(args);
        Player* spectate = ObjectAccessor::FindPlayerByName(name);
        if (!spectate)
        {
            handler->SendSysMessage(isSpanish ? "El jugador no ha sido encontrado." : "Requested player not found.");
            return true;
        }
        if (spectate->IsSpectator())
        {
            handler->SendSysMessage(isSpanish ? "El jugador es un espectador." : "Requested player is a spectator.");
            return true;
        }
        if (!spectate->FindMap() || !spectate->FindMap()->IsBattleArena())
        {
            handler->SendSysMessage(isSpanish ? "El jugador no esta en una arena." : "Requested player is not in arena.");
            return true;
        }
        BattlegroundMap* bgmap = ((BattlegroundMap*)spectate->FindMap());
        if (!bgmap->GetBG() || bgmap->GetBG()->GetStatus() == STATUS_WAIT_LEAVE)
        {
            handler->SendSysMessage(isSpanish ? "La arena ha finalizado." : "This arena battle has finished.");
            return true;
        }

        if (player->IsBeingTeleported() || !player->IsInWorld())
            errors.push_back(isSpanish ? "No puedes espectar mientras estas siendo teletransportado." : "Can't use while being teleported.");
        if (!player->FindMap() || player->FindMap()->Instanceable())
            errors.push_back(isSpanish ? "No puedes espectar mientras estas en una instancia, BG o arena." : "Can't use while in instance, bg or arena.");
        if (player->GetVehicle())
            errors.push_back(isSpanish ? "No puedes estar en un vehiculo o montura." : "Can't be on a vehicle.");
        if (player->IsInCombat())
            errors.push_back(isSpanish ? "No puedes estar en combate." : "Can't be in combat.");
        if (player->isUsingLfg())
            errors.push_back(isSpanish ? "No puedes espectar mientras estas coleado en el sistema LFG." : "Can't spectate while using LFG system.");
        if (player->InBattlegroundQueue())
            errors.push_back(isSpanish ? "No puedes estar anotando en arena o BG." : "Can't be queued for arena or bg.");
        if (player->GetGroup())
            errors.push_back(isSpanish ? "No puedes estar en un grupo." : "Can't be in a group.");
        if (player->HasUnitState(UNIT_STATE_ISOLATED))
            errors.push_back(isSpanish ? "No puedes estar aislado." : "Can't be isolated.");
        if (player->m_mover != player)
            errors.push_back(isSpanish ? "Debes tener el control de ti mismo." : "You must control yourself.");
        if (player->IsInFlight())
            errors.push_back(isSpanish ? "No puedes estar volando." : "Can't be in flight.");
        if (player->IsMounted())
            errors.push_back(isSpanish ? "Debes desmontarte antes de espectar." : "Dismount before spectating.");
        if (!player->IsAlive())
            errors.push_back(isSpanish ? "Debes estar vivo." : "Must be alive.");
        if (!player->m_Controlled.empty())
            errors.push_back(isSpanish ? "No puedes estar controlando criaturas." : "Can't be controlling creatures.");

        const Unit::VisibleAuraMap* va = player->GetVisibleAuras();
        for (Unit::VisibleAuraMap::const_iterator itr = va->begin(); itr != va->end(); ++itr)
            if (Aura* aura = itr->second->GetBase())
                if (!itr->second->IsPositive() && !aura->IsPermanent() && aura->GetDuration() < HOUR * IN_MILLISECONDS)
                {
                    switch (aura->GetSpellInfo()->Id)
                    {
                    case lfg::LFG_SPELL_DUNGEON_DESERTER:
                    case lfg::LFG_SPELL_DUNGEON_COOLDOWN:
                    case 26013: // bg deserter
                    case 57724: // sated
                    case 57723: // exhaustion
                    case 25771: // forbearance
                    case 15007: // resurrection sickness
                    case 24755: // Tricked or Treated (z eventu)
                        continue;
                    }

                    errors.push_back(isSpanish ? "No puedes tener auras negativas." : "Can't have negative auras.");
                    break;
                }

        if (uint32 inviteInstanceId = player->GetPendingSpectatorInviteInstanceId())
        {
            if (Battleground* tbg = sBattlegroundMgr->GetBattleground(inviteInstanceId))
                tbg->RemoveToBeTeleported(player->GetGUID());
            player->SetPendingSpectatorInviteInstanceId(0);
        }

        bool bgPreparation = false;
        if ((!handler->GetSession()->GetSecurity() && bgmap->GetBG()->GetStatus() != STATUS_IN_PROGRESS) ||
            (handler->GetSession()->GetSecurity() && bgmap->GetBG()->GetStatus() != STATUS_WAIT_JOIN && bgmap->GetBG()->GetStatus() != STATUS_IN_PROGRESS))
        {
            bgPreparation = true;
            handler->SendSysMessage(isSpanish ? "La arena todavia no ha comenzado. Seras invitado tan pronto como empiece." : "Arena is not in progress yet. You will be invited as soon as it starts.");
            bgmap->GetBG()->AddToBeTeleported(player->GetGUID(), spectate->GetGUID());
            player->SetPendingSpectatorInviteInstanceId(spectate->GetBattlegroundId());
        }

        if (!errors.empty())
        {
            handler->PSendSysMessage(isSpanish ? "Para espectar, por favor arregla lo siguiente:" : "To spectate, please fix the following:");
            for (std::list<std::string>::const_iterator itr = errors.begin(); itr != errors.end(); ++itr)
                handler->PSendSysMessage("- %s", (*itr).c_str());

            return true;
        }

        if (bgPreparation)
            return true;

        // search for two teams
        Battleground* bGround = spectate->GetBattleground();
        if (bGround->isRated())
        {
            uint32 slot = bGround->GetArenaType() - 2;
            if (bGround->GetArenaType() > 3)
                slot = 2;
            uint32 firstTeamID = spectate->GetArenaTeamId(slot);
            uint32 secondTeamID = 0;
            Player* firstTeamMember = spectate;
            Player* secondTeamMember = NULL;
            for (Battleground::BattlegroundPlayerMap::const_iterator itr = bGround->GetPlayers().begin(); itr != bGround->GetPlayers().end(); ++itr)
                if (Player* tmplayer = ObjectAccessor::FindPlayer(itr->first))
                {
                    if (tmplayer->IsSpectator())
                        continue;

                    uint32 tmpID = tmplayer->GetArenaTeamId(slot);
                    if (tmpID != firstTeamID && tmpID > 0)
                    {
                        secondTeamID = tmpID;
                        secondTeamMember = tmplayer;
                        break;
                    }
                }

            if (firstTeamID > 0 && secondTeamID > 0 && secondTeamMember)
            {
                ArenaTeam* firstTeam = sArenaTeamMgr->GetArenaTeamById(firstTeamID);
                ArenaTeam* secondTeam = sArenaTeamMgr->GetArenaTeamById(secondTeamID);
                if (firstTeam && secondTeam)
                {
                    if (isSpanish)
                    {
                        handler->PSendSysMessage("Has entrado en una arena puntuada.");
                        handler->PSendSysMessage("Equipos:");
                        handler->PSendSysMessage("|cFFffffff%s|r vs |cFFffffff%s|r", firstTeam->GetName().c_str(), secondTeam->GetName().c_str());
                        handler->PSendSysMessage("|cFFffffff%u(%u)|r -- |cFFffffff%u(%u)|r", firstTeam->GetRating(), firstTeam->GetAverageMMR(firstTeamMember->GetGroup()),
                            secondTeam->GetRating(), secondTeam->GetAverageMMR(secondTeamMember->GetGroup()));
                        handler->PSendSysMessage("Para salir del espectador, escribe .spect leave. Tambien puedes hacer 1a persona escribiendo .spect watch NombreJugador, y .spect watch NombreJugador otra vez para cancelarlo.");
                    }
                    else
                    {
                        handler->PSendSysMessage("You entered a Rated Arena.");
                        handler->PSendSysMessage("Teams:");
                        handler->PSendSysMessage("|cFFffffff%s|r vs |cFFffffff%s|r", firstTeam->GetName().c_str(), secondTeam->GetName().c_str());
                        handler->PSendSysMessage("|cFFffffff%u(%u)|r -- |cFFffffff%u(%u)|r", firstTeam->GetRating(), firstTeam->GetAverageMMR(firstTeamMember->GetGroup()),
                            secondTeam->GetRating(), secondTeam->GetAverageMMR(secondTeamMember->GetGroup()));
                        handler->PSendSysMessage("To exit spectate, type .spect leave. You can also do player POV typing .spect watch PlayerName, and .spect watch PlayerName another time to cancel POV.");
                    }
                }
            }
        }

        player->SetPendingSpectatorForBG(spectate->GetBattlegroundId());
        player->SetBattlegroundId(spectate->GetBattlegroundId(), spectate->GetBattlegroundTypeId(), PLAYER_MAX_BATTLEGROUND_QUEUES, false, false, TEAM_NEUTRAL);
        player->SetEntryPoint();
        float z = spectate->GetMapId() == 618 ? std::max(28.27f, spectate->GetPositionZ() + 0.25f) : spectate->GetPositionZ() + 0.25f;
        player->TeleportTo(spectate->GetMapId(), spectate->GetPositionX(), spectate->GetPositionY(), z, spectate->GetOrientation(), TELE_TO_GM_MODE);
        return true;
    }

    static bool HandleSpectatorWatchCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return true;

        Player* player = handler->GetSession()->GetPlayer();
        if (!player->IsSpectator())
            return true;

        if (!player->FindMap() || !player->FindMap()->IsBattleArena())
            return true;

        Battleground* bg = ((BattlegroundMap*)player->FindMap())->GetBG();
        if (!bg || bg->GetStatus() != STATUS_IN_PROGRESS)
            return true;

        std::string name = std::string(args);
        Player* spectate = ObjectAccessor::FindPlayerByName(name);
        if (!spectate || !spectate->IsAlive() || spectate->IsSpectator() || spectate->GetGUID() == player->GetGUID() || !spectate->IsInWorld() || !spectate->FindMap() || spectate->IsBeingTeleported() || spectate->FindMap() != player->FindMap() || !bg->IsPlayerInBattleground(spectate->GetGUID()))
            return true;

        if (WorldObject* o = player->GetViewpoint())
            if (Unit* u = o->ToUnit())
            {
                u->RemoveAurasByType(SPELL_AURA_BIND_SIGHT, player->GetGUID());
                player->RemoveAurasDueToSpell(SPECTATOR_SPELL_BINDSIGHT, player->GetGUID(), (1 << EFFECT_1));

                if (u->GetGUID() == spectate->GetGUID())
                    return true;
            }

        if (player->GetUInt64Value(PLAYER_FARSIGHT) || player->m_seer != player) // pussywizard: below this point we must not have a viewpoint!
            return true;

        if (player->HaveAtClient(spectate))
            player->CastSpell(spectate, SPECTATOR_SPELL_BINDSIGHT, true);

        return true;
    }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> spectatorCommandTable =
        {
            { "version",        SEC_CONSOLE,        false, &HandleSpectatorVersionCommand,                  "" },
            { "reset",          SEC_CONSOLE,        false, &HandleSpectatorResetCommand,                    "" },
            { "spectate",       SEC_CONSOLE,        false, &HandleSpectatorSpectateCommand,                 "" },
            { "watch",          SEC_CONSOLE,        false, &HandleSpectatorWatchCommand,                    "" },
            { "leave",          SEC_CONSOLE,        false, &HandleSpectatorLeaveCommand,                    "" },
            { "",               SEC_CONSOLE,        false, &HandleSpectatorCommand,                         "" }
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "spect",          SEC_CONSOLE,        false, nullptr,                                         "", spectatorCommandTable }
        };
        return commandTable;
    }

};

std::vector<Battleground*> ratedArenas;


void LoadAllArenas()
{
    ratedArenas.clear();
    for (uint8 i = 0; i <= MAX_BATTLEGROUND_TYPE_ID; ++i)
    {
        if (!sBattlegroundMgr->IsArenaType(BattlegroundTypeId(i)))
            continue;

        const BattlegroundContainer& arenas = sBattlegroundMgr->GetBattlegroundList();

        for (BattlegroundContainer::const_iterator itr = arenas.begin(); itr != arenas.end(); ++itr)
        {
            Battleground* arena = itr->second;
            if (arena->GetBgTypeID() == BattlegroundTypeId(i))
            {
                if (!arena->GetPlayersSize())
                    continue;

                if (!arena->isRated())
                    continue;

                ratedArenas.push_back(arena);
            }
        }
    }

    if (ratedArenas.size() < 2)
        return;

    std::vector<Battleground*>::iterator itr = ratedArenas.begin();
    int count = 0;
    for (; itr != ratedArenas.end(); ++itr)
    {
        if (!(*itr))
        {
            count++;
            continue;
        }

        // I have no idea if this event could ever happen, but if it did, it would most likely
        // cause crash
        int size = ratedArenas.size();
        if (count >= size)
            return;

        // Bubble sort, oh yeah, that's the stuff..     
        for (int i = count; i < size; i++)
        {
            if (Battleground* tmpBg = ratedArenas[i])
            {
                Battleground* tmp = (*itr);
                (*itr) = ratedArenas[i];
                ratedArenas[i] = tmp;
            }
        }
        count++;
    }

    return;
}

uint16 GetSpecificArenasCount(ArenaType type, uint16 count1)
{
    if (ratedArenas.empty())
        return 0;

    uint16 count[2] = { 0, 0 };

    for (std::vector<Battleground*>::const_iterator citr = ratedArenas.begin(); citr != ratedArenas.end(); ++citr)
    {
        if (Battleground* arena = (*citr))
        {
            if (arena->GetArenaType() == type)
            {
                if (arena->GetStatus() == STATUS_IN_PROGRESS)
                    count[1]++;

                count[0]++;
            }
        }
    }

    count1 = count[1];
    return count[0];
}


enum ClassTalents
{
    SPELL_DK_BLOOD = 62905,    // Not Dancing Rune Weapon (i used Improved Death Strike)
    SPELL_DK_FROST = 49143,    // Not Howling Blash (i used Frost Strike)
    SPELL_DK_UNHOLY = 49206,

    SPELL_DRUID_BALANCE = 48505,
    SPELL_DRUID_FERAL_COMBAT = 50334,
    SPELL_DRUID_RESTORATION = 63410,    // Not Wild Growth (i used Improved Barkskin)

    SPELL_HUNTER_BEAST_MASTERY = 53270,
    SPELL_HUNTER_MARKSMANSHIP = 53209,
    SPELL_HUNTER_SURVIVAL = 53301,

    SPELL_MAGE_ARCANE = 44425,
    SPELL_MAGE_FIRE = 31661,    // Not Living Bomb (i used Dragon's Breath)
    SPELL_MAGE_FROST = 44572,

    SPELL_PALADIN_HOLY = 53563,
    SPELL_PALADIN_PROTECTION = 53595,
    SPELL_PALADIN_RETRIBUTION = 53385,

    SPELL_PRIEST_DISCIPLINE = 47540,
    SPELL_PRIEST_HOLY = 47788,
    SPELL_PRIEST_SHADOW = 47585,

    SPELL_ROGUE_ASSASSINATION = 1329,     // Not Hunger For Blood (i used Mutilate)
    SPELL_ROGUE_COMBAT = 51690,
    SPELL_ROGUE_SUBTLETY = 51713,

    SPELL_SHAMAN_ELEMENTAL = 51490,
    SPELL_SHAMAN_ENHACEMENT = 30823,    // Not Feral Spirit (i used Shamanistic Rage)
    SPELL_SHAMAN_RESTORATION = 974,      // Not Riptide (i used Earth Shield)

    SPELL_WARLOCK_AFFLICTION = 48181,
    SPELL_WARLOCK_DEMONOLOGY = 59672,
    SPELL_WARLOCK_DESTRUCTION = 50796,

    SPELL_WARRIOR_ARMS = 46924,
    SPELL_WARRIOR_FURY = 23881,    // Not Titan's Grip (i used Bloodthirst)
    SPELL_WARRIOR_PROTECTION = 46968,
};

enum NpcSpectatorAtions {
    NPC_SPECTATOR_ACTION_MAIN_MENU  = 1,
    NPC_SPECTATOR_ACTION_SPECIFIC   = 10,
    NPC_SPECTATOR_ACTION_2V2_GAMES  = 1000000,
    NPC_SPECTATOR_ACTION_3V3_GAMES  = 2000000,
    NPC_SPECTATOR_ACTION_3V3S_GAMES = 3000000,

    NPC_SPECTATOR_ACTION_SELECTED_PLAYER = 5000000
};

const uint8  GamesOnPage = 15;

class npc_arena_spectator : public CreatureScript
{
public:
    npc_arena_spectator() : CreatureScript("npc_arena_spectator") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->PlayerTalkClass->ClearMenus();
        CloseGossipMenuFor(player);

        LoadAllArenas();
        uint32 arenasQueueTotal[3] = { 0, 0, 0 };
        uint32 arenasQueuePlaying[3] = { 0, 0, 0 };
        arenasQueueTotal[0] = GetSpecificArenasCount(ARENA_TYPE_2v2, arenasQueuePlaying[0]);
        arenasQueueTotal[1] = GetSpecificArenasCount(ARENA_TYPE_3v3, arenasQueuePlaying[1]);
        /* Cuando haya soloq */
        /*arenasQueueTotal[2] = GetSpecificArenasCount(ARENA_TYPE_3v3_SOLO, arenasQueuePlaying[2]);*/
        arenasQueueTotal[2] = GetSpecificArenasCount(ARENA_TYPE_5v5, arenasQueuePlaying[2]);

        bool isSpanish = IsSpanishPlayer(player);

        std::stringstream Gossip2s;
        std::stringstream Gossip3s;
        std::stringstream Gossip3ss;

        if (isSpanish)
        {
            Gossip2s << "|TInterface\\icons\\Achievement_Arena_2v2_7:35:35:-30:0|tJuegos: 2v2 (Jugando: " << arenasQueueTotal[0] << ")"/* << arenasQueuePlaying[0] << ")"*/;
            Gossip3s << "|TInterface\\icons\\Achievement_Arena_3v3_7:35:35:-30:0|tJuegos: 3v3 (Jugando: " << arenasQueueTotal[1] << ")"/* << arenasQueuePlaying[1] << ")"*/;
            Gossip3ss << "|TInterface\\icons\\Achievement_Arena_5v5_7:35:35:-30:0|tJuegos: 1v1 (Jugando: " << arenasQueueTotal[2] << ")"/* << arenasQueuePlaying[2] << ")"*/;
        }
        else
        {
            Gossip2s << "|TInterface\\icons\\Achievement_Arena_2v2_7:35:35:-30:0|tGames: 2v2 (Playing: " << arenasQueueTotal[0] << ")"/* << arenasQueuePlaying[0] << ")"*/;
            Gossip3s << "|TInterface\\icons\\Achievement_Arena_3v3_7:35:35:-30:0|tGames: 3v3 (Playing: " << arenasQueueTotal[1] << ")"/* << arenasQueuePlaying[1] << ")"*/;
            Gossip3ss << "|TInterface\\icons\\Achievement_Arena_5v5_7:35:35:-30:0|tGames: 1v1 (Playing: " << arenasQueueTotal[2] << ")"/* << arenasQueuePlaying[2] << ")"*/;
        }
        /* Cuando haya soloq */
        //Gossip3ss << "|TInterface\\icons\\Achievement_Arena_5v5_7:35:35:-30:0|tGames: 3v3 SoloQueue (Playing: " << arenasQueueTotal[2] << ")"/* << arenasQueuePlaying[2] << ")"*/;
        

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, Gossip3ss.str(), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3S_GAMES);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, Gossip2s.str(), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, Gossip3s.str(), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "|TInterface\\icons\\Spell_Holy_DevineAegis:35:35:-30:0|tEspectar jugador especifico."
                                                               : "|TInterface\\icons\\Spell_Holy_DevineAegis:35:35:-30:0|tSpectate Specific Player.",
            GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SPECIFIC, "", 0, true);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        bool isSpanish = IsSpanishPlayer(player);

        if (action == NPC_SPECTATOR_ACTION_MAIN_MENU)
        {
            OnGossipHello(player, creature);
            return true;
        }

        if (action >= NPC_SPECTATOR_ACTION_2V2_GAMES && action < NPC_SPECTATOR_ACTION_3V3_GAMES)
        {
            AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Refrescar" : "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES);
            bool haveMatches = ShowPage(player, action - NPC_SPECTATOR_ACTION_2V2_GAMES, ARENA_TYPE_2v2, NPC_SPECTATOR_ACTION_2V2_GAMES);
            if (haveMatches)
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            else
                OnGossipHello(player, creature);
        }
        else if (action >= NPC_SPECTATOR_ACTION_3V3_GAMES && action < NPC_SPECTATOR_ACTION_3V3S_GAMES)
        {
            AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Refrescar" : "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES);
            bool haveMatches = ShowPage(player, action - NPC_SPECTATOR_ACTION_3V3_GAMES, ARENA_TYPE_3v3, NPC_SPECTATOR_ACTION_3V3_GAMES);
            if (haveMatches)
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            else
                OnGossipHello(player, creature);
        }
        else if (action >= NPC_SPECTATOR_ACTION_3V3S_GAMES && action < NPC_SPECTATOR_ACTION_SELECTED_PLAYER)
        {
            AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Refrescar" : "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3S_GAMES);
            bool haveMatches = ShowPage(player, action - NPC_SPECTATOR_ACTION_3V3S_GAMES, ARENA_TYPE_5v5, NPC_SPECTATOR_ACTION_3V3S_GAMES);
            if (haveMatches)
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            else
                OnGossipHello(player, creature);
        }
        else
        {
            uint64 guid = action - NPC_SPECTATOR_ACTION_SELECTED_PLAYER;
            if (Player* target = ObjectAccessor::FindPlayer(guid))
            {
                ChatHandler handler(player->GetSession());
                char const* pTarget = target->GetName().c_str();
                arena_spectator_commands::HandleSpectatorSpectateCommand(&handler, pTarget);
            }
        }
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override
    {
        if (!player)
            return true;

        player->PlayerTalkClass->ClearMenus();
        CloseGossipMenuFor(player);
        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case NPC_SPECTATOR_ACTION_SPECIFIC: // choosing a player
                const char* plrName = code;

                char playerName[50];
                strcpy(playerName, plrName);

                for (int i = 0; i < 13; i++)
                {
                    if (playerName[i] == NULL)
                        break;
                    if (i == 0 && playerName[i] > 96)
                        playerName[0] -= 32;
                    else if (playerName[i] < 97)
                        playerName[i] += 32;
                }

                if (Player* target = ObjectAccessor::FindPlayerByName(playerName))
                {
                    ChatHandler handler(player->GetSession());
                    char const* pTarget = target->GetName().c_str();
                    arena_spectator_commands::HandleSpectatorSpectateCommand(&handler, pTarget);
                }
                bool isSpanish = IsSpanishPlayer(player);
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "El jugador no esta online o no existe." : "Player is not online or does not exist.");
                return true;
            }
        }
        return false;
    }

    static std::string GetClassNameById(Player* player)
    {
        std::string sClass = "";
        switch (player->getClass())
        {
        case CLASS_WARRIOR:
            if (player->HasTalent(SPELL_WARRIOR_ARMS, player->GetActiveSpec()))
                sClass = "A";
            else if (player->HasTalent(SPELL_WARRIOR_FURY, player->GetActiveSpec()))
                sClass = "F";
            else if (player->HasTalent(SPELL_WARRIOR_PROTECTION, player->GetActiveSpec()))
                sClass = "P";
            sClass += "Warrior ";
            break;
        case CLASS_PALADIN:
            if (player->HasTalent(SPELL_PALADIN_HOLY, player->GetActiveSpec()))
                sClass = "H";
            else if (player->HasTalent(SPELL_PALADIN_PROTECTION, player->GetActiveSpec()))
                sClass = "P";
            else if (player->HasTalent(SPELL_PALADIN_RETRIBUTION, player->GetActiveSpec()))
                sClass = "R";
            sClass += "Paladin ";
            break;
        case CLASS_HUNTER:
            if (player->HasTalent(SPELL_HUNTER_BEAST_MASTERY, player->GetActiveSpec()))
                sClass = "BM";
            else if (player->HasTalent(SPELL_HUNTER_MARKSMANSHIP, player->GetActiveSpec()))
                sClass = "MM";
            else if (player->HasTalent(SPELL_HUNTER_SURVIVAL, player->GetActiveSpec()))
                sClass = "S";
            sClass += "Hunter ";
            break;
        case CLASS_ROGUE:
            if (player->HasTalent(SPELL_ROGUE_ASSASSINATION, player->GetActiveSpec()))
                sClass = "A";
            else if (player->HasTalent(SPELL_ROGUE_COMBAT, player->GetActiveSpec()))
                sClass = "C";
            else if (player->HasTalent(SPELL_ROGUE_SUBTLETY, player->GetActiveSpec()))
                sClass = "S";
            sClass += "Rogue ";
            break;
        case CLASS_PRIEST:
            if (player->HasTalent(SPELL_PRIEST_DISCIPLINE, player->GetActiveSpec()))
                sClass = "D";
            else if (player->HasTalent(SPELL_PRIEST_HOLY, player->GetActiveSpec()))
                sClass = "H";
            else if (player->HasTalent(SPELL_PRIEST_SHADOW, player->GetActiveSpec()))
                sClass = "S";
            sClass += "Priest ";
            break;
        case CLASS_DEATH_KNIGHT:
            if (player->HasTalent(SPELL_DK_BLOOD, player->GetActiveSpec()))
                sClass = "B";
            else if (player->HasTalent(SPELL_DK_FROST, player->GetActiveSpec()))
                sClass = "F";
            else if (player->HasTalent(SPELL_DK_UNHOLY, player->GetActiveSpec()))
                sClass = "U";
            sClass += "DK ";
            break;
        case CLASS_SHAMAN:
            if (player->HasTalent(SPELL_SHAMAN_ELEMENTAL, player->GetActiveSpec()))
                sClass = "EL";
            else if (player->HasTalent(SPELL_SHAMAN_ENHACEMENT, player->GetActiveSpec()))
                sClass = "EN";
            else if (player->HasTalent(SPELL_SHAMAN_RESTORATION, player->GetActiveSpec()))
                sClass = "R";
            sClass += "Shaman ";
            break;
        case CLASS_MAGE:
            if (player->HasTalent(SPELL_MAGE_ARCANE, player->GetActiveSpec()))
                sClass = "A";
            else if (player->HasTalent(SPELL_MAGE_FIRE, player->GetActiveSpec()))
                sClass = "Fi";
            else if (player->HasTalent(SPELL_MAGE_FROST, player->GetActiveSpec()))
                sClass = "Fr";
            sClass += "Mage ";
            break;
        case CLASS_WARLOCK:
            if (player->HasTalent(SPELL_WARLOCK_AFFLICTION, player->GetActiveSpec()))
                sClass = "A";
            else if (player->HasTalent(SPELL_WARLOCK_DEMONOLOGY, player->GetActiveSpec()))
                sClass = "Dem";
            else if (player->HasTalent(SPELL_WARLOCK_DESTRUCTION, player->GetActiveSpec()))
                sClass = "Des";
            sClass += "Warlock ";
            break;
        case CLASS_DRUID:
            if (player->HasTalent(SPELL_DRUID_BALANCE, player->GetActiveSpec()))
                sClass = "B";
            else if (player->HasTalent(SPELL_DRUID_FERAL_COMBAT, player->GetActiveSpec()))
                sClass = "F";
            else if (player->HasTalent(SPELL_DRUID_RESTORATION, player->GetActiveSpec()))
                sClass = "R";
            sClass += "Druid ";
            break;
        default:
            sClass = "<Unknown>";
            break;
        }

        return sClass;
    }

    static std::string GetGamesStringData(Battleground* team, uint16 mmr, uint16 mmrTwo)
    {
        std::string teamsMember[BG_TEAMS_COUNT];
        uint32 firstTeamId = 0;
        for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin(); itr != team->GetPlayers().end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
            {
                if (player->IsSpectator())
                    continue;

                if (player->IsGameMaster())
                    continue;

                uint32 team = itr->second->GetTeamId();
                if (!firstTeamId)
                    firstTeamId = team;

                teamsMember[firstTeamId == team] += player->GetName() + " ";
            }

        std::string data = "";

        std::stringstream ss;
        std::stringstream sstwo;
        ss << mmr;
        sstwo << mmrTwo;

        data += "(" + ss.str() + ") " + teamsMember[0] + "-VS- " + teamsMember[1] + "(" + sstwo.str() + ")";

        return data;
    }

    static uint64 GetFirstPlayerGuid(Battleground* team)
    {
        for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin(); itr != team->GetPlayers().end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
                return itr->first;
        return 0;
    }

    static void GetAllPlayersName(Battleground* team, std::vector<std::string>* vstrPlayers)
    {
        for (Battleground::BattlegroundPlayerMap::const_iterator itr = team->GetPlayers().begin(); itr != team->GetPlayers().end(); ++itr)
            if (Player* player = ObjectAccessor::FindPlayer(itr->first))
                vstrPlayers->push_back(player->GetName());
    }

    static bool ShowPage(Player* player, uint16 page, uint32 IsTop, uint32 type)
    {
        uint32 firstTeamId = 0;
        uint16 TypeOne = 0;
        uint16 TypeTwo = 0;
        uint16 TypeThree = 0;
        uint16 TypePlayer = 0;
        uint16 mmr = 0;
        uint16 mmrTwo = 0;

        //Para buscar player a player, añado un vector de strings
        std::vector<std::string> vstrPlayers;

        bool haveNextPage = false;
        for (uint8 i = 0; i <= MAX_BATTLEGROUND_TYPE_ID; ++i)
        {
            if (!sBattlegroundMgr->IsArenaType(BattlegroundTypeId(i)))
                continue;

            const BattlegroundContainer& arenas = sBattlegroundMgr->GetBattlegroundList();

            for (BattlegroundContainer::const_iterator itr = arenas.begin(); itr != arenas.end(); ++itr)
            {
                Battleground* arena = itr->second;
                if (arena->GetBgTypeID() == BattlegroundTypeId(i))
                {
                    Player* target = ObjectAccessor::FindPlayer(GetFirstPlayerGuid(arena));
                    if (!target)
                        continue;

                    if (target && (target->HasAura(32728) || target->HasAura(32727)))
                        continue;

                    if (!arena->GetPlayersSize())
                        continue;

                    if (!arena->isRated() && arena->GetArenaType() != ARENA_TYPE_5v5)
                        continue;

                    if (arena->GetArenaType() == ARENA_TYPE_2v2)
                    {
                        mmr = arena->GetArenaMatchmakerRating(TEAM_ALLIANCE);
                        firstTeamId = target->GetArenaTeamId(0);
                        Battleground::BattlegroundPlayerMap::const_iterator citr = arena->GetPlayers().begin();
                        for (; citr != arena->GetPlayers().end(); ++citr)
                            if (Player* plrs = ObjectAccessor::FindPlayer(citr->first))
                                if (plrs->GetArenaTeamId(0) != firstTeamId)
                                    mmrTwo = arena->GetArenaMatchmakerRating(citr->second->GetTeamId());
                    }
                    else if (arena->GetArenaType() == ARENA_TYPE_3v3)
                    {
                        mmr = arena->GetArenaMatchmakerRating(TEAM_HORDE);
                        firstTeamId = target->GetArenaTeamId(1);
                        Battleground::BattlegroundPlayerMap::const_iterator citr = arena->GetPlayers().begin();
                        for (; citr != arena->GetPlayers().end(); ++citr)
                            if (Player* plrs = ObjectAccessor::FindPlayer(citr->first))
                                if (plrs->GetArenaTeamId(1) != firstTeamId)
                                    mmrTwo = arena->GetArenaMatchmakerRating(citr->second->GetTeamId());
                    }
                    /* Cuando haya soloq */
                    //else if (arena->GetArenaType() == ARENA_TYPE_3v3_SOLO)
                    else if (arena->GetArenaType() == ARENA_TYPE_5v5)
                    {
                        mmr = arena->GetArenaMatchmakerRating(TEAM_NEUTRAL);
                        
                        firstTeamId = target->GetArenaTeamId(2);
                        ArenaTeam* firstTeam = sArenaTeamMgr->GetArenaTeamById(firstTeamId);
                        if (firstTeam) mmr = firstTeam->GetRating();
                        Battleground::BattlegroundPlayerMap::const_iterator citr = arena->GetPlayers().begin();
                        for (; citr != arena->GetPlayers().end(); ++citr)
                            if (Player* plrs = ObjectAccessor::FindPlayer(citr->first))
                                if (plrs->GetArenaTeamId(2) != firstTeamId)
                                {
                                    mmrTwo = arena->GetArenaMatchmakerRating(citr->second->GetTeamId());
                                    ArenaTeam* secondTeam = sArenaTeamMgr->GetArenaTeamById(plrs->GetArenaTeamId(2));
                                    if (secondTeam) mmrTwo = secondTeam->GetRating();
                                }
                    }


                    if (IsTop == ARENA_TYPE_2v2 && arena->GetArenaType() == ARENA_TYPE_2v2)
                    {
                        TypeOne++;
                        if (TypeOne > (page + 1) * GamesOnPage)
                        {
                            haveNextPage = true;
                            break;
                        }

                        if (TypeOne >= page * GamesOnPage)
                            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, GetGamesStringData(arena, mmr, mmrTwo), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena));
                    }
                    else if (IsTop == ARENA_TYPE_3v3 && arena->GetArenaType() == ARENA_TYPE_3v3)
                    {
                        TypeTwo++;
                        if (TypeTwo > (page + 1) * GamesOnPage)
                        {
                            haveNextPage = true;
                            break;
                        }

                        if (TypeTwo >= page * GamesOnPage)
                            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, GetGamesStringData(arena, mmr, mmrTwo), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena));
                    }
                    /* Cuando haya soloq */
                    /*else if (IsTop == ARENA_TYPE_5v5 && arena->GetArenaType() == ARENA_TYPE_3v3_SOLO)*/
                    else if (IsTop == ARENA_TYPE_5v5 && arena->GetArenaType() == ARENA_TYPE_5v5)
                    {
                        TypeThree++;
                        if (TypeThree > (page + 1) * GamesOnPage)
                        {
                            haveNextPage = true;
                            break;
                        }
                        if (TypeThree >= page * GamesOnPage)
                            AddGossipItemFor(player, GOSSIP_ICON_BATTLE, GetGamesStringData(arena, mmr, mmrTwo), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + GetFirstPlayerGuid(arena));
                    }
                }
            }
        }

        if (IsTop == 0) //esto va a ser el buscar players
        {
            // Sort names using std::sort
            std::sort(vstrPlayers.begin(), vstrPlayers.end());

            for (std::vector<std::string>::const_iterator itr = vstrPlayers.begin(); itr != vstrPlayers.end(); ++itr)
            {
                if (Player* target = ObjectAccessor::FindPlayerByName(itr->c_str()))
                {
                    TypePlayer++;
                    if (TypePlayer > (page + 1) * GamesOnPage)
                    {
                        haveNextPage = true;

                    }
                    uint32 testGuid = target->GetGUID();
                    if (TypePlayer >= page * GamesOnPage)
                        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, target->GetName() + "(" + GetClassNameById(target) + ")", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + target->GetGUID());
                }
            }
        }

        bool isSpanish = IsSpanishPlayer(player);
        if (page > 0)
        {
            if (type == NPC_SPECTATOR_ACTION_2V2_GAMES)
                AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Previo.." : "Previous..", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES + page - 1);
            else if (type == NPC_SPECTATOR_ACTION_3V3_GAMES)
                AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Previo.." : "Previous..", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES + page - 1);
            else if (type == NPC_SPECTATOR_ACTION_3V3S_GAMES)
                AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Previo.." : "Previous..", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3S_GAMES + page - 1);
            else if (type == NPC_SPECTATOR_ACTION_SPECIFIC)
                AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Previo.." : "Previous..", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SPECIFIC + page - 1);
        }

        if (haveNextPage)
        {
            if (type == NPC_SPECTATOR_ACTION_2V2_GAMES)
                AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Siguiente.." : "Next..", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES + page + 1);
            else if (type == NPC_SPECTATOR_ACTION_3V3_GAMES)
                AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Siguiente.." : "Next..", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES + page + 1);
            else if (type == NPC_SPECTATOR_ACTION_3V3S_GAMES)
                AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Siguiente.." : "Next..", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3S_GAMES + page + 1);
            else if (type == NPC_SPECTATOR_ACTION_SPECIFIC)
                AddGossipItemFor(player, GOSSIP_ICON_DOT, isSpanish ? "Siguiente.." : "Next..", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SPECIFIC + page + 1);
        }

        if (page == 0 && TypeOne == 0 && TypeTwo == 0 && TypeThree == 0 && TypePlayer == 0)
        {
            return false;
        }
        return true;
    }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    struct MyAI : public ScriptedAI
    {
        MyAI(Creature* m_creature) : ScriptedAI(m_creature) { }
    };

    CreatureAI* GetAI(Creature* m_creature) const override
    {
        return new MyAI(m_creature);
    }
};


void AddSC_arena_spectator_script()
{
    new arena_spectator_commands();
    new npc_arena_spectator();
}
