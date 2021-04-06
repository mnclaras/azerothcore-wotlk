#include "ScriptMgr.h"
#include "Player.h"
#include "Configuration/Config.h"
#include "Creature.h"
#include "Guild.h"
#include "SpellAuraEffects.h"
#include "Chat.h"
#include "ScriptedGossip.h"
#include "GuildMgr.h"
#include "Define.h"
#include "GossipDef.h"
#include "DataMap.h"
#include "GameObject.h"
#include "Transport.h"
#include "Maps/MapManager.h"
#include "mod_guild_points.h"

static const Position guildHousePosition = { -14593.151367f, -259.881683f, 12.135949f, 5.246307f };
static const Position shop = { -11823.9f, -4779.58f, 5.9206f, 1.1357f };

enum GuildHouseMaps
{
    MAP_TANARIS = 1
};

struct GuildZonesSpecificInfo
{
    uint32 zoneId;
    uint32 phase;
};

static std::set<uint32> GuildZones;
static std::map<uint32, GuildZonesSpecificInfo> GuildZonesInfo;


class GuildData : public DataMap::Base
{
public:
    GuildData() {}
    GuildData(uint32 phase, float posX, float posY, float posZ) : phase(phase), posX(posX), posY(posY), posZ(posZ) {}
    uint32 phase;
    float posX;
    float posY;
    float posZ;
};

class GuildHelper : public GuildScript {

public:

    GuildHelper() : GuildScript("GuildHelper") {}

    void OnCreate(Guild*, Player* leader, const std::string&)
    {
        ChatHandler(leader->GetSession()).PSendSysMessage(
            IsSpanishPlayer(leader) ? "Ahora posees una hermandad. Puedes comprar una casa de hermandad en cualquier momento."
            : "You now own a guild. You can purchase a guild house!");
    }

    uint32 GetGuildPhase(Guild* guild) {
        return guild->GetId() + 10;
    }

    void OnDisband(Guild* guild)
    {
        if (RemoveGuildHouse(guild))
        {       
            sLog->outBasic("GUILDHOUSE: Deleting guild house data due to disbanding of guild...");
        } else { sLog->outBasic("GUILDHOUSE: Error deleting guild house data during disbanding of guild!!"); }

        sModGuildPointsMgr->DeleteGuild(guild);
    }

    bool RemoveGuildHouse(Guild* guild)
    {
        uint32 guildPhase = GetGuildPhase(guild);

        QueryResult has_gh = CharacterDatabase.PQuery("SELECT `id`, `guild`, `map` FROM `guild_house` WHERE guild = %u", guild->GetId());
        if (!has_gh)
        {
            return false;
        }

        QueryResult CreatureResult;
        QueryResult GameobjResult;

        // Lets find all of the gameobjects to be removed
        GameobjResult = WorldDatabase.PQuery("SELECT `guid` FROM `gameobject` WHERE `map` = '%u' AND `phaseMask` = '%u'", (*has_gh)[2].GetUInt32(), guildPhase);
        // Lets find all of the creatures to be removed
        CreatureResult = WorldDatabase.PQuery("SELECT `guid` FROM `creature` WHERE `map` = '%u' AND `phaseMask` = '%u'", (*has_gh)[2].GetUInt32(), guildPhase);


        // remove creatures from the deleted guild house map
        if (CreatureResult) {
            do
            {
                Field* fields = CreatureResult->Fetch();
                uint32 lowguid = fields[0].GetInt32();
                if (CreatureData const* cr_data = sObjectMgr->GetCreatureData(lowguid)) {
                    if (Creature* creature = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(lowguid, cr_data->id, HIGHGUID_UNIT), (Creature*)NULL))
                    {
                        creature->CombatStop();
                        creature->DeleteFromDB();
                        creature->AddObjectToRemoveList();
                    }
                }
            } while (CreatureResult->NextRow());
        }


        // remove gameobjects from the deleted guild house map
        if (GameobjResult) {
            do
            {
                Field* fields = GameobjResult->Fetch();
                uint32 lowguid = fields[0].GetInt32();
                if (GameObjectData const* go_data = sObjectMgr->GetGOData(lowguid)) {
                    //if (GameObject* gobject = ObjectAccessor::GetObjectInWorld(lowguid, (GameObject*)NULL))
                    if (GameObject* gobject = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(lowguid, go_data->id, HIGHGUID_GAMEOBJECT), (GameObject*)NULL))
                    {
                        gobject->SetRespawnTime(0);
                        gobject->Delete();
                        gobject->DeleteFromDB();
                        gobject->CleanupsBeforeDelete();
                        //delete gobject;
                    }
                }

            } while (GameobjResult->NextRow());
        }

        return true;
    }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }
};

class GuildHouseSeller : public CreatureScript {

public:
    GuildHouseSeller() : CreatureScript("GuildHouseSeller") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        bool isSpanish = IsSpanishPlayer(player);
        if (!player->GetGuild())
        {
            ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "No eres miembro de una hermandad" : "You are not a member of a guild.");
            CloseGossipMenuFor(player);
            return false;
        }

        QueryResult has_gh = CharacterDatabase.PQuery("SELECT id, `guild` FROM `guild_house` WHERE guild = %u", player->GetGuildId());

        // Only show Teleport option if guild owns a guildhouse
        if (has_gh)
        {
            AddGossipItemFor(player, GOSSIP_ICON_TABARD, isSpanish ? "Ir a la Casa de Hermandad" : "Teleport to Guild House", GOSSIP_SENDER_MAIN, 1);
        }

        if (player->GetGuild()->GetLeaderGUID() == player->GetGUID())
        {
            // Only show "Sell" option if they have a guild house & are guild leader
            if (has_gh)
            {
                AddGossipItemFor(player, GOSSIP_ICON_TABARD, isSpanish ? "Borrar Casa de Hermandad" : "Delete Guild House!", GOSSIP_SENDER_MAIN, 3,
                    isSpanish ? "Seguro que quieres eliminar tu Casa de Hermandad?" : "Are you sure you want to delete your Guild house?", 0, false);
            }
            else
            {
                // Only leader of the guild can buy guild house & only if they don't already have a guild house
                AddGossipItemFor(player, GOSSIP_ICON_TABARD, isSpanish ? "Comprar Casa de Hermandad (5000 puntos)!" : "Buy Guild House (5000 points)!", GOSSIP_SENDER_MAIN, 2);
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Cerrar menu" : "Close menu", GOSSIP_SENDER_MAIN, 5);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        bool isSpanish = IsSpanishPlayer(player);
        switch (action)
        {
        case 5: // close
            CloseGossipMenuFor(player);
            break;
        case 3: // Delete guild house
        {
            QueryResult has_gh = CharacterDatabase.PQuery("SELECT `id`, `guild`, `map` FROM `guild_house` WHERE guild = %u", player->GetGuildId());
            if (!has_gh)
            {
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "Tu hermandad no tiene Casa de Hermandad" : "Your guild does not own a Guild House!");
                CloseGossipMenuFor(player);
                return false;
            }

            if (RemoveGuildHouse(player, (*has_gh)[2].GetUInt32()))
            {
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "La Casa de Hermandad ha sido eliminada" : "You have successfully deleted your guild house.");
                player->GetGuild()->BroadcastToGuild(player->GetSession(), false,
                    isSpanish ? "Hemos eliminado nuestra Casa de Hermandad." : "We just removed our guild house.", LANG_UNIVERSAL);
                sLog->outBasic("GUILDHOUSE: Successfully deleted guildhouse");
                CloseGossipMenuFor(player);
            }
            else
            {
                ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "Hubo un problema eliminando la casa de hermandad." : "There was an error removing your guild house.");
                CloseGossipMenuFor(player);
            }

            sModGuildPointsMgr->DeleteGuild((*has_gh)[2].GetUInt32());

            break;
        }
        case 2: // buy guild house
            BuyGuildHouse(player->GetGuild(), player, creature);
            break;
        case 1: // teleport to guild house
            TeleportGuildHouse(player->GetGuild(), player, creature);
            break;
        }

        if (action >= 100)
        {
            uint32 guildPosition = action - 100;
            QueryResult resultAvailableGH = CharacterDatabase.PQuery(
                "SELECT `map`, `zone`, `posX`, `posY`, `posZ` FROM guild_house_position WHERE `id` = '%u';", guildPosition);
            if (resultAvailableGH)
            {
                uint32 map = (*resultAvailableGH)[0].GetUInt32();
                uint32 zone = (*resultAvailableGH)[1].GetUInt32();
                float posX = (*resultAvailableGH)[2].GetFloat();
                float posY = (*resultAvailableGH)[3].GetFloat();
                float posZ = (*resultAvailableGH)[4].GetFloat();

                if (sModGuildPointsMgr->CheckCanSpendGuildHousePoints(player, 5000))
                {
                    sModGuildPointsMgr->SpendGuildHousePoints(player, 5000);

                    CharacterDatabase.PQuery("INSERT INTO `guild_house` (guild, phase, map, zoneId, posX, posY, posZ) VALUES (%u, %u, %u, %u, %f, %f, %f)",
                        player->GetGuildId(), GetGuildPhase(player), map, zone, posX, posY, posZ);

                    // Msg to purchaser and Msg Guild as purchaser 
                    ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "La Casa de Hermandad ha sido comprada con exito!" : "You have successfully purchased a Guild House");
                    player->GetGuild()->BroadcastToGuild(player->GetSession(), false, isSpanish ? "Tenemos Casa de Hermandad!" : "We now have a Guild House!", LANG_UNIVERSAL);
                    sLog->outBasic("GUILDHOUSE: GuildId: '%u' has purchased a guildhouse", player->GetGuildId());

                    GuildZonesInfo[player->GetGuildId()].phase = GetGuildPhase(player);
                    GuildZonesInfo[player->GetGuildId()].zoneId = zone;

                    // Spawn a portal and the guild assistant automatically as part of purchase.
                    SpawnInitialNpcs(player, guildPosition);
                }
                CloseGossipMenuFor(player);
            }
        }

        return true;
    }

    uint32 GetGuildPhase(Player* player) {
        return player->GetGuildId() + 10;
    }

    bool RemoveGuildHouse(Player* player, uint32 mapId)
    {
        uint32 guildPhase = GetGuildPhase(player);
        QueryResult CreatureResult;
        QueryResult GameobjResult;

        // Lets find all of the gameobjects to be removed       
        GameobjResult = WorldDatabase.PQuery("SELECT `guid` FROM `gameobject` WHERE `map` = '%u' AND `phaseMask` = '%u'", mapId, guildPhase);
        // Lets find all of the creatures to be removed
        CreatureResult = WorldDatabase.PQuery("SELECT `guid` FROM `creature` WHERE `map` = '%u' AND `phaseMask` = '%u'", mapId, guildPhase);


        // remove creatures from the deleted guild house map
        if (CreatureResult) {
            do
            {
                Field* fields = CreatureResult->Fetch();
                uint32 lowguid = fields[0].GetInt32();
                if (CreatureData const* cr_data = sObjectMgr->GetCreatureData(lowguid)) {
                    if (Creature* creature = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(lowguid, cr_data->id, HIGHGUID_UNIT), (Creature*)NULL))
                    {
                        creature->CombatStop();
                        creature->DeleteFromDB();
                        creature->AddObjectToRemoveList();
                    }
                }
            } while (CreatureResult->NextRow());
        }


        // remove gameobjects from the deleted guild house map
        if (GameobjResult) {
            do
            {
                Field* fields = GameobjResult->Fetch();
                uint32 lowguid = fields[0].GetInt32();
                if (GameObjectData const* go_data = sObjectMgr->GetGOData(lowguid)) {
                    //if (GameObject* gobject = ObjectAccessor::GetObjectInWorld(lowguid, (GameObject*)NULL))
                    if (GameObject* gobject = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(lowguid, go_data->id, HIGHGUID_GAMEOBJECT), (GameObject*)NULL))
                    {
                        gobject->SetRespawnTime(0);
                        gobject->Delete();
                        gobject->DeleteFromDB();
                        gobject->CleanupsBeforeDelete();
                        //delete gobject;
                    }
                }

            } while (GameobjResult->NextRow());
        }

        return true;
    }

    void SpawnInitialNpcs(Player* player, uint32 guildPosition)
    {
        // LEAF NODE, DO TRANSACTION
        for (GuildHouseSpawnInfoContainer::const_iterator itr = sModGuildPointsMgr->m_GuildHouseSpawnInfoContainer.begin(); itr != sModGuildPointsMgr->m_GuildHouseSpawnInfoContainer.end(); ++itr)
        {
            if ((*itr)->isInitialSpawn && (*itr)->guildPosition == guildPosition && (*itr)->entry && (*itr)->entry > 0)
            {
                if ((*itr)->isCreature)
                {
                    SpawnNPC((*itr)->entry, (*itr)->map, (*itr)->posX, (*itr)->posY, (*itr)->posZ, (*itr)->orientation, player);
                }
                else
                {
                    SpawnObject((*itr)->entry, (*itr)->map, (*itr)->posX, (*itr)->posY, (*itr)->posZ, (*itr)->orientation, player);
                }
            }
        }
    }

    bool BuyGuildHouse(Guild* guild, Player* player, Creature* creature)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT `id`, `guild` FROM guild_house WHERE `guild` = %u", guild->GetId());
        bool isSpanish = IsSpanishPlayer(player);
        if (result)
        {
            ChatHandler(player->GetSession()).PSendSysMessage(isSpanish ? "Tu hermandad ya tiene una Casa de Hermandad!" : "Your guild already has a Guild House!");
            CloseGossipMenuFor(player);
            return false;
        }

        ClearGossipMenuFor(player);

        QueryResult resultAvailableGH = CharacterDatabase.Query("SELECT `id`, `name` FROM guild_house_position;");
        if (resultAvailableGH)
        {
            do {
                // commented out due to travis, but keeping for future expansion into other areas
                Field* fields = resultAvailableGH->Fetch();
                uint32 id = fields[0].GetUInt32();
                std::string name = fields[1].GetString();

                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, isSpanish ? "Casa de hermandad [" + name + "]" : "Guild House [" + name + "]", GOSSIP_SENDER_MAIN, 100 + id,
                    isSpanish ? "Comprar Casa de Hermandad [" + name + "]?" : "Buy Guild House [" + name + "]?", 0, false);
            } while (resultAvailableGH->NextRow());
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, isSpanish ? "Cerrar menu" : "Close menu", GOSSIP_SENDER_MAIN, 5);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    void TeleportGuildHouse(Guild* guild, Player* player, Creature* creature)
    {
        GuildData* guildData = player->CustomData.GetDefault<GuildData>("phase");

        QueryResult result = CharacterDatabase.PQuery("SELECT `phase`, `map`,`posX`, `posY`, `posZ` FROM guild_house WHERE `guild` = '%u'", guild->GetId());

        if (!result)
        {
            OnGossipHello(player, creature);
            return;
        }
        else
        {
            guildData->phase = (*result)[0].GetUInt32();
            uint32 map = (*result)[1].GetUInt32();
            guildData->posX = (*result)[2].GetFloat();
            guildData->posY = (*result)[3].GetFloat();
            guildData->posZ = (*result)[4].GetFloat();

            player->TeleportTo(map, guildData->posX, guildData->posY, guildData->posZ, player->GetOrientation());
        }
    }

    void SpawnNPC(uint32 entry, uint32 mapId, float posX, float posY, float posZ, float orientation, Player* player)
    {
        if (!sObjectMgr->GetCreatureTemplate(entry))
        {
            return;
        }

        Map* map = sMapMgr->FindMap(mapId, 0);
        if (!map)
        {
            return;
        }

        bool processOk = true;

        Creature* creature = new Creature();
        if (!creature->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT), map, GetGuildPhase(player), entry, 0, posX, posY, posZ, orientation))
        {
            delete creature;
            processOk = false;
        }
        else
        {
            creature->SaveToDB(mapId, (1 << map->GetSpawnMode()), GetGuildPhase(player));

            uint32 db_guid = creature->GetDBTableGUIDLow();

            creature->CleanupsBeforeDelete();
            delete creature;
            creature = new Creature();

            if (!creature->LoadCreatureFromDB(db_guid, map))
            {
                delete creature;
                processOk = false;
            }
            else
            {
                sObjectMgr->AddCreatureToGrid(db_guid, sObjectMgr->GetCreatureData(db_guid));
            }
        }
    }

    void SpawnObject(uint32 entry, uint32 mapId, float posX, float posY, float posZ, float orientation, Player* player)
    {
        uint32 objectId = entry;
        if (!objectId)
        {
            return;
        }

        const GameObjectTemplate* objectInfo = sObjectMgr->GetGameObjectTemplate(objectId);

        if (!objectInfo)
        {
            return;
        }

        if (objectInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(objectInfo->displayId))
        {
            return;
        }

        Map* map = sMapMgr->FindMap(mapId, 0);
        if (!map)
        {
            return;
        }

        bool processOk = true;

        GameObject* object = sObjectMgr->IsGameObjectStaticTransport(objectInfo->entry) ? new StaticTransport() : new GameObject();
        uint32 guidLow = sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT);

        if (!object->Create(guidLow, objectInfo->entry, map, GetGuildPhase(player), posX, posY, posZ, orientation, G3D::Quat(), 0, GO_STATE_READY))
        {
            delete object;
            processOk = false;
        }
        else
        {
            // fill the gameobject data and save to the db
            object->SaveToDB(mapId, (1 << map->GetSpawnMode()), GetGuildPhase(player));
            // delete the old object and do a clean load from DB with a fresh new GameObject instance.
            // this is required to avoid weird behavior and memory leaks
            delete object;

            object = sObjectMgr->IsGameObjectStaticTransport(objectInfo->entry) ? new StaticTransport() : new GameObject();
            // this will generate a new guid if the object is in an instance
            if (!object->LoadGameObjectFromDB(guidLow, map))
            {
                delete object;
                processOk = false;
            }
            else
            {
                // TODO: is it really necessary to add both the real and DB table guid here ?
                sObjectMgr->AddGameobjectToGrid(guidLow, sObjectMgr->GetGOData(guidLow));
            }
        }
    }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }
};

class GuildHouseV2PlayerScript : public PlayerScript
{
public:
    GuildHouseV2PlayerScript() : PlayerScript("GuildHouseV2PlayerScript") { }

    void OnLogin(Player* player)
    {
        CheckPlayer(player, player->GetZoneId());
    }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/)
    {
        CheckPlayer(player, newZone);
    }

    uint32 GetNormalPhase(Player* player) const
    {
        if (player->IsGameMaster())
            return PHASEMASK_ANYWHERE;

        uint32 phase = player->GetPhaseByAuras();
        if (!phase)
            return PHASEMASK_NORMAL;
        else
            return phase;
    }

    void CheckPlayer(Player* player, uint32 newZone)
    {
        GuildData* guildData = player->CustomData.GetDefault<GuildData>("phase");

        std::map<uint32, GuildZonesSpecificInfo>::iterator itr = GuildZonesInfo.find(player->GetGuildId());
        if (itr != GuildZonesInfo.end())
        {
            guildData->phase = itr->second.phase;
            if (player->GetZoneId() == itr->second.zoneId || newZone == itr->second.zoneId)
            {
                player->SetPhaseMask(guildData->phase, true);
                return;
            }
        }
        player->SetPhaseMask(GetNormalPhase(player), true);

        //QueryResult result = CharacterDatabase.PQuery("SELECT `phase`, `zoneId` FROM guild_house WHERE `guild` = %u", player->GetGuildId());

        //if (result)
        //{
        //    guildData->phase = (*result)[0].GetUInt32();

        //    if (player->GetZoneId() == (*result)[1].GetUInt32() || newZone == (*result)[1].GetUInt32())
        //    {
        //        player->SetPhaseMask(guildData->phase, true);
        //        return;
        //    }
        //}
        //player->SetPhaseMask(GetNormalPhase(player), true);
    }

    void TeleportToShop(Player* player)
    {
        player->TeleportTo(MAP_TANARIS, shop.GetPositionX(), shop.GetPositionY(), shop.GetPositionZ(), shop.GetOrientation());
    }

    bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }
};

class GuildHouseGlobal : public GlobalScript
{
public:
    GuildHouseGlobal() : GlobalScript("GuildHouseGlobal") {}

    void OnBeforeWorldObjectSetPhaseMask(WorldObject const* worldObject, uint32& /*oldPhaseMask*/, uint32& /*newPhaseMask*/, bool& useCombinedPhases, bool& /*update*/) override
    {
        if (GuildZones.find(worldObject->GetZoneId()) != GuildZones.end())
        {
            useCombinedPhases = false;
        }
        else
        {
            useCombinedPhases = true;
        }
    }
};

class GuildHouses_World : public WorldScript
{
public:
    GuildHouses_World() : WorldScript("GuildHouses_World") { }

    void OnStartup() override
    {
        // Load templates for Template NPC #1
        sLog->outString("== MOD GUILD HOUSES ===========================================================================");

        sLog->outString("Loading Guild House Zones...");
        QueryResult result = CharacterDatabase.Query("SELECT DISTINCT `zone` FROM `guild_house_position`;");
        if (result)
        {
            do {
                Field* fields = result->Fetch();
                uint32 zoneId = fields[0].GetUInt32();
                GuildZones.insert(zoneId);
            } while (result->NextRow());
        }

        sLog->outString("Loading Guild House Phase And Zone of Specific...");
        QueryResult resultGH = CharacterDatabase.PQuery("SELECT `phase`, `zoneId`, `guild` FROM `guild_house`;");
        if (resultGH)
        {
            do {
                Field* fields = resultGH->Fetch();
                uint32 phase = fields[0].GetUInt32();
                uint32 zoneId = fields[1].GetUInt32();
                uint32 guild = fields[2].GetUInt32();

                GuildZonesInfo[guild].phase = phase;
                GuildZonesInfo[guild].zoneId = zoneId;

            } while (resultGH->NextRow());
        }

        sLog->outString("== MOD GUILD HOUSES ===========================================================================");
    }
};

void AddGuildHouseV2Scripts() {
    new GuildHelper();
    new GuildHouses_World();
    new GuildHouseSeller();
    new GuildHouseV2PlayerScript();
    new GuildHouseGlobal();
}

