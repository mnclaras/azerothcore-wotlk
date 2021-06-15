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
#include "Pet.h"

#define DEFAULT_MESSAGE 907

typedef struct { float x, y; } Point;

static const Point V[] =
{
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f },
    { 500.0f, 250.0f }
};

static const int n = 14;

class custom_npc_anti_pvp : public CreatureScript
{
public:
    custom_npc_anti_pvp() : CreatureScript("NPC_AntiPvP") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new custom_npc_anti_pvpAI(creature);
    }

    struct custom_npc_anti_pvpAI : public BossAI
    {
        uint32 timer;

        custom_npc_anti_pvpAI(Creature* creature) : BossAI(creature, 0) {}

        int isLeft(Point P0, Point P1, Point P2)
        {
            return ((P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y));
        }

        bool IsInArea(Point P)
        {
            int wn = 0;

            for (int i = 0; i < n; i++)
            {
                if (V[i].y <= P.y)
                {
                    if (V[i + 1].y > P.y)
                        if (isLeft(V[i], V[i + 1], P) > 0)
                            ++wn;
                }
                else
                {
                    if (V[i + 1].y <= P.y)
                        if (isLeft(V[i], V[i + 1], P) < 0)
                            --wn;
                }
            }
            return wn != 0;
        }

        void Reset() override
        {
            timer = 1000;
        }

        void UpdateAI(uint32 diff) override
        {
            if (timer <= diff)
            {
                Map::PlayerList const& Players = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
                {
                    if (Player* player = itr->GetSource())
                    {
                        if (player->GetAreaId() != me->GetAreaId())
                            continue;
                        Point p = { player->GetPositionX(), player->GetPositionY() };
                        if (IsInArea(p))
                            player->SetPvP(false);
                        else
                            player->SetPvP(false);
                    }
                }
                timer = 1000;
            }
            else
                timer -= diff;
        }
    };
};

void AddSC_custom_npc_anti_pvp()
{
    new custom_npc_anti_pvp();
}
