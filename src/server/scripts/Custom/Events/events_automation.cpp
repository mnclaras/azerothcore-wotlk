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
#include "ScriptMgr.h"
#include "Common.h"
#include "DisableMgr.h"
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
#include "World.h"
#include "GameObjectAI.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Group.h"
#include "GroupMgr.h"

struct CustomEventWinners
{
    uint64 firstGUID;
    uint64 secondGUID;
    uint64 thirdGUID;
};

static CustomEventWinners eventWinners;
//static const Position teleportRewardFirst = { -14593.151367f, -259.881683f, 12.135949f, 5.246307f };
//static const Position teleportRewardSecond = { -14593.151367f, -259.881683f, 12.135949f, 5.246307f };
//static const Position teleportRewardThird = { -14593.151367f, -259.881683f, 12.135949f, 5.246307f };


class go_event_reward_first : public GameObjectScript
{
public:
    go_event_reward_first() : GameObjectScript("go_event_reward_first") { }

    struct go_event_reward_firstAI : public GameObjectAI
    {
        go_event_reward_firstAI(GameObject* go) : GameObjectAI(go) { }

        void UpdateAI(uint32 /*diff*/)
        {
            if (go)
            {
                WorldObject* obj = go;
                Player* target = obj->SelectNearestPlayer(0.5f);
                if (!target || target->IsBeingTeleported() || !target->IsAlive())
                    return;

                if (AccountMgr::IsPlayerAccount(target->GetSession()->GetSecurity())
                    && !eventWinners.firstGUID)
                {
                    eventWinners.firstGUID = target->GetGUID();

                    // Send broadcast message to zone
                    std::stringstream text;
                    text << "|cffff6060[Evento]:|r " << target->GetName() << " ha quedado en |cFFFF4500" << "primer lugar" << "|r!";
                    sWorld->SendServerMessage(SERVER_MSG_STRING, text.str().c_str());

                    // Teleport player to first reward zone
                    // TODO: Give quest instead of teleport to zone
                    //player->TeleportTo(0,
                    //    teleportRewardFirst.GetPositionX(),
                    //    teleportRewardFirst.GetPositionY(),
                    //    teleportRewardFirst.GetPositionZ(),
                    //    teleportRewardFirst.GetOrientation());

                    //// Summon second winner gameobject
                    //obj->SummonGameObject(GAMEOBJECT_WINNER_TWO,
                    //    teleportRewardSecond.GetPositionX(),
                    //    teleportRewardSecond.GetPositionY(),
                    //    teleportRewardSecond.GetPositionZ(),
                    //    teleportRewardSecond.GetOrientation(),
                    //    teleportRewardSecond.GetOrientation(),
                    //    teleportRewardSecond.GetOrientation(),
                    //    teleportRewardSecond.GetOrientation(),
                    //    teleportRewardSecond.GetOrientation(),
                    //    300000);

                    // Destroy gameobject
                    go->RemoveFromWorld();
                }
            }
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_event_reward_firstAI(go);
    }
};

class go_event_reward_second : public GameObjectScript
{
public:
    go_event_reward_second() : GameObjectScript("go_event_reward_second") { }

    struct go_event_reward_secondAI : public GameObjectAI
    {
        go_event_reward_secondAI(GameObject* go) : GameObjectAI(go) { }

        void UpdateAI(uint32 /*diff*/)
        {
            if (go)
            {
                WorldObject* obj = go;
                Player* target = obj->SelectNearestPlayer(0.5f);
                if (!target || target->IsBeingTeleported() || !target->IsAlive())
                    return;

                if (AccountMgr::IsPlayerAccount(target->GetSession()->GetSecurity())
                    && (eventWinners.firstGUID && eventWinners.firstGUID != target->GetGUID()))
                {
                    eventWinners.secondGUID = target->GetGUID();

                    // Send broadcast message to zone
                    std::stringstream text;
                    text << "|cffff6060[Evento]:|r " << target->GetName() << " ha quedado en |cFFFF4500" << "segundo lugar" << "|r!";
                    sWorld->SendServerMessage(SERVER_MSG_STRING, text.str().c_str());

                    // Teleport player to first reward zone
                    // TODO: Give quest instead of teleport to zone
                    //player->TeleportTo(0,
                    //    teleportRewardSecond.GetPositionX(),
                    //    teleportRewardSecond.GetPositionY(),
                    //    teleportRewardSecond.GetPositionZ(),
                    //    teleportRewardSecond.GetOrientation());

                    //// Summon second winner gameobject
                    //obj->SummonGameObject(GAMEOBJECT_WINNER_TWO,
                    //    teleportRewardT.GetPositionX(),
                    //    teleportRewardT.GetPositionY(),
                    //    teleportRewardT.GetPositionZ(),
                    //    teleportRewardT.GetOrientation(),
                    //    teleportRewardT.GetOrientation(),
                    //    teleportRewardT.GetOrientation(),
                    //    teleportRewardT.GetOrientation(),
                    //    teleportRewardT.GetOrientation(),
                    //    300000);

                    // Destroy gameobject
                    go->RemoveFromWorld();
                }
            }
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_event_reward_secondAI(go);
    }
};

class go_event_reward_third : public GameObjectScript
{
public:
    go_event_reward_third() : GameObjectScript("go_event_reward_third") { }

    struct go_event_reward_thirdAI : public GameObjectAI
    {
        go_event_reward_thirdAI(GameObject* go) : GameObjectAI(go) { }

        void UpdateAI(uint32 /*diff*/)
        {
            if (go)
            {
                WorldObject* obj = go;
                Player* target = obj->SelectNearestPlayer(0.5f);
                if (!target || target->IsBeingTeleported() || !target->IsAlive())
                    return;

                if (AccountMgr::IsPlayerAccount(target->GetSession()->GetSecurity())
                    && (eventWinners.firstGUID && eventWinners.firstGUID != target->GetGUID())
                    && (eventWinners.secondGUID && eventWinners.secondGUID != target->GetGUID()))
                {
                    eventWinners.thirdGUID = target->GetGUID();

                    // Send broadcast message to zone
                    std::stringstream text;
                    text << "|cffff6060[Evento]:|r " << target->GetName() << " ha quedado en |cFFFF4500" << "tercer lugar" << "|r!";
                    sWorld->SendServerMessage(SERVER_MSG_STRING, text.str().c_str());

                    // Teleport player to first reward zone
                    // TODO: Give quest instead of teleport to zone
                    //player->TeleportTo(0,
                    //    teleportRewardSecond.GetPositionX(),
                    //    teleportRewardSecond.GetPositionY(),
                    //    teleportRewardSecond.GetPositionZ(),
                    //    teleportRewardSecond.GetOrientation());

                    //// Summon second winner gameobject
                    //obj->SummonGameObject(GAMEOBJECT_WINNER_TWO,
                    //    teleportRewardT.GetPositionX(),
                    //    teleportRewardT.GetPositionY(),
                    //    teleportRewardT.GetPositionZ(),
                    //    teleportRewardT.GetOrientation(),
                    //    teleportRewardT.GetOrientation(),
                    //    teleportRewardT.GetOrientation(),
                    //    teleportRewardT.GetOrientation(),
                    //    teleportRewardT.GetOrientation(),
                    //    300000);

                    std::stringstream textFinish;
                    textFinish << "|cffff6060[Evento]:|r " << "El evento ha |cFFFF4500" << "finalizado" << "|r!. Gracias a todos por participar.";
                    sWorld->SendServerMessage(SERVER_MSG_STRING, textFinish.str().c_str());

                    //eventWinners.firstGUID = nullptr;
                    //eventWinners.secondGUID = nullptr;
                    //eventWinners.thirdGUID = nullptr;

                    // Destroy gameobject
                    go->RemoveFromWorld();
                }
            }
        }
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return new go_event_reward_thirdAI(go);
    }
};

void AddSC_event_automation()
{
    new go_event_reward_first();
    new go_event_reward_second();
    new go_event_reward_third();
}
