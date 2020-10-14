/*
* Copyright (C) Amnesyx
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
#include <random>

enum Spells
{
    SPELL_BEBENDE_ERDE = 6524,
    SPELL_BLADESTORM = 9632,
    SPELL_BERSERK = 26662,
    SPELL_ENRAGE = 64487,
    SPELL_SLIME_SPRAY = 69508,
    VISUAL_NETHER_PORTAL = 30490,
    SPELL_SUMMON_DRUDGE_GHOULS = 70358,
    SPELL_SLIME_POOL_EFFECT = 66882,
    SPELL_MUTATED_PLAGUE = 72451,
    SPELL_MUTATED_PLAGUE_CLEAR = 72618,
    SPELL_MORTAL_WOUND = 71127,
    SPELL_DOMINATE_MIND_25 = 71289,
    SPELL_BONE_SLICE = 69055,
    SPELL_BOMB_SUICIDE = 71088,
    SPELL_LEGION_FLAME = 66197,
    SPELL_INCINERATE_FLESH = 66237,
    SPELL_RADIANCE = 66935,
    SPELL_DECIMATE = 71123,
    SPELL_DIVINE_SURGE = 71465,
    SPELL_BLISTERING_COLD = 70123,
    SPELL_ICY_GRIP = 70117,
    SPELL_ICY_GRIP_JUMP = 70122,
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
    EVENT_SPELL_RADIANCE,
    EVENT_DECIMATE,
    EVENT_BLISTERING_COLD,
    EVENT_ICY_GRIP
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
    NPC_DRUDGE_GHOUL = 37695,
    NPC_OOZE_SPRAY_STALKER = 37986,
    NPC_SLIME_POOL = 35176,
    NPC_BOSS_TWO_ADD = 2100002 // Calabacino Explosivo
};

#define TEXT_RADIATE "Se esta empezando a radiar luz. Tapate los ojos!"

class custom_hallows_end_event_boss_one : public CreatureScript
{
public:
    custom_hallows_end_event_boss_one() : CreatureScript("custom_hallows_end_event_boss_one") { }

    struct custom_hallows_end_event_boss_oneAI : public ScriptedAI
    {
        custom_hallows_end_event_boss_oneAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }

        // OK
        void Reset() override
        {
            if (!me->IsAlive())
                return;

            _events.Reset();
            Summons.DespawnAll();
        }

        // OK
        void JustDied(Unit* /*killer*/) override
        {
            me->MonsterYell("No me lo puedo...creer...Has ganado esta vez, pero todavia te queda camino!", LANG_UNIVERSAL, 0);
            Summons.DespawnAll();
            UnsummonAliveCreatures(NPC_DRUDGE_GHOUL);
            _events.Reset();
        }

        // OK
        void EnterCombat(Unit* /*who*/) override
        {
            me->MonsterYell("Que comience el juego...", LANG_UNIVERSAL, 0);

            _events.SetPhase(PHASE_ONE);
            _events.ScheduleEvent(EVENT_VISUAL_NETHER_PORTAL, 1000);
            _events.ScheduleEvent(EVENT_BEBENDE_ERDE, 12000);
        }

        void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask)
        {
            if (me->HealthBelowPctDamaged(75, damage) && _events.IsInPhase(PHASE_ONE))
            {
                _events.SetPhase(PHASE_TWO);
                _events.ScheduleEvent(EVENT_BLADESTORM, 10000);
            }

            if (me->HealthBelowPctDamaged(45, damage) && _events.IsInPhase(PHASE_TWO))
            {
                _events.SetPhase(PHASE_THREE);
                //_events.ScheduleEvent(EVENT_SUMMON_DRUDGE_GHOUL, 10000);
                _events.ScheduleEvent(EVENT_BERSERK, 150000, PHASE_THREE);   // Berserk starts 2.5 minutes after phase 3 begins. 
                _events.ScheduleEvent(EVENT_ENRAGE, 180000, PHASE_THREE);    // In phase 3, 3 minutes to kill boss or it's wipe.
            }
        }

        void JustSummoned(Creature* summon) override
        {
            Summons.Summon(summon);
        }

        void SummonedCreatureDies(Creature* summon, Unit*)
        {
            switch (summon->GetEntry())
            {
            case NPC_DRUDGE_GHOUL:
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
                case EVENT_VISUAL_NETHER_PORTAL:
                    DoCast(me, VISUAL_NETHER_PORTAL);
                    break;
                case EVENT_BEBENDE_ERDE:
                    DoCastVictim(SPELL_BEBENDE_ERDE);
                    _events.ScheduleEvent(EVENT_BEBENDE_ERDE, 8000);
                    break;
                case EVENT_BLADESTORM:
                    DoCastVictim(SPELL_BLADESTORM);
                    _events.ScheduleEvent(EVENT_BLADESTORM, 20000);
                    break;
                case EVENT_SUMMON_DRUDGE_GHOUL:
                    me->MonsterYell("Venid esbirros...", LANG_UNIVERSAL, 0);
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                        me->CastSpell(target, SPELL_SUMMON_DRUDGE_GHOULS, false);
                    _events.ScheduleEvent(EVENT_SUMMON_DRUDGE_GHOUL, 25000);
                    break;
                case EVENT_BERSERK:
                    me->MonsterYell("Me has puesto furioso!", LANG_UNIVERSAL, 0);
                    DoCast(me, SPELL_BERSERK);
                    _events.ScheduleEvent(EVENT_BERSERK, 150000);
                    break;
                case EVENT_ENRAGE:
                    me->MonsterYell("Se acabo!", LANG_UNIVERSAL, 0);
                    DoCast(me, SPELL_ENRAGE);
                    _events.ScheduleEvent(EVENT_ENRAGE, 180000);
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
        return new custom_hallows_end_event_boss_oneAI(creature);
    }
};

class custom_hallows_end_event_boss_two : public CreatureScript
{
public:
    custom_hallows_end_event_boss_two() : CreatureScript("custom_hallows_end_event_boss_two") { }

    struct custom_hallows_end_event_boss_twoAI : public ScriptedAI
    {
        custom_hallows_end_event_boss_twoAI(Creature* creature) : ScriptedAI(creature), Summons(me) { }

        // OK
        void Reset() override
        {
            if (!me->IsAlive())
                return;

            _events.Reset();
            Summons.DespawnAll();
        }

        // OK
        void JustDied(Unit* /*killer*/) override
        {
            me->MonsterYell("No me lo puedo...creer...Ahora estas cerca de acabar tu camino!", LANG_UNIVERSAL, 0);
            Summons.DespawnAll();
            _events.Reset();
            UnsummonAliveCreatures(NPC_BOSS_TWO_ADD);
            //me->CastSpell((Unit*)NULL, SPELL_MUTATED_PLAGUE_CLEAR, true);
        }

        // OK
        void EnterCombat(Unit* /*who*/) override
        {
            me->MonsterYell("Que comience el juego...", LANG_UNIVERSAL, 0);

            _events.SetPhase(PHASE_ONE);

            _events.ScheduleEvent(EVENT_MORTAL_WOUND, urand(4500, 7000));
            _events.ScheduleEvent(EVENT_SPELL_SLIME_POOL, 10000, PHASE_ONE);
            _events.ScheduleEvent(EVENT_SLIME_SPRAY, 15000, PHASE_ONE);
            DoCastSelf(SPELL_DIVINE_SURGE, true);
        }

        void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask)
        {
            if (me->HealthBelowPctDamaged(70, damage) && _events.IsInPhase(PHASE_ONE))
            {
                me->MonsterYell("FASE 2. Me estas enfadando!", LANG_UNIVERSAL, 0);
                _events.SetPhase(PHASE_TWO);
                _events.ScheduleEvent(EVENT_ICY_GRIP, 2000, PHASE_TWO);
                _events.ScheduleEvent(EVENT_SUMMONS, 5000, PHASE_TWO);
                _events.ScheduleEvent(EVENT_SPELL_DOMINATE_MIND_25, 15000, PHASE_TWO);
            }

            if (me->HealthBelowPctDamaged(35, damage) && _events.IsInPhase(PHASE_TWO))
            {
                me->MonsterYell("FASE 3. No voy a permitir que os salgais con la vuestra!", LANG_UNIVERSAL, 0);
                _events.SetPhase(PHASE_THREE);
                _events.ScheduleEvent(EVENT_ICY_GRIP, 2000, PHASE_THREE);
                _events.ScheduleEvent(EVENT_SPELL_LEGION_FLAME, 5000, PHASE_THREE);
                _events.ScheduleEvent(EVENT_SPELL_INCINERATE_FLESH, 12000, PHASE_THREE);
            }

            if (me->HealthBelowPctDamaged(10, damage) && _events.IsInPhase(PHASE_THREE))
            {
                me->MonsterYell("FASE 4. Vais a morir todos!", LANG_UNIVERSAL, 0);
                _events.SetPhase(PHASE_FOUR);
                _events.ScheduleEvent(EVENT_ICY_GRIP, 2000, PHASE_FOUR);
                _events.ScheduleEvent(EVENT_SPELL_RADIANCE, 5000, PHASE_FOUR);
                _events.ScheduleEvent(EVENT_DECIMATE, 7000, PHASE_FOUR);
            }
        }

        void JustSummoned(Creature* summon) override
        {
            Summons.Summon(summon);

            switch (summon->GetEntry())
            {
            case NPC_BOSS_TWO_ADD:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                    summon->AI()->AttackStart(target);
                break;
            default:
                break;
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit*)
        {
            switch (summon->GetEntry())
            {
            case NPC_BOSS_TWO_ADD:
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
                    me->MonsterYell("Venid esbirros...", LANG_UNIVERSAL, 0);
                    me->SummonCreature(NPC_BOSS_TWO_ADD, me->GetPositionX() + 5, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 600000);
                    _events.ScheduleEvent(EVENT_SUMMONS, 30000);
                    break;
                case EVENT_UNROOT:
                    me->SetControlled(false, UNIT_STATE_ROOT);
                    me->DisableRotate(false);
                    break;
                case EVENT_SLIME_SPRAY:
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                    {
                        if (Creature* c = me->SummonCreature(NPC_OOZE_SPRAY_STALKER, *target, TEMPSUMMON_TIMED_DESPAWN, 8000))
                        {
                            me->SetOrientation(me->GetAngle(c));
                            me->SetControlled(true, UNIT_STATE_ROOT);
                            me->DisableRotate(true);
                            me->SetFacingTo(me->GetAngle(c));
                            me->SendMovementFlagUpdate();
                            me->CastSpell(c, SPELL_SLIME_SPRAY, false);
                        }
                    }
                    _events.DelayEvents(1);
                    _events.ScheduleEvent(EVENT_SLIME_SPRAY, 25000);
                    _events.ScheduleEvent(EVENT_UNROOT, 0);
                }
                break;
                case EVENT_SPELL_SLIME_POOL:
                    if (Creature* c = me->SummonCreature(NPC_SLIME_POOL, *me, TEMPSUMMON_TIMED_DESPAWN, 30000))
                        c->CastSpell(c, SPELL_SLIME_POOL_EFFECT, true);
                    _events.ScheduleEvent(EVENT_SPELL_SLIME_POOL, 10000);
                    break;
                case EVENT_MORTAL_WOUND:
                    me->CastSpell(me->GetVictim(), SPELL_MORTAL_WOUND, false);
                    _events.ScheduleEvent(EVENT_MORTAL_WOUND, urand(4500, 7000));
                    break;
                case EVENT_SPELL_DOMINATE_MIND_25:
                {
                    me->MonsterYell("Ahora vais a hacer lo que yo os diga...", LANG_UNIVERSAL, 0);

                    std::vector<Player*> validPlayers;
                    Map::PlayerList const& pList = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                        if (Player* plr = itr->GetSource())
                            if (plr->IsAlive() && !plr->IsGameMaster() && plr->GetExactDist2dSq(me) < (150.0f * 150.0f))
                                if (!me->GetVictim() || me->GetVictim()->GetGUID() != plr->GetGUID())
                                {
                                    validPlayers.push_back(plr);
                                }

                    std::vector<Player*>::iterator begin = validPlayers.begin(), end = validPlayers.end();

                    std::random_device rd;
                    std::shuffle(begin, end, std::default_random_engine{ rd() });

                    for (uint8 i = 0; i < 3 && i < validPlayers.size(); i++)
                    {
                        Unit* target = validPlayers[i];
                        me->CastSpell(target, SPELL_DOMINATE_MIND_25, true);
                    }

                    _events.ScheduleEvent(EVENT_SPELL_DOMINATE_MIND_25, 35000);
                }
                break;
                case EVENT_SPELL_LEGION_FLAME:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                    {
                        me->MonsterYell("Corre insentato!", LANG_UNIVERSAL, 0);
                        me->CastSpell(target, SPELL_LEGION_FLAME, false);
                    }
                    _events.ScheduleEvent(EVENT_SPELL_LEGION_FLAME, 15000);
                    break;
                case EVENT_SPELL_INCINERATE_FLESH:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                    {
                        me->CastSpell(target, SPELL_INCINERATE_FLESH, false);
                    }
                    _events.ScheduleEvent(EVENT_SPELL_LEGION_FLAME, urand(10000, 13000));
                    break;
                case EVENT_SPELL_RADIANCE:
                    me->CastSpell((Unit*)NULL, SPELL_RADIANCE, false);
                    me->MonsterTextEmote(TEXT_RADIATE, 0, true);
                    _events.ScheduleEvent(EVENT_SPELL_RADIANCE, 6000);
                    break;
                case EVENT_DECIMATE:
                    me->CastSpell(me->GetVictim(), SPELL_DECIMATE, false);
                    _events.ScheduleEvent(EVENT_DECIMATE, 10000);
                    break;
                case EVENT_BLISTERING_COLD:
                    me->MonsterYell("Huid de mi...", LANG_UNIVERSAL, 0);
                    me->CastSpell(me, SPELL_BLISTERING_COLD, false);
					break;
                case EVENT_ICY_GRIP:
                    me->CastSpell((Unit*)NULL, SPELL_ICY_GRIP, false);
                    _events.ScheduleEvent(EVENT_BLISTERING_COLD, 1500);
                    break;
                default:
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

        void EnterEvadeMode()
        {
            me->SetControlled(false, UNIT_STATE_ROOT);
            me->DisableRotate(false);
            ScriptedAI::EnterEvadeMode();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            switch (spell->Id)
            {
            case SPELL_SLIME_SPRAY:
                me->MonsterYell("La comida me ha sentado mal...", LANG_UNIVERSAL, 0);
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
        return new custom_hallows_end_event_boss_twoAI(creature);
    }
};

class custom_hallows_end_event_boss_two_add : public CreatureScript
{
public:
    custom_hallows_end_event_boss_two_add() : CreatureScript("custom_hallows_end_event_boss_two_add") { }

    struct custom_hallows_end_event_boss_two_addAI : public ScriptedAI
    {
        custom_hallows_end_event_boss_two_addAI(Creature* creature) : ScriptedAI(creature) { }

        // OK
        void Reset() override
        {
            if (!me->IsAlive())
                return;

            _events.Reset();
        }

        // OK
        void JustDied(Unit* /*killer*/) override
        {
            _events.Reset();
        }

        // OK
        void EnterCombat(Unit* /*who*/) override
        {
            _events.SetPhase(PHASE_ONE);
            _events.ScheduleEvent(EVENT_BONE_SLICE, 7000);
        }

        void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask)
        {
            if (me->HealthBelowPctDamaged(20, damage) && _events.IsInPhase(PHASE_ONE))
            {
                _events.SetPhase(PHASE_TWO);
                _events.ScheduleEvent(EVENT_BOMB_SUICIDE, 2000);
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
                case EVENT_BONE_SLICE:
                    DoCastVictim(SPELL_BONE_SLICE);
                    _events.ScheduleEvent(EVENT_BEBENDE_ERDE, 10000);
                    break;
                case EVENT_BOMB_SUICIDE:
                    DoCastSelf(SPELL_BOMB_SUICIDE, false);
                    break;

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
        return new custom_hallows_end_event_boss_two_addAI(creature);
    }
};


void AddSC_custom_hallows_end_event()
{
    new custom_hallows_end_event_boss_one();
    new custom_hallows_end_event_boss_two();
}
