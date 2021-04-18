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

#include "AccountMgr.h"
#include "ArenaTeamMgr.h"
#include "Common.h"
#include "DisableMgr.h"
#include "BattlegroundMgr.h"
#include "ArenaTeam.h"
#include "Language.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "WorldSession.h"
#include "Log.h"
#include "Chat.h"
#include "Common.h"
#include "DisableMgr.h"
#include "DBCStores.h" 
#include "ScriptMgr.h"
#include "World.h"
#include "Group.h"
#include "GroupMgr.h"
#include "GameObjectAI.h"
#include "Player.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "Unit.h"


struct PvPIslandInfoKills
{
    uint32 killCount;
};
struct PvPIslandInfoHealing
{
    uint32 healingAmount;
    uint32 currentHealingStreak;
};

enum ModBgAurasAvailable
{
    AURA_KILLS_5 = 36151,
    AURA_KILLS_10 = 45576,
    AURA_KILLS_15 = 28330,
    AURA_KILLS_20 = 16003,
    AURA_KILLS_25 = 75041,
    AURA_KILLS_30 = 51201,
    AURA_HEALING_100000 = 62011,
    AURA_HEALING_200000 = 56740,
    AURA_HEALING_300000 = 53143,
    AURA_HEALING_400000 = 58712,
    AURA_HEALING_500000 = 52952,
    AURA_HEALING_600000 = 43312,
    MAX_HEALING_AMOUNT = 600000,
    MAX_KILLS_AMOUNT = 30
};

static const std::set<uint32> battlegroundZoneIds{
    3277, // Warsong Gulch
    3358, // Arathi Basin
    3820, // Eye of the Storm
    4710, // Isle of Conquest
    4384, // Strand of the Ancients
    2597 // Alterac Valley
};

static std::map<uint32, PvPIslandInfoKills> KillingStreak;
static std::map<uint32, PvPIslandInfoHealing> HealingStreak;

class mod_bg_auras_kill : public PlayerScript
{
public:
    mod_bg_auras_kill() : PlayerScript("mod_bg_auras_kill") { }

    uint64 killerGUID = 0;
    uint64 victimGUID = 0;
    uint32 killerZoneId = 0;
    uint32 victimZoneId = 0;

    void OnLogout(Player* player)
    {
        ResetStreakAndAuras(player, true);
    }

    // When player Battleground and his GUID was stored as the GUID of a killer, the killcount will be reset
    void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/)
    {
        if (battlegroundZoneIds.find(newZone) == battlegroundZoneIds.end())
        {
            ResetStreakAndAuras(player, true);
        }
        else
        {
            std::map<uint32, PvPIslandInfoKills>::iterator killingStreak_itr = KillingStreak.find(player->GetGUID());
            std::map<uint32, PvPIslandInfoHealing>::iterator healingStreak_itr = HealingStreak.find(player->GetGUID());

            if (killingStreak_itr != KillingStreak.end())
            {
                // Check if any aura should be applied
                uint32 killCount = killingStreak_itr->second.killCount;
                if (killCount >= 5)
                {
                    RemoveKillingStreakAuras(player);

                    if      (killCount >= 30)   AddKillingStreakAura(player, 30);
                    else if (killCount >= 25)   AddKillingStreakAura(player, 25);
                    else if (killCount >= 20)   AddKillingStreakAura(player, 20);
                    else if (killCount >= 15)   AddKillingStreakAura(player, 15);
                    else if (killCount >= 10)   AddKillingStreakAura(player, 10);
                    else if (killCount >= 5)    AddKillingStreakAura(player, 5);
                }
            }

            if (healingStreak_itr != HealingStreak.end())
            {
                // Check if any aura should be applied
                uint32 currentStreak = healingStreak_itr->second.currentHealingStreak;
                if (currentStreak >= 100000)
                {
                    RemoveHealingStreakAuras(player);
                    AddHealingStreakAura(player, currentStreak);
                }
            }
        }
    }

    void OnPVPKill(Player* killer, Player* victim)
    {
        killerGUID = killer->GetGUID();
        victimGUID = victim->GetGUID();

        // If player killed himself, do not execute any code(think of when a warlock uses Hellfire, when player falls to dead, etc.)
        if (killerGUID == victimGUID)
        {
            ResetStreakAndAuras(killer, false);
            return;
        }

        killerZoneId = killer->GetZoneId();
        victimZoneId = victim->GetZoneId();

        if (battlegroundZoneIds.find(killerZoneId) != battlegroundZoneIds.end() && battlegroundZoneIds.find(victimZoneId) != battlegroundZoneIds.end())
        {
            KillingStreak[killerGUID].killCount++;          // Increment kill count by one on every kill

            ResetStreakAndAuras(victim, false);

            // If killcount is 5, 10, 15, 20, 25, 30
            if ((KillingStreak[killerGUID].killCount % 5) == 0 && KillingStreak[killerGUID].killCount <= MAX_KILLS_AMOUNT)
            {
                RemoveKillingStreakAuras(killer);
                AddKillingStreakAura(killer, KillingStreak[killerGUID].killCount);
                std::stringstream text;
                text << "|cffff6060[BG Killing Streak]:|r " << killer->GetName() << " lleva una racha de asesinatos de |cFFFF4500" << std::to_string(KillingStreak[killerGUID].killCount) << "|r!";
                sWorld->SendZoneText(killerZoneId, text.str().c_str());
            }
        }
    }

    static void RemoveKillingStreakAuras(Player* player)
    {
        player->RemoveAurasDueToSpell(AURA_KILLS_5);
        player->RemoveAurasDueToSpell(AURA_KILLS_10);
        player->RemoveAurasDueToSpell(AURA_KILLS_15);
        player->RemoveAurasDueToSpell(AURA_KILLS_20);
        player->RemoveAurasDueToSpell(AURA_KILLS_25);
        player->RemoveAurasDueToSpell(AURA_KILLS_30);
    }

    static void AddKillingStreakAura(Player* player, uint32 streakCount)
    {
        uint32 auraToApply = 0;
        switch (streakCount)
        {
        case 5:     auraToApply = AURA_KILLS_5;  break;
        case 10:    auraToApply = AURA_KILLS_10; break;
        case 15:    auraToApply = AURA_KILLS_15; break;
        case 20:    auraToApply = AURA_KILLS_20; break;
        case 25:    auraToApply = AURA_KILLS_25; break;
        case 30:    auraToApply = AURA_KILLS_30; break;
        }

        if (auraToApply && auraToApply > 0)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(auraToApply);
            if (spellInfo)
            {
                Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, player, player);
            }
        }
    }

    static void RemoveHealingStreakAuras(Player* player)
    {
        player->RemoveAurasDueToSpell(AURA_HEALING_100000);
        player->RemoveAurasDueToSpell(AURA_HEALING_200000);
        player->RemoveAurasDueToSpell(AURA_HEALING_300000);
        player->RemoveAurasDueToSpell(AURA_HEALING_400000);
        player->RemoveAurasDueToSpell(AURA_HEALING_500000);
        player->RemoveAurasDueToSpell(AURA_HEALING_600000);
    }

    static void ResetStreakAndAuras(Player* player, bool onlyAuras)
    {
        if (KillingStreak[player->GetGUID()].killCount)
        {
            if (!onlyAuras)
            {
                KillingStreak[player->GetGUID()].killCount = 0;
            }
            RemoveKillingStreakAuras(player);
        }
        if (HealingStreak[player->GetGUID()].healingAmount)
        {
            if (!onlyAuras)
            {
                HealingStreak[player->GetGUID()].healingAmount = 0;
                HealingStreak[player->GetGUID()].currentHealingStreak = 0;
            }
            RemoveHealingStreakAuras(player);
        }
    }

    static void AddHealingStreakAura(Unit* player, uint32 healingStreak)
    {
        uint32 auraToApply = 0;
        switch (healingStreak)
        {
        case 100000:    auraToApply = AURA_HEALING_100000; break;
        case 200000:    auraToApply = AURA_HEALING_200000; break;
        case 300000:    auraToApply = AURA_HEALING_300000; break;
        case 400000:    auraToApply = AURA_HEALING_400000; break;
        case 500000:    auraToApply = AURA_HEALING_500000; break;
        case 600000:    auraToApply = AURA_HEALING_600000; break;
        }

        if (auraToApply && auraToApply > 0)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(auraToApply);
            if (spellInfo)
            {
                Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, player, player);
            }
        }
    }
};

class mod_bg_auras_heal : public UnitScript
{
public:
    mod_bg_auras_heal() : UnitScript("mod_bg_auras_heal") { }

    uint64 healerGUID = 0;
    uint32 healerZoneId = 0;
    uint32 healedZoneId = 0;
    uint32 healingAmount = 0;
    uint32 currentHealingStreak = 0;

    void OnHeal(Unit* healer, Unit* reciever, uint32& gain)
    {
        if (!gain || !healer)
            return;

        // If it's for example a (bugged) area-heal that also heals enemies we should not count this for the quest
        if (!reciever->IsFriendlyTo(healer))
            return;

        healerZoneId = healer->GetZoneId();
        healedZoneId = reciever->GetZoneId();

        if (battlegroundZoneIds.find(healerZoneId) != battlegroundZoneIds.end() && battlegroundZoneIds.find(healedZoneId) != battlegroundZoneIds.end())
        {
            if (reciever->IsPet() || reciever->GetTypeId() == TYPEID_PLAYER)
            {
                healerGUID = healer->GetGUID();
                HealingStreak[healerGUID].healingAmount += gain;

                healingAmount = HealingStreak[healerGUID].healingAmount;
                currentHealingStreak = HealingStreak[healerGUID].currentHealingStreak;

                if ((healingAmount < 100000) || (healingAmount > MAX_HEALING_AMOUNT && currentHealingStreak == MAX_HEALING_AMOUNT))
                    return;

                // If healingAmount is 100k, 200k, 300k, 400k, 500k, 600k
                if (healingAmount >= 100000 && currentHealingStreak < 100000)
                {
                    SetHealingStreakAura(healer, 100000); HealingStreak[healerGUID].currentHealingStreak = 100000;
                }
                else if (healingAmount >= 200000 && currentHealingStreak < 200000)
                {
                    SetHealingStreakAura(healer, 200000); HealingStreak[healerGUID].currentHealingStreak = 200000;
                }
                else if (healingAmount >= 300000 && currentHealingStreak < 300000)
                {
                    SetHealingStreakAura(healer, 300000); HealingStreak[healerGUID].currentHealingStreak = 300000;
                }
                else if (healingAmount >= 400000 && currentHealingStreak < 400000)
                {
                    SetHealingStreakAura(healer, 400000); HealingStreak[healerGUID].currentHealingStreak = 400000;
                }
                else if (healingAmount >= 500000 && currentHealingStreak < 500000)
                {
                    SetHealingStreakAura(healer, 500000); HealingStreak[healerGUID].currentHealingStreak = 500000;
                }
                else if (healingAmount >= 600000 && currentHealingStreak < 600000)
                {
                    SetHealingStreakAura(healer, 600000); HealingStreak[healerGUID].currentHealingStreak = 600000;
                }
            }
        }
    }

    static void RemoveHealingStreakAuras(Unit* player)
    {
        player->RemoveAurasDueToSpell(AURA_HEALING_100000);
        player->RemoveAurasDueToSpell(AURA_HEALING_200000);
        player->RemoveAurasDueToSpell(AURA_HEALING_300000);
        player->RemoveAurasDueToSpell(AURA_HEALING_400000);
        player->RemoveAurasDueToSpell(AURA_HEALING_500000);
        player->RemoveAurasDueToSpell(AURA_HEALING_600000);
    }

    static void AddHealingStreakAura(Unit* player, uint32 healingStreak)
    {
        uint32 auraToApply = 0;
        switch (healingStreak)
        {
        case 100000:    auraToApply = AURA_HEALING_100000; break;
        case 200000:    auraToApply = AURA_HEALING_200000; break;
        case 300000:    auraToApply = AURA_HEALING_300000; break;
        case 400000:    auraToApply = AURA_HEALING_400000; break;
        case 500000:    auraToApply = AURA_HEALING_500000; break;
        case 600000:    auraToApply = AURA_HEALING_600000; break;
        }

        if (auraToApply && auraToApply > 0)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(auraToApply);
            if (spellInfo)
            {
                Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, player, player);
            }
        }
    }

    static void SetHealingStreakAura(Unit* healer, uint32 streakAmount)
    {
        RemoveHealingStreakAuras(healer);
        AddHealingStreakAura(healer, streakAmount);
        std::stringstream text;
        text << "|cffff6060[BG Healing Streak]:|r " << healer->GetName() << " lleva una racha de sanación de |cFFFF4500" << std::to_string(streakAmount) << "|r!";
        sWorld->SendZoneText(healer->GetZoneId(), text.str().c_str());
    }
};


void AddSC_mod_bg_auras()
{
    new mod_bg_auras_kill();
    new mod_bg_auras_heal();
}
