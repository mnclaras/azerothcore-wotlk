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

#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "Unit.h"
#include "Chat.h"
#include "WorldSession.h"
#include "Creature.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Battleground.h"
#include <random>

enum Spells
{   
    SPELL_FROST_AURA_25 = 55799,
    SPELL_CLEAVE_ARMOR = 74367,
    SPELL_ARCTIC_BREATH = 66689,
    SPELL_TYMPANIC_TANTARUM = 62776,
    SPELL_STOMP_25 = 62413,
    SPELL_FROSTBOLT_VOLLEY = 72905,
    SPELL_BONE_SPIKE_GRAVEYARD = 69057,
    SPELL_SNOWBALL = 21343,
    SPELL_HAMMER_JUSTICE = 66863,
    SPELL_SIF_BLIZZARD = 62576,
    SPELL_OVERLOAD_25 = 63481,
    SPELL_RUNE_OF_DEATH_25 = 63490,
};

enum Events
{
    EVENT_NONE,
    EVENT_BEBENDE_ERDE,
    EVENT_ENRAGE,
    EVENT_BERSERK,
    EVENT_VISUAL_NETHER_PORTAL,
    EVENT_BLADESTORM,
    EVENT_SUMMONS,
    EVENT_SUMMONS_IMMUNE,
    EVENT_SUMMON_HALLOWS,
    EVENT_SUMMON_DRUDGE_GHOUL,
    EVENT_SLIME_SPRAY,
    EVENT_UNROOT,
    EVENT_SPELL_SLIME_POOL,
    EVENT_MORTAL_WOUND,
    EVENT_SPELL_DOMINATE_MIND_25,
    EVENT_BONE_SLICE,
    EVENT_BOMB_SUICIDE,
    EVENT_SPELL_LEGION_FLAME,
    EVENT_SPELL_INCINERATE_FLESH,
    EVENT_SPELL_RADIANCE_LEFT6,
    EVENT_SPELL_RADIANCE_LEFT3,
    EVENT_SPELL_RADIANCE_LEFT2,
    EVENT_SPELL_RADIANCE_LEFT1,
    EVENT_SPELL_RADIANCE,
    EVENT_DECIMATE,
    EVENT_BLISTERING_COLD,
    EVENT_ICY_GRIP,
    EVENT_FREYA_GROUND_TREMOR,
    EVENT_CLEAVE,
    EVENT_SPELL_ARCTIC_BREATH,
    EVENT_TYMPANIC_TANTARUM,
    EVENT_STOMP,
    EVENT_ARG_STOMP,
    EVENT_SPELL_FROSTBOLT_VOLLEY,
    EVENT_SPELL_BONE_SPIKE_GRAVEYARD,
    EVENT_SNOWBALL,
    EVENT_SPELL_SIF_BLIZZARD,
    EVENT_OVERLOAD,
    EVENT_IMMUNE,
    EVENT_RUNE_OF_DEATH
};

enum Phases
{
    PHASE_ONE = 1,
    PHASE_TWO = 2,
    PHASE_THREE = 3,
    PHASE_FOUR = 4
};

enum Summons
{
    NPC_BOSS_BUSY_EVERGREEN_ADD = 00000, // Calabacino Explosivo
    NPC_BOSS_BUSY_EVERGREEN_ADD_MINI = 20000, // Calabacino Explosivo
};

#define DEFAULT_MESSAGE 907

class custom_christmas_event_boss_grinch : public CreatureScript
{
public:
    custom_christmas_event_boss_grinch() : CreatureScript("custom_christmas_event_boss_grinch") { }

    struct custom_christmas_event_boss_grinchAI : public ScriptedAI
    {
        custom_christmas_event_boss_grinchAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            if (!me->IsAlive())
                return;

            _events.Reset();
        }

        void JustDied(Unit* /*killer*/) override
        {
            _events.Reset();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _events.SetPhase(PHASE_ONE);

            me->CastSpell(me, SPELL_FROST_AURA_25, true);
            _events.ScheduleEvent(EVENT_CLEAVE, 5000);

            _events.ScheduleEvent(EVENT_SPELL_ARCTIC_BREATH, 5000, PHASE_ONE);
            _events.ScheduleEvent(EVENT_ARG_STOMP, 15000, PHASE_ONE);
        }

        void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
        {
            if (me->HealthBelowPctDamaged(50, damage) && _events.IsInPhase(PHASE_ONE))
            {
                me->MonsterYell("FASE 2. Me has enfadado!", LANG_UNIVERSAL, 0);
                _events.CancelEventGroup(PHASE_ONE);
                _events.SetPhase(PHASE_TWO);

                _events.ScheduleEvent(EVENT_TYMPANIC_TANTARUM, 5000, PHASE_TWO);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CLEAVE:
                    me->CastSpell(me->GetVictim(), SPELL_CLEAVE_ARMOR, false);
                    _events.ScheduleEvent(EVENT_CLEAVE, 5000);
                    break;
                case EVENT_SPELL_ARCTIC_BREATH:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 90.0f, true))
                        me->CastSpell(target, SPELL_ARCTIC_BREATH, false);
                    _events.ScheduleEvent(EVENT_SPELL_ARCTIC_BREATH, 25000, PHASE_ONE);
                    break;
                case EVENT_ARG_STOMP:
                    me->CastSpell(me->GetVictim(), SPELL_STOMP_25, false);
                    _events.ScheduleEvent(EVENT_ARG_STOMP, 25000, PHASE_ONE);
                    break;
                case EVENT_TYMPANIC_TANTARUM:
                    me->MonsterTextEmote("El Grinch empieza a hacer que la tierra tiemble...", 0, true);
                    me->MonsterYell("NO! NO! NO! NO! NO!", LANG_UNIVERSAL, 0);
                    me->CastSpell(me, SPELL_TYMPANIC_TANTARUM, true);
                    _events.ScheduleEvent(EVENT_TYMPANIC_TANTARUM, 20000, PHASE_TWO);
                    return;
                default:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_christmas_event_boss_grinchAI(creature);
    }
};

class custom_christmas_event_boss_busy_evergreen : public CreatureScript
{
public:
    custom_christmas_event_boss_busy_evergreen() : CreatureScript("custom_christmas_event_boss_busy_evergreen") { }

    struct custom_christmas_event_boss_busy_evergreenAI : public ScriptedAI
    {
        custom_christmas_event_boss_busy_evergreenAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }

        void Reset() override
        {
            if (!me->IsAlive())
                return;

            _events.Reset();
            Summons.DespawnAll();
        }

        void JustDied(Unit* /*killer*/) override
        {
            me->MonsterYell("No me lo puedo...creer...Ahora estas cerca de acabar tu camino!", LANG_UNIVERSAL, 0);
            Summons.DespawnAll();
            _events.Reset();
            UnsummonAliveCreatures(NPC_BOSS_BUSY_EVERGREEN_ADD);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            me->MonsterYell("Que comience el juego...", LANG_UNIVERSAL, 0);

            Summons.DespawnAll();
            _events.Reset();

            _events.SetPhase(PHASE_ONE);
            me->CastSpell(me, SPELL_FROST_AURA_25, true);
            _events.ScheduleEvent(EVENT_SPELL_FROSTBOLT_VOLLEY, 5000, PHASE_FOUR);

            _events.ScheduleEvent(EVENT_SUMMONS, 5000, PHASE_ONE);
        }

        void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask) override
        {
            if (me->HealthBelowPctDamaged(70, damage) && _events.IsInPhase(PHASE_ONE))
            {
                me->MonsterYell("FASE 2. Me estas enfadando!", LANG_UNIVERSAL, 0);
                _events.CancelEventGroup(PHASE_ONE);
                _events.SetPhase(PHASE_TWO);

                _events.ScheduleEvent(EVENT_SNOWBALL, 5000, PHASE_TWO);
                _events.ScheduleEvent(EVENT_SPELL_SIF_BLIZZARD, 4500, PHASE_TWO);
            }

            if (me->HealthBelowPctDamaged(25, damage) && _events.IsInPhase(PHASE_TWO))
            {
                me->MonsterYell("FASE 3. No voy a permitir que os salgais con la vuestra!", LANG_UNIVERSAL, 0);
                _events.CancelEventGroup(PHASE_TWO);
                _events.CancelEventGroup(PHASE_FOUR);
                _events.SetPhase(PHASE_THREE);

                _events.ScheduleEvent(EVENT_OVERLOAD, 5000, PHASE_THREE);
                _events.ScheduleEvent(EVENT_RUNE_OF_DEATH, 3000, PHASE_THREE);
            }
        }

        void JustSummoned(Creature* summon) override
        {
            Summons.Summon(summon);

            switch (summon->GetEntry())
            {
            case NPC_BOSS_BUSY_EVERGREEN_ADD:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                    summon->AI()->AttackStart(target);
                break;
            default:
                break;
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit*) override
        {
            switch (summon->GetEntry())
            {
            case NPC_BOSS_BUSY_EVERGREEN_ADD:
                summon->ToTempSummon()->SetTimer(5000);
                summon->ToTempSummon()->SetTempSummonType(TEMPSUMMON_TIMED_DESPAWN);
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;


            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SPELL_FROSTBOLT_VOLLEY:
                    me->CastSpell((Unit*)nullptr, SPELL_FROSTBOLT_VOLLEY, false);
                    _events.ScheduleEvent(EVENT_SPELL_FROSTBOLT_VOLLEY, 12000);
                    break;
                case EVENT_SUMMONS:
                    me->MonsterYell("Venid esbirros...", LANG_UNIVERSAL, 0);
                    me->SummonCreature(NPC_BOSS_BUSY_EVERGREEN_ADD, me->GetPositionX() + 5, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 600000);
                    _events.ScheduleEvent(EVENT_SUMMONS, 15000, PHASE_ONE);
                    break;
                case EVENT_SNOWBALL:
                {
                    std::vector<Player*> validPlayers;
                    Map::PlayerList const& pList = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                        if (Player* plr = itr->GetSource())
                            if (plr->IsAlive() && !plr->IsGameMaster() && plr->GetExactDist2dSq(me) < (150.0f * 150.0f))
                                if (!me->GetVictim() || me->GetVictim()->GetGUID() != plr->GetGUID())
                                    validPlayers.push_back(plr);

                    std::vector<Player*>::iterator begin = validPlayers.begin(), end = validPlayers.end();

                    std::random_device rd;
                    std::shuffle(begin, end, std::default_random_engine{ rd() });

                    for (uint8 i = 0; i < 2 && i < validPlayers.size(); i++)
                    {
                        Unit* target = validPlayers[i];
                        me->CastSpell(me->GetVictim(), SPELL_SNOWBALL, false);
                    }

                    _events.ScheduleEvent(EVENT_SNOWBALL, 10000, PHASE_TWO);
                }
                break;
                case EVENT_SPELL_SIF_BLIZZARD:
                    me->CastSpell(me->GetVictim(), SPELL_SIF_BLIZZARD, false);
                    _events.ScheduleEvent(EVENT_SPELL_SIF_BLIZZARD, 15000, PHASE_TWO);
                    break;
                case EVENT_OVERLOAD:
                    me->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                    me->CastSpell(me, SPELL_OVERLOAD_25, true);
                    _events.ScheduleEvent(EVENT_OVERLOAD, 25000, PHASE_THREE);
                    _events.ScheduleEvent(EVENT_IMMUNE, 5999, PHASE_THREE);
                    break;
                case EVENT_IMMUNE:
                    me->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
                    break;
                case EVENT_RUNE_OF_DEATH:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        me->CastSpell(target, SPELL_RUNE_OF_DEATH_25, true);
                    _events.ScheduleEvent(EVENT_RUNE_OF_DEATH, 25000, PHASE_THREE);
                    break;
                default:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

        void EnterEvadeMode() override
        {
            me->SetControlled(false, UNIT_STATE_ROOT);
            me->DisableRotate(false);
            ScriptedAI::EnterEvadeMode();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell) override
        {
            switch (spell->Id)
            {
            case SPELL_SNOWBALL:
                me->CastSpell(target, SPELL_HAMMER_JUSTICE, true);
                break;
            default:
                break;
            }
        }

        void UnsummonAliveCreatures(uint32 creatureEntry)
        {
            std::list<Creature*> unitList;
            me->GetCreaturesWithEntryInRange(unitList, 200.0f, creatureEntry);
            for (std::list<Creature*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
                (*itr)->ToCreature()->DespawnOrUnsummon(500);
        }

    private:
        EventMap _events;
        SummonList Summons;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_christmas_event_boss_busy_evergreenAI(creature);
    }
};

class custom_christmas_event_boss_busy_evergreen_add : public CreatureScript
{
public:
    custom_christmas_event_boss_busy_evergreen_add() : CreatureScript("custom_christmas_event_boss_busy_evergreen_add") { }

    struct custom_christmas_event_boss_busy_evergreen_addAI : public ScriptedAI
    {
        custom_christmas_event_boss_busy_evergreen_addAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }

        void Reset() override
        {
            if (!me->IsAlive())
                return;

            _events.Reset();
            Summons.DespawnAll();
        }

        void JustDied(Unit* /*killer*/) override
        {
            Summons.DespawnAll();
            _events.Reset();
            UnsummonAliveCreatures(NPC_BOSS_BUSY_EVERGREEN_ADD_MINI);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            me->MonsterYell("Que comience el juego...", LANG_UNIVERSAL, 0);

            Summons.DespawnAll();
            _events.Reset();
            _events.ScheduleEvent(EVENT_SUMMONS, 500);
        }

        void JustSummoned(Creature* summon) override
        {
            Summons.Summon(summon);

            switch (summon->GetEntry())
            {
            case NPC_BOSS_BUSY_EVERGREEN_ADD_MINI:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                    summon->AI()->AttackStart(target);
                break;
            default:
                break;
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit*) override
        {
            switch (summon->GetEntry())
            {
            case NPC_BOSS_BUSY_EVERGREEN_ADD_MINI:
                summon->ToTempSummon()->SetTimer(5000);
                summon->ToTempSummon()->SetTempSummonType(TEMPSUMMON_TIMED_DESPAWN);
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SUMMONS:
                    me->SummonCreature(NPC_BOSS_BUSY_EVERGREEN_ADD_MINI, me->GetPositionX() + 5, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 600000);
                    _events.ScheduleEvent(EVENT_SUMMONS, 3000);
                    break;
                default:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

        void UnsummonAliveCreatures(uint32 creatureEntry)
        {
            std::list<Creature*> unitList;
            me->GetCreaturesWithEntryInRange(unitList, 200.0f, creatureEntry);
            for (std::list<Creature*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
                (*itr)->ToCreature()->DespawnOrUnsummon(500);
        }

    private:
        EventMap _events;
        SummonList Summons;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_christmas_event_boss_busy_evergreen_addAI(creature);
    }
};

class custom_christmas_event_teleporter_grinch : public CreatureScript
{
public:
    custom_christmas_event_teleporter_grinch() : CreatureScript("custom_christmas_event_teleporter_grinch") { }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER,
            isSpanish ? "Llevame con la bestia." : "Take me to the beast.",
            GOSSIP_SENDER_MAIN, 2,
            isSpanish ? "Deseas ir a combatir a la bestia?" : "Do you want to face the beast?",
            0, false);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case 2:
                player->TeleportTo(568, 1142.510376f, 1931.967773f, 0.560032f, 3.183893f);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

    struct custom_christmas_event_teleporter_grinchAI : public ScriptedAI
    {
        custom_christmas_event_teleporter_grinchAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_christmas_event_teleporter_grinchAI(creature);
    }
};

class custom_christmas_event_teleporter_rebels : public CreatureScript
{
public:
    custom_christmas_event_teleporter_rebels() : CreatureScript("custom_christmas_event_teleporter_rebels") { }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER,
            isSpanish ? "Llevame con los rebeldes." : "Take me to the rebels.",
            GOSSIP_SENDER_MAIN, 2,
            isSpanish ? "Deseas ir a combatir a los rebeldes?" : "Do you want to fight against the rebels?",
            0, false);

        AddGossipItemFor(player, GOSSIP_ICON_TRAINER,
            isSpanish ? "Llevame con los capitanes." : "Take me to the captains.",
            GOSSIP_SENDER_MAIN, 3,
            isSpanish ? "Deseas ir a combatir a los capitanes?" : "Do you want to fight against the captains?",
            0, false);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case 2:
                player->TeleportTo(575, 89.12249f, 550.664001f, -208.959351f, 2.648375f);
                CloseGossipMenuFor(player);
                break;
            case 3:
                player->TeleportTo(575, 1606.062134f, -291.932678f, -80.986588f, 0.341667f);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

    struct custom_christmas_event_teleporter_rebelsAI : public ScriptedAI
    {
        custom_christmas_event_teleporter_rebelsAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_christmas_event_teleporter_rebelsAI(creature);
    }
};

class custom_christmas_event_teleporter_busy_evergreen : public CreatureScript
{
public:
    custom_christmas_event_teleporter_busy_evergreen() : CreatureScript("custom_christmas_event_teleporter_busy_evergreen") { }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER,
            isSpanish ? "Llevame a Busy Evergreen." : "Take me to Busy Evergreen.",
            GOSSIP_SENDER_MAIN, 2,
            isSpanish ? "Deseas ir a combatir a Busy Evergreen?" : "Do you want to face Busy Evergreen?",
            0, false);
        SendGossipMenuFor(player, DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        if (sender == GOSSIP_SENDER_MAIN)
        {
            switch (action)
            {
            case 2:
                player->TeleportTo(533, 3970.246582f, -3714.073730f, 46.144012, 1.606889f);
                CloseGossipMenuFor(player);
                break;
            }
        }

        return true;
    }

    struct custom_christmas_event_teleporter_busy_evergreenAI : public ScriptedAI
    {
        custom_christmas_event_teleporter_busy_evergreenAI(Creature* creature) : ScriptedAI(creature) { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_christmas_event_teleporter_busy_evergreenAI(creature);
    }
};

void AddSC_custom_christmas_event()
{
    new custom_christmas_event_teleporter_grinch();
    new custom_christmas_event_teleporter_rebels();
    new custom_christmas_event_teleporter_busy_evergreen();
    new custom_christmas_event_boss_grinch();
    new custom_christmas_event_boss_busy_evergreen();
    new custom_christmas_event_boss_busy_evergreen_add();
}
