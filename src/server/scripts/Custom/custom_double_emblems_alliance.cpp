#include "DBCEnums.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "DBCStores.h"
#include "Language.h"
#include "Player.h"
#include "Group.h"
#include "GroupMgr.h"
#include "InstanceScript.h"

#define EMBLEM_OF_FROST_ENTRY 49426

class custom_double_emblems_alliance : public PlayerScript
{
public:
    custom_double_emblems_alliance() : PlayerScript("custom_double_emblems_alliance") { }

    void OnCreatureKill(Player* player, Creature* boss) override
    {
        if (!player)
            return;

        // Icecrown Citadel & Rubi Sanctum
        if (boss->GetMap()->IsRaid() && boss->getLevel() > 80 && boss->IsDungeonBoss() && player->GetTeamId() == TEAM_ALLIANCE && (player->GetMapId() == 631 || player->GetMapId() == 724))
        {
            player->AddItem(EMBLEM_OF_FROST_ENTRY, 2);

            Group* grp = player->GetGroup();
            if (grp)
            {
                for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
                    if (Player* member = itr->GetSource())
                        if (member->IsInMap(player) && player->GetGUID() != member->GetGUID())
                        {
                            member->AddItem(EMBLEM_OF_FROST_ENTRY, 2);
                        }
            }
        }
    }
};

void AddSC_custom_double_emblems_alliance()
{
    new custom_double_emblems_alliance();
}
