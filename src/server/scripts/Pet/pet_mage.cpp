/*
 * Copyright (C) 2016+     AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-GPL2
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 */

/*
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "npc_pet_mag_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "CombatAI.h"
#include "Pet.h"
#include "SpellAuras.h"

enum MageSpells
{
    SPELL_MAGE_CLONE_ME                 = 45204,
    SPELL_MAGE_MASTERS_THREAT_LIST      = 58838,
    SPELL_PET_HIT_SCALING               = 61013,
    SPELL_SUMMON_MIRROR_IMAGE1          = 58831,
    SPELL_SUMMON_MIRROR_IMAGE2          = 58833,
    SPELL_SUMMON_MIRROR_IMAGE3          = 58834,
    SPELL_SUMMON_MIRROR_IMAGE_GLYPH     = 65047,
    SPELL_MAGE_FROST_BOLT               = 59638,
    SPELL_MAGE_FIRE_BLAST               = 59637
};

class DeathEvent : public BasicEvent
{
    public:
        DeathEvent(Creature& owner) : BasicEvent(), _owner(owner) { }

        bool Execute(uint64 /*eventTime*/, uint32 /*diff*/)
        {
            Unit::Kill(&_owner, &_owner);
            return true;
        }

    private:
        Creature& _owner;
};

class npc_pet_mage_mirror_image : public CreatureScript
{
    public:
        npc_pet_mage_mirror_image() : CreatureScript("npc_pet_mage_mirror_image") { }

        struct npc_pet_mage_mirror_imageAI : ScriptedAI
        {
            npc_pet_mage_mirror_imageAI(Creature* creature) : ScriptedAI(creature)
            {
                _selectionTimer = 0;
                _checktarget = 0;
            }

            uint32 dist = urand(1, 5);

            void InitializeAI() override
            {
                ScriptedAI::InitializeAI();
                Unit* owner = me->GetOwner();
                if (!owner)
                    return;

                // Clone Me!
                owner->CastSpell(me, SPELL_MAGE_CLONE_ME, true);

                // xinef: Glyph of Mirror Image (4th copy)
                float angle = 0.0f;
                switch (me->GetUInt32Value(UNIT_CREATED_BY_SPELL))
                {
                    case SPELL_SUMMON_MIRROR_IMAGE1:
                        angle = 0.5f * M_PI;
                        break;
                    case SPELL_SUMMON_MIRROR_IMAGE2:
                        angle = M_PI;
                        break;
                    case SPELL_SUMMON_MIRROR_IMAGE3:
                        angle = 1.5f * M_PI;
                        break;
                }

                ((Minion*)me)->SetFollowAngle(angle);
                if (owner->IsInCombat())
                    me->NearTeleportTo(me->GetPositionX() + cos(angle)*dist, me->GetPositionY() + sin(angle)*dist, me->GetPositionZ(), me->GetOrientation(), false, false, false, false);
                else
                    me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);

                me->SetReactState(REACT_DEFENSIVE);

                me->m_Events.AddEvent(new DeathEvent(*me), me->m_Events.CalculateTime(29500));
            }

            uint64 GetElementalTargetGUID()
            {
                uint64 elementalTargetGUID = 0;

                std::list<Unit*> targets;
                acore::AnyFriendlyUnitInObjectRangeCheck elemental_check(me, me, 50);
                acore::UnitListSearcher<acore::AnyFriendlyUnitInObjectRangeCheck> searcher(me, targets, elemental_check);
                me->VisitNearbyObject(50, searcher);
                for (std::list<Unit*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
                {
                    if ((*iter)->GetEntry() == 510) // elemental entry
                        if ((*iter)->GetOwnerGUID() == me->GetOwnerGUID()) // same owner
                        {
                            elementalTargetGUID = (*iter)->GetTarget();
                            break;
                        }
                }

                return elementalTargetGUID;
            }

            // Do not reload Creature templates on evade mode enter - prevent visual lost
            void EnterEvadeMode() override
            {
                if (me->IsInEvadeMode() || !me->IsAlive())
                    return;

                Unit* owner = me->GetCharmerOrOwner();

                me->CombatStop(true);
                if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW))
                {
                    me->GetMotionMaster()->Clear(false);
                    me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
                }
            }

            void MySelectNextTarget()
            {
                Unit* owner = me->GetOwner();

                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                {
                    Unit* elementalTarget = ObjectAccessor::GetUnit(*me, GetElementalTargetGUID());
                    Unit* selection = owner->ToPlayer()->GetSelectedUnit();
                    if (elementalTarget && me->IsValidAttackTarget(elementalTarget) && !elementalTarget->HasBreakableByDamageCrowdControlAura())
                    {
                        if (elementalTarget != me->GetVictim())
                        {
                            if (owner->IsInCombat())
                            {
                                me->GetMotionMaster()->Clear(false);
                                SetGazeOn(elementalTarget);
                            }
                        }
                    }
                    else if (selection && selection != me->GetVictim() && me->IsValidAttackTarget(selection) && (!me->GetVictim() || !me->IsValidAttackTarget(me->GetVictim()) || !owner->CanSeeOrDetect(me->GetVictim())) && !selection->HasBreakableByDamageCrowdControlAura())
                    {
                        if (owner->IsInCombat())
                        {
                            me->GetMotionMaster()->Clear(false);
                            SetGazeOn(selection);
                        }
                    }
                    else if ((!me->GetVictim() && !owner->IsInCombat())
                        || (me->GetVictim() && !owner->CanSeeOrDetect(me->GetVictim()))
                        || (me->GetVictim() && me->GetVictim()->HasBreakableByDamageCrowdControlAura()))
                    {
                        EnterEvadeMode();
                    }
                }
            }

            void Reset() override
            {
                _selectionTimer = 0;
                _checktarget = 0;
                _events.Reset();
                me->SetReactState(REACT_PASSIVE);
                MySelectNextTarget();
            }

            void EnterCombat(Unit* who) override
            {
                if (me->GetVictim() && !me->GetVictim()->HasBreakableByDamageCrowdControlAura())
                {
                    me->CastSpell(who, SPELL_MAGE_FIRE_BLAST, false);
                    _events.ScheduleEvent(SPELL_MAGE_FROST_BOLT, 0);
                    _events.ScheduleEvent(SPELL_MAGE_FIRE_BLAST, 6500);
                }
                else
                    EnterEvadeMode();
            }

            void UpdateAI(uint32 diff) override
            {
                _events.Update(diff);
                if (_events.GetTimer() < 1200)
                    return;

                _checktarget += diff;
                _selectionTimer += diff;


                if (!UpdateVictimWithGaze() || !me->IsInCombat() || !me->GetVictim())
                {
                    MySelectNextTarget();
                    return;
                }

                if (_checktarget >= 1000)
                {
                    if (me->GetVictim()->HasBreakableByDamageCrowdControlAura() || !me->GetVictim()->IsAlive())
                    {
                        MySelectNextTarget();
                        me->InterruptNonMeleeSpells(true); // Stop casting if target is CC or not Alive.
                        _selectionTimer = 0;
                        return;
                    }

                    if (_selectionTimer >= 1000)
                    {
                        MySelectNextTarget();
                        _selectionTimer = 0;
                        return;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING | UNIT_STATE_LOST_CONTROL))
                    return;

                if (uint32 spellId = _events.ExecuteEvent())
                {
                    if (spellId == SPELL_MAGE_FROST_BOLT)
                    {
                        DoCastVictim(spellId, false);
                        _events.ScheduleEvent(SPELL_MAGE_FROST_BOLT, 2500);
                    }
                    else if (spellId == SPELL_MAGE_FIRE_BLAST)
                    {
                        DoCastVictim(spellId, false);
                        _events.ScheduleEvent(SPELL_MAGE_FIRE_BLAST, 6500);
                    }
                }

                
            }

        private:
            uint32 _checktarget;
            uint32 _selectionTimer;
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pet_mage_mirror_imageAI(creature);
        }
};

void AddSC_mage_pet_scripts()
{
    new npc_pet_mage_mirror_image();
}
