#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "Configuration/Config.h"
#include "Creature.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "Define.h"
#include "GossipDef.h"
#include "DataMap.h"
#include "GameObject.h"
#include "Transport.h"
#include "CreatureAI.h"
#include "mod_guild_points.h"


enum GuildHouseNPCData
{
    SENDER_WITH_CHILDRENS = 9999999,
    ACTION_GO_BACK = 9999998,
    ACTION_GOODBYE = 9999999
};

class GuildHouseSpawner : public CreatureScript {

public:
    GuildHouseSpawner() : CreatureScript("GuildHouseSpawner") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        if (player->GetGuild())
        {
            //Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId());
            //Guild::Member const* memberMe = guild->GetMember(player->GetGUID());
            if (!sModGuildPointsMgr->MemberHaveGuildHousePointsPermission(player))
            {
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ?
                    "No estas autorizado para hacer compras en la casa de hermandad": "You are not authorized to make guild house purchases.");
                CloseGossipMenuFor(player);
                return false;
            }
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "No estas en una hermandad" : "You are not in a guild!");
            CloseGossipMenuFor(player);
            return false;
        }

        ClearGossipMenuFor(player);

        std::string pointsText = isSpanish ? " puntos." : " points.";
        std::string wantToCreateText = isSpanish ? "Deseas crear [" : "Want to create [";
        std::string forText = isSpanish ? "] por " : "] for ";
        std::string guildPointsText = isSpanish ? " puntos de hermandad?" : " guild points?";
        std::string currentPointsText = isSpanish ? "TUS PUNTOS: " : "YOUR POINTS: ";

        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, currentPointsText + std::to_string(sModGuildPointsMgr->GetGuildHousePoints(player->GetGuildId())),
            GOSSIP_SENDER_MAIN, ACTION_GO_BACK);

        // Display all first level options
        for (GuildHouseSpawnInfoContainer::const_iterator itr = sModGuildPointsMgr->m_GuildHouseSpawnInfoContainer.begin(); itr != sModGuildPointsMgr->m_GuildHouseSpawnInfoContainer.end(); ++itr)
        {
            if ((!(*itr)->parent || (*itr)->parent == 0) && !(*itr)->name.empty() && (*itr)->isVisible && !(*itr)->isInitialSpawn)
            {              
                if (!(*itr)->isMenu)
                {
                    if (((*itr)->isCreature && !NPCExists(player, (*itr)->entry))
                        ||
                        (!(*itr)->isCreature && !ObjectExists(player, (*itr)->entry)))
                    {
                        AddGossipItemFor(player, GOSSIP_ICON_TALK, (*itr)->name + ": " + std::to_string((*itr)->points) + pointsText,
                            GOSSIP_SENDER_MAIN/*(*itr)->points*/, (*itr)->id,
                            wantToCreateText + (*itr)->name + forText + std::to_string((*itr)->points) + guildPointsText,
                            0, false);
                    }
                }
                else
                {
                    AddGossipItemFor(player, GOSSIP_ICON_TALK, (*itr)->name, SENDER_WITH_CHILDRENS, (*itr)->id);
                }             
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_TALK,
            isSpanish ? "|TInterface/ICONS/Thrown_1H_Harpoon_D_01Blue:20|t Hasta Luego!"
            : "|TInterface/ICONS/Thrown_1H_Harpoon_D_01Blue:20|t Nevermind!",
            GOSSIP_SENDER_MAIN, ACTION_GOODBYE);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        bool isSpanish = IsSpanishPlayer(player);

        ClearGossipMenuFor(player);
        switch (action)
        {
        case ACTION_GO_BACK: { OnGossipHello(player, creature); break; } // GO BACK
        case ACTION_GOODBYE: { CloseGossipMenuFor(player); break; } // CLOSE
            
        default:
            // action = ID
            // sender = POINTS OR SHOW CHILDREN MENU

            if (sender == SENDER_WITH_CHILDRENS)
            {
                std::string pointsText = isSpanish ? " puntos." : " points.";
                std::string wantToCreateText = isSpanish ? "Deseas crear [" : "Want to create [";
                std::string forText = isSpanish ? "] por " : "] for ";
                std::string guildPointsText = isSpanish ? " puntos de hermandad?" : " guild points?";
                std::string currentPointsText = isSpanish ? "TUS PUNTOS: " : "YOUR POINTS: ";

                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, currentPointsText + std::to_string(sModGuildPointsMgr->GetGuildHousePoints(player->GetGuildId())),
                    GOSSIP_SENDER_MAIN, ACTION_GO_BACK);

                bool menuHavePurchases = false;
                for (GuildHouseSpawnInfoContainer::const_iterator itr = sModGuildPointsMgr->m_GuildHouseSpawnInfoContainer.begin(); itr != sModGuildPointsMgr->m_GuildHouseSpawnInfoContainer.end(); ++itr)
                {
                    if ((*itr)->parent && (*itr)->parent == action && !(*itr)->name.empty() && (*itr)->isVisible && !(*itr)->isInitialSpawn)
                    {
                        if (!(*itr)->isMenu)
                        {
                            if (((*itr)->isCreature && !NPCExists(player, (*itr)->entry))
                                ||
                                (!(*itr)->isCreature && !ObjectExists(player, (*itr)->entry)))
                            {
                                AddGossipItemFor(player, GOSSIP_ICON_TALK, (*itr)->name + ": " + std::to_string((*itr)->points) + pointsText,
                                    GOSSIP_SENDER_MAIN/*(*itr)->points*/, (*itr)->id,
                                    wantToCreateText + (*itr)->name + forText + std::to_string((*itr)->points) + guildPointsText,
                                    0, false);
                                menuHavePurchases = true;
                            }        
                        }
                        else
                        {
                            AddGossipItemFor(player, GOSSIP_ICON_TALK, (*itr)->name, SENDER_WITH_CHILDRENS, (*itr)->id);
                            menuHavePurchases = true;
                        }
                    }
                }

                if (menuHavePurchases)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_TALK,
                        isSpanish ? "|TInterface/ICONS/Thrown_1H_Harpoon_D_01Blue:20|t Hasta Luego!"
                        : "|TInterface/ICONS/Thrown_1H_Harpoon_D_01Blue:20|t Nevermind!",
                        GOSSIP_SENDER_MAIN, ACTION_GOODBYE);
                    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                }
                else
                {
                    ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "Ya has comprado todos los NPC y Objetos de esta seccion."
                        : "You already bought all NPC and Objects of this section.");
                    OnGossipHello(player, creature);
                }
            }
            else
            {
                // LEAF NODE, DO TRANSACTION
                for (GuildHouseSpawnInfoContainer::const_iterator itr = sModGuildPointsMgr->m_GuildHouseSpawnInfoContainer.begin(); itr != sModGuildPointsMgr->m_GuildHouseSpawnInfoContainer.end(); ++itr)
                {
                    if ((*itr)->id == action)
                    {
                        if ((*itr)->isCreature)
                        {
                            SpawnNPC((*itr)->entry, (*itr)->posX, (*itr)->posY, (*itr)->posZ, (*itr)->orientation, player, (*itr)->points);
                        }
                        else
                        {
                            SpawnObject((*itr)->entry, (*itr)->posX, (*itr)->posY, (*itr)->posZ, (*itr)->orientation, player, (*itr)->points);
                        }
                        break;
                    }
                }

                OnGossipHello(player, creature);
            }
            break;
        }
        return true;
    }

	uint32 GetGuildPhase(Player* player) {
		return player->GetGuildId() + 10;
	}

    bool NPCExists(Player* player, uint32 entry)
    {
        if (player->FindNearestCreature(entry, VISIBILITY_RANGE, true))
        {
            return true;
        }

        return false;
    }

    bool ObjectExists(Player* player, uint32 entry)
    {
        if (player->FindNearestGameObject(entry, VISIBILITY_RANGE))
        {
            return true;
        }

        return false;
    }

    void SpawnNPC(uint32 entry, float posX, float posY, float posZ, float orientation, Player* player, uint32 cost)
    {
        bool isSpanish = IsSpanishPlayer(player);

        if (NPCExists(player, entry))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "Ya tienes a este NPC!" : "You already have this NPC!");
            return;
        }

        if (!sObjectMgr->GetCreatureTemplate(entry))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "El NPC no pudo ser añadido. Contacta con un GM! (Criatura)" : "NPC couldn't be added. Contact a GM! (Creature)");
            return;
        }

        if (sModGuildPointsMgr->SpendGuildHousePoints(player, cost))
        {
            bool processOk = true;

            Creature* creature = new Creature();
            if (!creature->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT), player->GetMap(), GetGuildPhase(player), entry, 0, posX, posY, posZ, orientation))
            {
                delete creature;
                processOk = false;
            }
            else
            {
                creature->SaveToDB(player->GetMapId(), (1 << player->GetMap()->GetSpawnMode()), GetGuildPhase(player));

                uint32 db_guid = creature->GetDBTableGUIDLow();

                creature->CleanupsBeforeDelete();
                delete creature;
                creature = new Creature();

                if (!creature->LoadCreatureFromDB(db_guid, player->GetMap()))
                {
                    delete creature;
                    processOk = false;
                }
                else
                {
                    sObjectMgr->AddCreatureToGrid(db_guid, sObjectMgr->GetCreatureData(db_guid));
                    ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "NPC añadido con exito!" : "NPC sucessfully added!");
                }
            }

            if (!processOk)
            {
                // If creature couldn't be added, we revert the points transaction.
                sModGuildPointsMgr->AddGuildHousePoints(player, cost);
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "El NPC no pudo ser añadido. Contacta con un GM!" : "NPC couldn't be added. Contact a GM!");
            }
        }
    }

    void SpawnObject(uint32 entry, float posX, float posY, float posZ, float orientation, Player* player, uint32 cost)
    {
        bool isSpanish = IsSpanishPlayer(player);

        if (ObjectExists(player, entry))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "Ya tienes este objeto!" : "You already have this object!");
            CloseGossipMenuFor(player);
            return;
        }

        uint32 objectId = entry;
        if (!objectId)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "El Objeto no pudo ser añadido. Contacta con un GM! (Plantilla)" : "Object couldn't be added. Contact a GM! (Template)");
            return;
        }

        const GameObjectTemplate* objectInfo = sObjectMgr->GetGameObjectTemplate(objectId);

        if (!objectInfo)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "El Objeto no pudo ser añadido. Contacta con un GM! (Plantilla)" : "Object couldn't be added. Contact a GM! (Template)");
            return;
        }

        if (objectInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(objectInfo->displayId))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "El Objeto no pudo ser añadido. Contacta con un GM! (Modelo)" : "Object couldn't be added. Contact a GM! (Model)");
            return;
        }

        if (sModGuildPointsMgr->SpendGuildHousePoints(player, cost))
        {
            bool processOk = true;

            GameObject* object = sObjectMgr->IsGameObjectStaticTransport(objectInfo->entry) ? new StaticTransport() : new GameObject();
            uint32 guidLow = sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT);

            if (!object->Create(guidLow, objectInfo->entry, player->GetMap(), GetGuildPhase(player), posX, posY, posZ, orientation, G3D::Quat(), 0, GO_STATE_READY))
            {
                delete object;
                processOk = false;
            }
            else
            {
                // fill the gameobject data and save to the db
                object->SaveToDB(player->GetMapId(), (1 << player->GetMap()->GetSpawnMode()), GetGuildPhase(player));
                // delete the old object and do a clean load from DB with a fresh new GameObject instance.
                // this is required to avoid weird behavior and memory leaks
                delete object;

                object = sObjectMgr->IsGameObjectStaticTransport(objectInfo->entry) ? new StaticTransport() : new GameObject();
                // this will generate a new guid if the object is in an instance
                if (!object->LoadGameObjectFromDB(guidLow, player->GetMap()))
                {
                    delete object;
                    processOk = false;
                }
                else
                {
                    // TODO: is it really necessary to add both the real and DB table guid here ?
                    sObjectMgr->AddGameobjectToGrid(guidLow, sObjectMgr->GetGOData(guidLow));
                    ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "Objeto añadido con exito!" : "Object sucessfully added!");
                }
            }

            if (!processOk)
            {
                // If creature couldn't be added, we revert the points transaction.
                sModGuildPointsMgr->AddGuildHousePoints(player, cost);
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "El Objeto no pudo ser añadido. Contacta con un GM!" : "Object couldn't be added. Contact a GM!");
            }
        }
    }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }
};

class GuildHouseNPCConf : public WorldScript
{
public:
    GuildHouseNPCConf() : WorldScript("GuildHouseNPCConf") {}

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
    }
};

void AddGuildHouseV2NPCScripts()
{
    new GuildHouseSpawner();
    new GuildHouseNPCConf();
}
