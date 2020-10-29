/*

# Enchanter NPC #

_This module was created for [StygianCore](https://rebrand.ly/stygiancoreproject). A World of Warcraft 3.3.5a Solo/LAN repack by StygianTheBest | [GitHub](https://rebrand.ly/stygiangithub) | [Website](https://rebrand.ly/stygianthebest))_

### Data ###
------------------------------------------------------------------------------------------------------------------
- Type: NPC (ID: 601015)
- Script: npc_enchantment
- Config: Yes
- SQL: No


### Version ###
------------------------------------------------------------------------------------------------------------------
- v2019.04.15 - Ported to AC by gtao725 (https://github.com/gtao725/)
- v2019.02.21 - Add AI/Phrases/Emotes, Update Menu
- v2018.12.05 - Fix broken menu. Replace 'Enchant Weapon' function. Add creature AI and creature text.
- v2018.12.01 - Update function, Add icons, Fix typos, Add a little personality (Emotes don't always work)
- v2017.08.08 - Release


### CREDITS
------------------------------------------------------------------------------------------------------------------
![Styx](https://stygianthebest.github.io/assets/img/avatar/avatar-128.jpg "Styx")
![StygianCore](https://stygianthebest.github.io/assets/img/projects/stygiancore/StygianCore.png "StygianCore")

##### This module was created for [StygianCore](https://rebrand.ly/stygiancoreproject). A World of Warcraft 3.3.5a Solo/LAN repack by StygianTheBest | [GitHub](https://rebrand.ly/stygiangithub) | [Website](https://rebrand.ly/stygianthebest))

#### Additional Credits

- [Blizzard Entertainment](http://blizzard.com)
- [TrinityCore](https://github.com/TrinityCore/TrinityCore/blob/3.3.5/THANKS)
- [SunwellCore](http://www.azerothcore.org/pages/sunwell.pl/)
- [AzerothCore](https://github.com/AzerothCore/azerothcore-wotlk/graphs/contributors)
- [OregonCore](https://wiki.oregon-core.net/)
- [Wowhead.com](http://wowhead.com)
- [OwnedCore](http://ownedcore.com/)
- [ModCraft.io](http://modcraft.io/)
- [MMO Society](https://www.mmo-society.com/)
- [AoWoW](https://wotlk.evowow.com/)
- [More credits are cited in the sources](https://github.com/StygianTheBest)

### LICENSE
------------------------------------------------------------------------------------------------------------------
This code and content is released under the [GNU AGPL v3](https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3).

*/

#include "ScriptMgr.h"
#include "Configuration/Config.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Unit.h"
#include "GameObject.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "InstanceScript.h"
#include "CombatAI.h"
#include "PassiveAI.h"
#include "Chat.h"
#include "DBCStructure.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum Enchants
{
    ENCHANT_WEP_BERSERKING = 3789,
    ENCHANT_WEP_BLADE_WARD = 3869,
    ENCHANT_WEP_BLOOD_DRAINING = 3870,
    ENCHANT_WEP_ACCURACY = 3788,
    ENCHANT_WEP_AGILITY_1H = 1103,
    ENCHANT_WEP_SPIRIT = 3844,
    ENCHANT_WEP_BATTLEMASTER = 2675,
    ENCHANT_WEP_BLACK_MAGIC = 3790,
    ENCHANT_WEP_ICEBREAKER = 3239,
    ENCHANT_WEP_LIFEWARD = 3241,
    ENCHANT_WEP_MIGHTY_SPELL_POWER = 3834, // One-hand
    ENCHANT_WEP_EXECUTIONER = 3225,
    ENCHANT_WEP_POTENCY = 3833,
    ENCHANT_WEP_TITANGUARD = 3851,
    ENCHANT_WEP_TITANIUMWEAPONCHAIN = 3731,
    ENCHANT_WEP_CRUSADER = 1900,
    ENCHANT_WEP_ICY_WEAPON = 1894,
    ENCHANT_2WEP_MASSACRE = 3827,
    ENCHANT_2WEP_SCOURGEBANE = 3247,
    ENCHANT_2WEP_GIANT_SLAYER = 3251,
    ENCHANT_2WEP_GREATER_SPELL_POWER = 3854,
    ENCHANT_2WEP_AGILITY = 2670,
    ENCHANT_2WEP_MONGOOSE = 2673,

    ENCHANT_SHIELD_DEFENSE = 1952,
    ENCHANT_SHIELD_INTELLECT = 1128,
    ENCHANT_SHIELD_RESILIENCE = 3229,
    ENCHANT_SHIELD_BLOCK = 2655,
    ENCHANT_SHIELD_STAMINA = 1071,
    ENCHANT_SHIELD_TOUGHSHIELD = 2653,
    ENCHANT_SHIELD_TITANIUM_PLATING = 3849,

    ENCHANT_HEAD_BLISSFUL_MENDING = 3819,
    ENCHANT_HEAD_BURNING_MYSTERIES = 3820,
    ENCHANT_HEAD_DOMINANCE = 3796,
    ENCHANT_HEAD_SAVAGE_GLADIATOR = 3842,
    ENCHANT_HEAD_STALWART_PROTECTOR = 3818,
    ENCHANT_HEAD_TORMENT = 3817,
    ENCHANT_HEAD_TRIUMPH = 3795,
    ENCHANT_HEAD_ECLIPSED_MOON = 3815,
    ENCHANT_HEAD_FLAME_SOUL = 3816,
    ENCHANT_HEAD_FLEEING_SHADOW = 3814,
    ENCHANT_HEAD_FROSTY_SOUL = 3812,
    ENCHANT_HEAD_TOXIC_WARDING = 3813,
    ENCHANT_HEAD_MIND_AMPLIFICATION_DASH = 3878,

    ENCHANT_SHOULDER_MASTERS_AXE = 3835,
    ENCHANT_SHOULDER_MASTERS_CRAG = 3836,
    ENCHANT_SHOULDER_MASTERS_PINNACLE = 3837,
    ENCHANT_SHOULDER_MASTERS_STORM = 3838,
    ENCHANT_SHOULDER_GREATER_AXE = 3808,
    ENCHANT_SHOULDER_GREATER_CRAG = 3809,
    ENCHANT_SHOULDER_GREATER_GLADIATOR = 3852,
    ENCHANT_SHOULDER_GREATER_PINNACLE = 3811,
    ENCHANT_SHOULDER_GREATER_STORM = 3810,
    ENCHANT_SHOULDER_DOMINANCE = 3794,
    ENCHANT_SHOULDER_TRIUMPH = 3793,

    ENCHANT_CLOAK_DARKGLOW_EMBROIDERY = 3728,
    ENCHANT_CLOAK_SWORDGUARD_EMBROIDERY = 3730,
    ENCHANT_CLOAK_LIGHTWEAVE_EMBROIDERY = 3722,
    ENCHANT_CLOAK_SPRINGY_ARACHNOWEAVE = 3859,
    ENCHANT_CLOAK_FLEXWEAVE_UNDERLAY = 3605,
    ENCHANT_CLOAK_WISDOM = 3296,
    ENCHANT_CLOAK_TITANWEAVE = 1951,
    ENCHANT_CLOAK_SPELL_PIERCING = 3243,
    ENCHANT_CLOAK_SHADOW_ARMOR = 3256,
    ENCHANT_CLOAK_MIGHTY_ARMOR = 3294,
    ENCHANT_CLOAK_MAJOR_AGILITY = 1099,
    ENCHANT_CLOAK_GREATER_SPEED = 3831,

    ENCHANT_LEG_EARTHEN = 3853,
    ENCHANT_LEG_FROSTHIDE = 3822,
    ENCHANT_LEG_ICESCALE = 3823,
    ENCHANT_LEG_BRILLIANT_SPELLTHREAD = 3719,
    ENCHANT_LEG_SAPPHIRE_SPELLTHREAD = 3721,
    ENCHANT_LEG_DRAGONSCALE = 3331,
    ENCHANT_LEG_WYRMSCALE = 3332,

    ENCHANT_GLOVES_GREATER_BLASTING = 3249,
    ENCHANT_GLOVES_ARMSMAN = 3253,
    ENCHANT_GLOVES_CRUSHER = 1603,
    ENCHANT_GLOVES_AGILITY = 3222,
    ENCHANT_GLOVES_PRECISION = 3234,
    ENCHANT_GLOVES_EXPERTISE = 3231,
    ENCHANT_GLOVES_EXTRA_SLOT = 3723,
    ENCHANT_GLOVES_SUPERIOR_SP = 3246,
    ENCHANT_GLOVES_ARMOR = 3260,

    ENCHANT_BRACERS_MAJOR_STAMINA = 3850,
    ENCHANT_BRACERS_SUPERIOR_SP = 2332,
    ENCHANT_BRACERS_GREATER_ASSUALT = 3845,
    ENCHANT_BRACERS_MAJOR_SPIRT = 1147,
    ENCHANT_BRACERS_EXPERTISE = 3231,
    ENCHANT_BRACERS_GREATER_STATS = 2661,
    ENCHANT_BRACERS_INTELLECT = 1119,
    ENCHANT_BRACERS_FURL_ARCANE = 3763,
    ENCHANT_BRACERS_FURL_FIRE = 3759,
    ENCHANT_BRACERS_FURL_FROST = 3760,
    ENCHANT_BRACERS_FURL_NATURE = 3762,
    ENCHANT_BRACERS_FURL_SHADOW = 3761,
    ENCHANT_BRACERS_FURL_ATTACK = 3756,
    ENCHANT_BRACERS_FURL_STAMINA = 3757,
    ENCHANT_BRACERS_FURL_SPELLPOWER = 3758,
    ENCHANT_BRACERS_EXTRA_SLOT = 3717,

    ENCHANT_CHEST_POWERFUL_STATS = 3832,
    ENCHANT_CHEST_SUPER_HEALTH = 3297,
    ENCHANT_CHEST_GREATER_MAINA_REST = 2381,
    ENCHANT_CHEST_EXCEPTIONAL_RESIL = 3245,
    ENCHANT_CHEST_GREATER_DEFENSE = 1953,

    ENCHANT_BOOTS_GREATER_ASSULT = 1597,
    ENCHANT_BOOTS_TUSKARS_VITLIATY = 3232,
    ENCHANT_BOOTS_SUPERIOR_AGILITY = 983,
    ENCHANT_BOOTS_GREATER_SPIRIT = 1147,
    ENCHANT_BOOTS_GREATER_VITALITY = 3244,
    ENCHANT_BOOTS_ICEWALKER = 3826,
    ENCHANT_BOOTS_GREATER_FORTITUDE = 1075,
    ENCHANT_BOOTS_NITRO_BOOTS = 3606,
    ENCHANT_BOOTS_PYRO_ROCKET = 3603,
    ENCHANT_BOOTS_HYPERSPEED = 3604,
    ENCHANT_BOOTS_ARMOR_WEBBING = 3860,

    ENCHANT_RING_ASSULT = 3839,
    ENCHANT_RING_GREATER_SP = 3840,
    ENCHANT_RING_STAMINA = 3791,

    ENCHANT_BELT_EXTRA_SLOT = 3729,
    ENCHANT_BELT_FRAG_BELT = 3601,

    ENCHANT_BOW_CRITICAL = 3608,
};

uint32 roll;
uint32 EnchanterAnnounceModule;
uint32 EnchanterNumPhrases;
uint32 EnchanterMessageTimer;
uint32 EnchanterEmoteSpell;
uint32 EnchanterEmoteCommand;

#define DEFAULT_MESSAGE 907

class EnchanterConfig : public WorldScript
{
public:
    EnchanterConfig() : WorldScript("EnchanterConfig_conf") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/npc_enchanter.conf";

#ifdef WIN32
            cfg_file = "npc_enchanter.conf";
#endif

            std::string cfg_def_file = cfg_file + ".dist";
            sConfigMgr->LoadMore(cfg_def_file.c_str());
            sConfigMgr->LoadMore(cfg_file.c_str());
            EnchanterAnnounceModule = sConfigMgr->GetBoolDefault("Enchanter.Announce", 1);
            EnchanterNumPhrases = sConfigMgr->GetIntDefault("Enchanter.NumPhrases", 3);
            EnchanterMessageTimer = sConfigMgr->GetIntDefault("Enchanter.MessageTimer", 60000);
            EnchanterEmoteSpell = sConfigMgr->GetIntDefault("Enchanter.EmoteSpell", 44940);
            EnchanterEmoteCommand = sConfigMgr->GetIntDefault("Enchanter.EmoteCommand", 3);

            // Enforce Min/Max Time
            if (EnchanterMessageTimer != 0)
            {
                if (EnchanterMessageTimer < 60000 || EnchanterMessageTimer > 300000)
                {
                    EnchanterMessageTimer = 60000;
                }
            }
        }
    }
};

class EnchanterAnnounce : public PlayerScript
{

public:

    EnchanterAnnounce() : PlayerScript("EnchanterAnnounce") {}

    void OnLogin(Player* player)
    {
        // Announce Module
        if (EnchanterAnnounceModule)
        {
            ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00EnchanterNPC |rmodule.");
        }
    }
};

class npc_enchantment : public CreatureScript
{

public:

    npc_enchantment() : CreatureScript("npc_enchantment") { }

    // Pick Phrase
    static string PickPhrase()
    {
        // Choose and speak a random phrase to the player
        // Phrases are stored in the config file
        std::string phrase = "";
        uint32 PhraseNum = urand(1, EnchanterNumPhrases); // How many phrases does the NPC speak? 
        phrase = "EC.P" + std::to_string(PhraseNum);

        // Sanitize
        if (phrase == "")
        {
            phrase = "ERROR! NPC Emote Text Not Found! Check the npc_enchanter.conf!";
        }

        std::string randMsg = sConfigMgr->GetStringDefault(phrase.c_str(), "");
        return randMsg.c_str();
    }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        bool isSpanish = IsSpanishPlayer(player);

        player->PlayerTalkClass->ClearMenus();
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Encantar Arma Principal]" : "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Enchant Main-Hand Weapon]", GOSSIP_SENDER_MAIN, 1);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Encantar Arma Secundaria]" : "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Enchant Off-Hand Weapon]", GOSSIP_SENDER_MAIN, 14);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_weapon_crossbow_38:20:20:-18|t[Encantar Arco]" : "|TInterface/ICONS/inv_weapon_crossbow_38:20:20:-18|t[Enchant Bow]", GOSSIP_SENDER_MAIN, 15);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_shield_71:20:20:-18|t[Encantar Escudo]" : "|TInterface/ICONS/Inv_shield_71:20:20:-18|t[Enchant Shield]", GOSSIP_SENDER_MAIN, 3);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_helmet_29:20:20:-18|t[Encantar Cabeza]" : "|TInterface/ICONS/inv_helmet_29:20:20:-18|t[Enchant Head]", GOSSIP_SENDER_MAIN, 4);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_shoulder_23:20:20:-18|t[Encantar Hombros]" : "|TInterface/ICONS/inv_shoulder_23:20:20:-18|t[Enchant Shoulders]", GOSSIP_SENDER_MAIN, 5);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_misc_cape_18:20:20:-18|t[Encantar Capa]" : "|TInterface/ICONS/Inv_misc_cape_18:20:20:-18|t[Enchant Cloak]", GOSSIP_SENDER_MAIN, 6);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_chest_cloth_04:20:20:-18|t[Encantar Pecho]" : "|TInterface/ICONS/inv_chest_cloth_04:20:20:-18|t[Enchant Chest]", GOSSIP_SENDER_MAIN, 7);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_bracer_14:20:20:-18|t[Encantar Brazales]" : "|TInterface/ICONS/inv_bracer_14:20:20:-18|t[Enchant Bracers]", GOSSIP_SENDER_MAIN, 8);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_gauntlets_06:20:20:-18|t[Encantar Manos]" : "|TInterface/ICONS/inv_gauntlets_06:20:20:-18|t[Enchant Gloves]", GOSSIP_SENDER_MAIN, 9);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_pants_11:20:20:-18|t[Encantar Piernas]" : "|TInterface/ICONS/inv_pants_11:20:20:-18|t[Enchant Legs]", GOSSIP_SENDER_MAIN, 10);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_boots_05:20:20:-18|t[Encantar Botas]" : "|TInterface/ICONS/inv_boots_05:20:20:-18|t[Enchant Boots]", GOSSIP_SENDER_MAIN, 11);
        player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_belt_61:20:20:-18|t[Encantar Cinturon]" : "|TInterface/ICONS/inv_belt_61:20:20:-18|t[Enchant Belt]", GOSSIP_SENDER_MAIN, 12);

        if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_jewelry_ring_85:20:20:-18|t[Encantar Anillos]" : "|TInterface/ICONS/Inv_jewelry_ring_85:20:20:-18|t[Enchant Rings]", GOSSIP_SENDER_MAIN, 13);

        player->PlayerTalkClass->SendGossipMenu(DEFAULT_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        Item* item;
        player->PlayerTalkClass->ClearMenus();
        bool isSpanish = IsSpanishPlayer(player);
        switch (action)
        {

        case 1: // Enchant Weapon
        {
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            if (!item)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada" : "This enchant requires a weapon to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }

            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Amparo de hojas" : "Blade Ward", GOSSIP_SENDER_MAIN, 102);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Cruzado" : "Crusader", GOSSIP_SENDER_MAIN, 230);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Drenador de sangre" : "Blood Draining", GOSSIP_SENDER_MAIN, 103);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Escalofrio Gelido" : "Icy Weapon", GOSSIP_SENDER_MAIN, 233);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Maestro de Batalla" : "Battlemaster", GOSSIP_SENDER_MAIN, 107);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Magia Negra" : "Black Magic", GOSSIP_SENDER_MAIN, 106);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Mangosta" : "Mongoose", GOSSIP_SENDER_MAIN, 113);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Rabiar" : "Berserking", GOSSIP_SENDER_MAIN, 104);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Resguardo de vida" : "Lifeward", GOSSIP_SENDER_MAIN, 109);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Rompehielo" : "Icebreaker", GOSSIP_SENDER_MAIN, 108);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Verdugo" : "Executioner", GOSSIP_SENDER_MAIN, 114);

            if (item->GetTemplate()->InventoryType == INVTYPE_2HWEAPON)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "81 Poder con Hechizos (2M)" : "81 Spell Power (2H)", GOSSIP_SENDER_MAIN, 115);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "35 Agilidad (2M)" : "35 Agility (2H)", GOSSIP_SENDER_MAIN, 116);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "110 Poder de Ataque (2M)" : "110 Attack Power (2H)", GOSSIP_SENDER_MAIN, 117);
            }

            player->ADD_GOSSIP_ITEM(1, isSpanish ? "25 Golpe + 25 Critico" : "25 Hit Rating + 25 Critical", GOSSIP_SENDER_MAIN, 105);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "28 Golpe - 50% Desarmar (Cadena de Titanio)" : "28 Hit Rating - 50% Disarm (Titanium Chain)", GOSSIP_SENDER_MAIN, 226);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "26 Agilidad" : "26 Agility", GOSSIP_SENDER_MAIN, 100);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "45 Espiritu" : "45 Spirit", GOSSIP_SENDER_MAIN, 101);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "50 Aguante" : "50 Stamina", GOSSIP_SENDER_MAIN, 110);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "65 Poder de Ataque" : "65 Attack Power", GOSSIP_SENDER_MAIN, 111);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "63 Poder con Hechizos" : "63 Spell Power", GOSSIP_SENDER_MAIN, 112);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100002, creature->GetGUID());
            return true;
            break;
        }
        case 2: // Enchant 2H Weapon
        {
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_2HWEAPON)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma de 2 Manos equipada." : "This enchant requires a 2H weapon to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }

            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Cruzado" : "Crusader", GOSSIP_SENDER_MAIN, 230);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Escalofrio Gelido" : "Icy Weapon", GOSSIP_SENDER_MAIN, 233);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Mangosta" : "Mongoose", GOSSIP_SENDER_MAIN, 113);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Rabiar" : "Berserking", GOSSIP_SENDER_MAIN, 104);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Verdugo" : "Executioner", GOSSIP_SENDER_MAIN, 114);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "81 Poder con Hechizos" : "81 Spell Power", GOSSIP_SENDER_MAIN, 115);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "35 Agilidad" : "35 Agility", GOSSIP_SENDER_MAIN, 116);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "110 Poder de Ataque" : "110 Attack Power", GOSSIP_SENDER_MAIN, 117);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "28 Golpe - 50% Desarmar (Cadena de Titanio)" : "28 Hit Rating - 50% Disarm (Titanium Chain)", GOSSIP_SENDER_MAIN, 226);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100003, creature->GetGUID());

            return true;
            break;
        }
        case 3: // Enchant Shield
        {
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_SHIELD)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere tener equipado un escudo." : "This enchant requires a shield to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }

            player->ADD_GOSSIP_ITEM(1, isSpanish ? "20 Defensa" : "20 Defense", GOSSIP_SENDER_MAIN, 118);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "25 Intelecto" : "25 Intellect", GOSSIP_SENDER_MAIN, 119);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "12 Temple" : "12 Resilience", GOSSIP_SENDER_MAIN, 120);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "36 Bloqueo" : "36 Block", GOSSIP_SENDER_MAIN, 121);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "18 Aguante" : "18 Stamina", GOSSIP_SENDER_MAIN, 122);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "81 Bloqueo + 50% Desarmar Reducido" : "81 Block + 50% Less Disarm", GOSSIP_SENDER_MAIN, 123);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);

            player->PlayerTalkClass->SendGossipMenu(100004, creature->GetGUID());
            return true;
            break;
        }
        case 4: // Enchant Head
        {
            if (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Disco de amplificación mental (+45 Aguante + control mental enemigos)" : "Mind Amplification Dash (+45p Stamina + enemy mind control)", GOSSIP_SENDER_MAIN, 235);
            }
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "50 Poder de Ataque + 20 Critico" : "50 Attack Power + 20 Crit", GOSSIP_SENDER_MAIN, 129);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "50 Poder de Ataque + 20 Temple" : "50 Attack Power + 20 Resilience", GOSSIP_SENDER_MAIN, 130);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "30 Poder con Hechizos + 20 Critico" : "30 Spell Power + 20 Crit", GOSSIP_SENDER_MAIN, 125);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "29 Poder con Hechizos + 20 Temple" : "29 Spell Power + 20 Resilience", GOSSIP_SENDER_MAIN, 126);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "30 Poder con Hechizos + 10 Mp5" : "30 Spell Power + 10 Mp5", GOSSIP_SENDER_MAIN, 124);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "37 Aguante + 20 Defensa" : "37 Stamina + 20 Defense", GOSSIP_SENDER_MAIN, 128);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "30 Aguante + 25 Temple" : "30 Stamina + 25 Resilience", GOSSIP_SENDER_MAIN, 127);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Arcanum de la luna eclipsada" : "Arcanum of Eclipsed Moon", GOSSIP_SENDER_MAIN, 131);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Arcanum del alma de la llama" : "Arcanum of the Flame's Soul", GOSSIP_SENDER_MAIN, 132);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Arcanum de la sombra huida" : "Arcanum of the Fleeing Shadow", GOSSIP_SENDER_MAIN, 133);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Arcanum del alma helada" : "Arcanum of the Frosty Soul", GOSSIP_SENDER_MAIN, 134);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Arcanum de proteccion toxica" : "Arcanum of Toxic Warding", GOSSIP_SENDER_MAIN, 135);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100005, creature->GetGUID());
            return true;
            break;
        }
        case 5: // Enchant Shoulders
        {
            if (player->HasSkill(SKILL_INSCRIPTION) && player->GetSkillValue(SKILL_INSCRIPTION) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "120 Poder de Ataque + 15 Critico" : "120 Attack Power + 15 Crit", GOSSIP_SENDER_MAIN, 136);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "70 Poder con Hechizos" : "70 Spell Power + 8 Mp5", GOSSIP_SENDER_MAIN, 137);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "60 Esquivar + 15 Defensa" : "60 Dodge + 15 Defense", GOSSIP_SENDER_MAIN, 138);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "70 Poder con Hechizos + 15 Critico" : "70 Spell Power + 15 Crit", GOSSIP_SENDER_MAIN, 139);
            }
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "40 Poder de Ataque + 15 Critico" : "40 Attack Power + 15 Crit", GOSSIP_SENDER_MAIN, 140);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "40 Poder de Ataque + 15 Temple" : "40 Attack Power + 15 Resilience", GOSSIP_SENDER_MAIN, 146);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "24 Poder con Hechizos + 15 Critico" : "24 Spell Power + 15 Crit", GOSSIP_SENDER_MAIN, 144);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "23 Poder con Hechizos + 15 Temple" : "23 Spell Power + 15 Resilience", GOSSIP_SENDER_MAIN, 145);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "23 Poder con Hechizos + 8 Mp5" : "24 Spell Power + 8 Mp5", GOSSIP_SENDER_MAIN, 141);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "30 Aguante + 15 Temple" : "30 Stamina + 15 Resilience", GOSSIP_SENDER_MAIN, 142);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "20 Esquivar + 15 Defensa" : "20 Dodge + 15 Defense", GOSSIP_SENDER_MAIN, 143);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100006, creature->GetGUID());
            return true;
            break;
        }
        case 6: // Enchant Cloak
        {
            if (player->HasSkill(SKILL_TAILORING) && player->GetSkillValue(SKILL_TAILORING) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Bordado de resplandor oscuro (Mana)" : "Darkglow Embroidery (Mana)", GOSSIP_SENDER_MAIN, 149);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Bordado de tejido de luz (Poder con Hechizos)" : "Lightweave Embroidery (Spell Power)", GOSSIP_SENDER_MAIN, 150);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Bordado de guardia de espada (Poder de Ataque)" : "Swordguard Embroidery (Attack Power)", GOSSIP_SENDER_MAIN, 151);
            }
            if (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Paracaidas (+28 Poder con Hechizos)" : "Parachute (+28 Spell Power)", GOSSIP_SENDER_MAIN, 147);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Paracaidas (+23 Agilidad)" : "Parachute (+23 Agility)", GOSSIP_SENDER_MAIN, 229);
            }
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Armadura de las sombras (Sigilo + 10 Agilidad)" : "Shadow Armor (Sigilo + 10 Agility)", GOSSIP_SENDER_MAIN, 148);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "22 Agilidad" : "22 Agility", GOSSIP_SENDER_MAIN, 156);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "23 Celeridad" : "23 Haste", GOSSIP_SENDER_MAIN, 157);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "35 Penetracion de Hechizos" : "35 Spell Penetration", GOSSIP_SENDER_MAIN, 154);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "10 Espiritu + 2% Amenaza Reducida" : "10 Spirit + 2% Reduced Threat", GOSSIP_SENDER_MAIN, 152);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "16 Defensa" : "16 Defense", GOSSIP_SENDER_MAIN, 153);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "225 Armadura" : "225 Armor", GOSSIP_SENDER_MAIN, 155);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100007, creature->GetGUID());
            return true;
            break;
        }
        case 7: //Enchant chest
        {
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "+10 Todas Estadisticas" : "+10 All Stats", GOSSIP_SENDER_MAIN, 158);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "20 Temple" : "20 Resilience", GOSSIP_SENDER_MAIN, 161);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "225 Salud" : "225 Health", GOSSIP_SENDER_MAIN, 159);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "10 Mp5" : "10 Mp5", GOSSIP_SENDER_MAIN, 160);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "22 Defensa" : "22 Defense", GOSSIP_SENDER_MAIN, 162);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100008, creature->GetGUID());
            return true;
            break;
        }
        case 8: //Enchant Bracers
        {
            if (player->HasSkill(SKILL_BLACKSMITHING) && player->GetSkillValue(SKILL_BLACKSMITHING) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Gema Extra" : "Extra Gem Slot", GOSSIP_SENDER_MAIN, 206);
            }
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "50 Poder de Ataque" : "50 Attack Power", GOSSIP_SENDER_MAIN, 165);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "30 Poder con Hechizos" : "30 Spell Power", GOSSIP_SENDER_MAIN, 164);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "40 Aguante" : "40 Stamina", GOSSIP_SENDER_MAIN, 163);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "18 Espiritu" : "18 Spirit", GOSSIP_SENDER_MAIN, 166);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "15 Pericia" : "15 Expertise", GOSSIP_SENDER_MAIN, 167);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "16 Intelecto" : "16 Intellect", GOSSIP_SENDER_MAIN, 169);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "+6 Todas Estadisticas" : "+6 All Stats", GOSSIP_SENDER_MAIN, 168);

            if (player->HasSkill(SKILL_LEATHERWORKING) && player->GetSkillValue(SKILL_LEATHERWORKING) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Forro de Piel - Resistencia Arcana" : "Fur Lining - Arcane Resist", GOSSIP_SENDER_MAIN, 170);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Forro de Piel - Resistencia Fuego" : "Fur Lining - Fire Resist", GOSSIP_SENDER_MAIN, 171);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Forro de Piel - Resistencia Escarcha" : "Fur Lining - Frost Resist", GOSSIP_SENDER_MAIN, 172);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Forro de Piel - Resistencia Naturaleza" : "Fur Lining - Nature Resist", GOSSIP_SENDER_MAIN, 173);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Forro de Piel - Resistencia Sombras" : "Fur Lining - Shadow Resist", GOSSIP_SENDER_MAIN, 174);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Forro de Piel - Poder de Ataque" : "Fur Lining - Attack Power", GOSSIP_SENDER_MAIN, 175);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Forro de Piel - Poder con Hechizos" : "Fur Lining - Spellpower", GOSSIP_SENDER_MAIN, 177);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Forro de Piel - Aguante" : "Fur Lining - Stamina", GOSSIP_SENDER_MAIN, 176);
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100009, creature->GetGUID());
            return true;
            break;
        }
        case 9: //Enchant Gloves
        {
            if (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Pirocohete Manual" : "Hand-Mounted Pyro Rocket", GOSSIP_SENDER_MAIN, 199);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Aceleradores de Hipervelocidad (Celeridad)" : "Hyperspeed Accelerators (Haste)", GOSSIP_SENDER_MAIN, 200);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Cincha de Armadura Reticulada (Armadura)" : "Reticulated Armor Webbing (Armor)", GOSSIP_SENDER_MAIN, 201);

            }
            if (player->HasSkill(SKILL_BLACKSMITHING) && player->GetSkillValue(SKILL_BLACKSMITHING) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Gema Extra" : "Extra Gem Slot", GOSSIP_SENDER_MAIN, 207);
            }

            player->ADD_GOSSIP_ITEM(1, isSpanish ? "44 Poder de Ataque" : "44 Attack Power", GOSSIP_SENDER_MAIN, 180);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "28 Poder con Hechizos" : "28 Spell Power", GOSSIP_SENDER_MAIN, 208);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "20 Agilidad" : "20 Agility", GOSSIP_SENDER_MAIN, 181);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "20 Golpe" : "20 Hit Rating", GOSSIP_SENDER_MAIN, 182);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "16 Critico" : "16 Critical Strike", GOSSIP_SENDER_MAIN, 178);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "2% Amenaza + 10 Parada" : "2% Threat + 10 Parry", GOSSIP_SENDER_MAIN, 179);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "15 Pericia" : "15 Expertise", GOSSIP_SENDER_MAIN, 183);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "240 Armadura" : "240 Armor", GOSSIP_SENDER_MAIN, 224);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100010, creature->GetGUID());
            return true;
            break;
        }
        case 10: //Enchant legs
        {
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "75 Poder de Ataque + 22 Critico" : "75 Attack Power + 22 Critical", GOSSIP_SENDER_MAIN, 186);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "50 Poder con Hechizos + 22 Espiritu" : "50 Spell Power + 22 Spirit", GOSSIP_SENDER_MAIN, 187);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "50 Poder con Hechizos + 30 Aguante" : "50 Spell Power + 30 Stamina", GOSSIP_SENDER_MAIN, 188);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "55 Aguante + 22 Agilidad" : "55 Stamina + 22 Agility", GOSSIP_SENDER_MAIN, 185);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "40 Temple + 28 Aguante" : "40 Resilience + 28 Stamina", GOSSIP_SENDER_MAIN, 184);
            //player->ADD_GOSSIP_ITEM(1, "72 Stamina + 35 Agility", GOSSIP_SENDER_MAIN, 189);
            //player->ADD_GOSSIP_ITEM(1, "100 Attack Power + 36 Critical", GOSSIP_SENDER_MAIN, 190);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100011, creature->GetGUID());
            return true;
            break;
        }
        case 11: //Enchant Boots
        {
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "32 Poder de Ataque" : "32 Attack Power", GOSSIP_SENDER_MAIN, 191);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "15 Aguante + Velocidad Incrementada" : "15 Stamina + Minor Speed Increase", GOSSIP_SENDER_MAIN, 192);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "16 Agilidad" : "16 Agility", GOSSIP_SENDER_MAIN, 193);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "18 Espiritu" : "18 Spirit", GOSSIP_SENDER_MAIN, 194);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "12 Golpe + 12 Critico" : "12 Hit Rating + 12 Critical", GOSSIP_SENDER_MAIN, 196);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "22 Aguante" : "22 Stamina", GOSSIP_SENDER_MAIN, 197);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Restaura 7 Vida + Mp5" : "Restore 7 Health + Mp5", GOSSIP_SENDER_MAIN, 195);
            if (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Propulsiones de Nitro" : "Nitro Boots", GOSSIP_SENDER_MAIN, 198);
            }
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100012, creature->GetGUID());
            return true;
            break;
        }
        case 12: //Enchant belt
        {
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "Ranura Extra" : "Extra Slot", GOSSIP_SENDER_MAIN, 205);
            if (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) == 450)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Cinturon de fragmentacion (Bomba)" : "Frag Belt (Bomb)", GOSSIP_SENDER_MAIN, 228);
            }
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100013, creature->GetGUID());
            return true;
            break;
        }
        case 13: //Enchant rings
        {
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "40 Poder de Ataque" : "40 Attack Power", GOSSIP_SENDER_MAIN, 202);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "23 Poder con Hechizos" : "23 Spell Power", GOSSIP_SENDER_MAIN, 203);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "30 Aguante" : "30 Stamina", GOSSIP_SENDER_MAIN, 204);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            player->PlayerTalkClass->SendGossipMenu(100014, creature->GetGUID());
            return true;
            break;
        }
        case 14: // Enchant Off-Hand Weapon
        {
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            if (item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Amparo de hojas" : "Blade Ward", GOSSIP_SENDER_MAIN, 211);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Cruzado" : "Crusader", GOSSIP_SENDER_MAIN, 231);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Drenador de sangre" : "Blood Draining", GOSSIP_SENDER_MAIN, 212);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Escalofrio Gelido" : "Icy Weapon", GOSSIP_SENDER_MAIN, 234);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Maestro de Gatalla" : "Battlemaster", GOSSIP_SENDER_MAIN, 216);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Magia Negra" : "Black Magic", GOSSIP_SENDER_MAIN, 215);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Mangosta" : "Mongoose", GOSSIP_SENDER_MAIN, 222);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Rabiar" : "Berserking", GOSSIP_SENDER_MAIN, 213);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Resguardo de vida" : "Lifeward", GOSSIP_SENDER_MAIN, 218);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Rompehielo" : "Icebreaker", GOSSIP_SENDER_MAIN, 217);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Verdugo" : "Executioner", GOSSIP_SENDER_MAIN, 223);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "25 Golpe + 25 Critico" : "25 Hit Rating + 25 Critical", GOSSIP_SENDER_MAIN, 214);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "28 Golpe - 50% Desarmar (Cadena de Titanio)" : "28 Hit Rating - 50% Disarm (Titanium Chain)", GOSSIP_SENDER_MAIN, 227);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "26 Agilidad" : "26 Agility", GOSSIP_SENDER_MAIN, 209);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "45 Espiritu" : "45 Spirit", GOSSIP_SENDER_MAIN, 210);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "50 Aguante" : "50 Stamina", GOSSIP_SENDER_MAIN, 219);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "65 Poder de Ataque" : "65 Attack Power", GOSSIP_SENDER_MAIN, 220);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "63 Poder con Hechizos" : "63 Spell Power", GOSSIP_SENDER_MAIN, 221);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            }
            else if (item->GetTemplate()->InventoryType == INVTYPE_2HWEAPON)
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Rabiar" : "Berserking", GOSSIP_SENDER_MAIN, 213);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Cruzado" : "Crusader", GOSSIP_SENDER_MAIN, 231);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Verdugo" : "Executioner", GOSSIP_SENDER_MAIN, 223);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Escalofrio Gelido" : "Icy Weapon", GOSSIP_SENDER_MAIN, 234);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "Mangosta" : "Mongoose", GOSSIP_SENDER_MAIN, 222);
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "110 Poder de Ataque" : "110 Attack Power", GOSSIP_SENDER_MAIN, 232);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            }
            else
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            player->PlayerTalkClass->SendGossipMenu(100015, creature->GetGUID());
            return true;
            break;
        }
        case 15: // Enchant Bow
        {
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
            if (!item)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma a distancia equipada." : "This enchant needs a ranged weapon to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            if ((item->GetTemplate()->InventoryType == INVTYPE_RANGED || item->GetTemplate()->InventoryType == INVTYPE_RANGEDRIGHT)
                && (item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_BOW || item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_GUN || item->GetTemplate()->SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW))
            {
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "40 Critico a Distancia" : "40 Ranged Critical Strike", GOSSIP_SENDER_MAIN, 225);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, isSpanish ? "Atras" : "Back", GOSSIP_SENDER_MAIN, 300);
            }
            else
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma a distancia equipada." : "This enchant needs a ranged weapon to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            player->PlayerTalkClass->SendGossipMenu(100016, creature->GetGUID());
            return true;
            break;
        }
        case 100:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_AGILITY_1H);
            break;

        case 101:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_SPIRIT);
            break;

        case 102:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BLADE_WARD);
            break;

        case 103:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BLOOD_DRAINING);
            break;

        case 104:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BERSERKING);
            break;

        case 105:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_ACCURACY);
            break;

        case 106:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BLACK_MAGIC);
            break;

        case 107:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BATTLEMASTER);
            break;

        case 108:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_ICEBREAKER);
            break;

        case 109:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_LIFEWARD);
            break;

        case 110:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_TITANGUARD);
            break;

        case 111:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_POTENCY);
            break;

        case 112:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_MIGHTY_SPELL_POWER);
            break;

        case 113:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_MONGOOSE);
            break;

        case 114:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_EXECUTIONER);
            break;

        case 115:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_2HWEAPON)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma de 2 Manos equipada." : "This enchant requires a 2H weapon to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_GREATER_SPELL_POWER);
            break;

        case 116:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_2HWEAPON)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma de 2 Manos equipada." : "This enchant requires a 2H weapon to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_AGILITY);
            break;

        case 117:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_2HWEAPON)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma de 2 Manos equipada." : "This enchant requires a 2H weapon to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_2WEP_MASSACRE);
            break;

        case 118:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_SHIELD)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere tener equipado un escudo." : "This enchant requires a shield to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_DEFENSE);
            break;

        case 119:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_SHIELD)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere tener equipado un escudo." : "This enchant requires a shield to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_INTELLECT);
            break;

        case 120:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_SHIELD)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere tener equipado un escudo." : "This enchant requires a shield to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_RESILIENCE);
            break;

        case 121:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_SHIELD)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere tener equipado un escudo." : "This enchant requires a shield to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_TOUGHSHIELD);
            break;

        case 122:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_SHIELD)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere tener equipado un escudo." : "This enchant requires a shield to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_STAMINA);
            break;

        case 123:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_SHIELD)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere tener equipado un escudo." : "This enchant requires a shield to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_TITANIUM_PLATING);
            break;

        case 124:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_BLISSFUL_MENDING);
            break;

        case 125:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_BURNING_MYSTERIES);
            break;

        case 126:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_DOMINANCE);
            break;

        case 127:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_SAVAGE_GLADIATOR);
            break;

        case 128:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_STALWART_PROTECTOR);
            break;

        case 129:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_TORMENT);
            break;

        case 130:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_TRIUMPH);
            break;

        case 131:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_ECLIPSED_MOON);
            break;

        case 132:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_FLAME_SOUL);
            break;

        case 133:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_FLEEING_SHADOW);
            break;

        case 134:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_FROSTY_SOUL);
            break;

        case 135:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_TOXIC_WARDING);
            break;

        case 136:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_AXE);
            break;

        case 137:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_CRAG);
            break;

        case 138:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_PINNACLE);
            break;

        case 139:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_STORM);
            break;

        case 140:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_AXE);
            break;

        case 141:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_CRAG);
            break;

        case 142:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_GLADIATOR);
            break;

        case 143:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_PINNACLE);
            break;

        case 144:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_STORM);
            break;

        case 145:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_DOMINANCE);
            break;

        case 146:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_TRIUMPH);
            break;

        case 147:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SPRINGY_ARACHNOWEAVE);
            break;

        case 148:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SHADOW_ARMOR);
            break;

        case 149:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_DARKGLOW_EMBROIDERY);
            break;

        case 150:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_LIGHTWEAVE_EMBROIDERY);
            break;

        case 151:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SWORDGUARD_EMBROIDERY);
            break;

        case 152:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_WISDOM);
            break;

        case 153:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_TITANWEAVE);
            break;

        case 154:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SPELL_PIERCING);
            break;

        case 155:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_MIGHTY_ARMOR);
            break;

        case 156:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_MAJOR_AGILITY);
            break;

        case 157:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_GREATER_SPEED);
            break;

        case 158:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_POWERFUL_STATS);
            break;

        case 159:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_SUPER_HEALTH);
            break;

        case 160:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_GREATER_MAINA_REST);
            break;

        case 161:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_EXCEPTIONAL_RESIL);
            break;

        case 162:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_GREATER_DEFENSE);
            break;

        case 163:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_MAJOR_STAMINA);
            break;

        case 164:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_SUPERIOR_SP);
            break;

        case 165:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_GREATER_ASSUALT);
            break;

        case 166:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_MAJOR_SPIRT);
            break;

        case 167:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_EXPERTISE);
            break;

        case 168:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_GREATER_STATS);
            break;

        case 169:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_INTELLECT);
            break;

        case 170:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_ARCANE);
            break;

        case 171:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_FIRE);
            break;

        case 172:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_FROST);
            break;

        case 173:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_NATURE);
            break;

        case 174:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_SHADOW);
            break;

        case 175:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_ATTACK);
            break;

        case 176:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_STAMINA);
            break;

        case 177:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_SPELLPOWER);
            break;

        case 178:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_GREATER_BLASTING);
            break;

        case 179:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_ARMSMAN);
            break;

        case 180:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_CRUSHER);
            break;

        case 181:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_AGILITY);
            break;

        case 182:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_PRECISION);
            break;

        case 183:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_EXPERTISE);
            break;

        case 184:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_EARTHEN);
            break;

        case 185:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_FROSTHIDE);
            break;

        case 186:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_ICESCALE);
            break;

        case 187:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_BRILLIANT_SPELLTHREAD);
            break;

        case 188:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_SAPPHIRE_SPELLTHREAD);
            break;

        case 189:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_DRAGONSCALE);
            break;

        case 190:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_WYRMSCALE);
            break;

        case 191:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_ASSULT);
            break;

        case 192:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_TUSKARS_VITLIATY);
            break;

        case 193:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_SUPERIOR_AGILITY);
            break;

        case 194:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_SPIRIT);
            break;

        case 195:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_VITALITY);
            break;

        case 196:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_ICEWALKER);
            break;

        case 197:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_FORTITUDE);
            break;

        case 198:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_NITRO_BOOTS);
            break;

        case 199:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_BOOTS_PYRO_ROCKET);
            break;

        case 200:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_BOOTS_HYPERSPEED);
            break;

        case 201:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_BOOTS_ARMOR_WEBBING);
            break;

        case 202:
            if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
            {
                Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER1), ENCHANT_RING_ASSULT);
                Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2), ENCHANT_RING_ASSULT);
            }
            break;

        case 203:
            if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
            {
                Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER1), ENCHANT_RING_GREATER_SP);
                Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2), ENCHANT_RING_GREATER_SP);
            }
            break;

        case 204:
            if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
            {
                Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER1), ENCHANT_RING_STAMINA);
                Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2), ENCHANT_RING_STAMINA);
            }
            break;

        case 205:
            EnchantPrismaticSlot(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_EXTRA_SLOT);
            break;

        case 206:
            EnchantPrismaticSlot(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_EXTRA_SLOT);
            break;

        case 207:
            EnchantPrismaticSlot(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_EXTRA_SLOT);
            break;

        case 208:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_SUPERIOR_SP);
            break;

        case 209:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_AGILITY_1H);
            break;
        case 210:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_SPIRIT);
            break;
        case 211:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_BLADE_WARD);
            break;
        case 212:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_BLOOD_DRAINING);
            break;
        case 213:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND
                || item->GetTemplate()->InventoryType == INVTYPE_2HWEAPON))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_BERSERKING);
            break;
        case 214:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_ACCURACY);
            break;
        case 215:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_BLACK_MAGIC);
            break;
        case 216:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_BATTLEMASTER);
            break;
        case 217:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_ICEBREAKER);
            break;
        case 218:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_LIFEWARD);
            break;
        case 219:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_TITANGUARD);
            break;
        case 220:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_POTENCY);
            break;
        case 221:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_MIGHTY_SPELL_POWER);
            break;
        case 222:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND
                || item->GetTemplate()->InventoryType == INVTYPE_2HWEAPON))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_2WEP_MONGOOSE);
            break;
        case 223:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND
                || item->GetTemplate()->InventoryType == INVTYPE_2HWEAPON))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_EXECUTIONER);
            break;
        case 224:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_ARMOR);
            break;
        case 225:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED), ENCHANT_BOW_CRITICAL);
            break;
        case 226:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_TITANIUMWEAPONCHAIN);
            break;
        case 227:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_TITANIUMWEAPONCHAIN);
            break;
        case 228:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST), ENCHANT_BELT_FRAG_BELT);
            break;
        case 229:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_FLEXWEAVE_UNDERLAY);
            break;
        case 230:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_CRUSADER);
            break;
        case 231:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND
                || item->GetTemplate()->InventoryType == INVTYPE_2HWEAPON))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_CRUSADER);
            break;
        case 232:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->GetTemplate()->InventoryType != INVTYPE_2HWEAPON)
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma de 2 Manos equipada." : "This enchant requires a 2H weapon to be equipped.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_2WEP_MASSACRE);
            break;

        case 233:
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_ICY_WEAPON);
            break;
        case 234:
            item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || !(item->GetTemplate()->InventoryType == INVTYPE_WEAPON || item->GetTemplate()->InventoryType == INVTYPE_WEAPONMAINHAND || item->GetTemplate()->InventoryType == INVTYPE_WEAPONOFFHAND
                || item->GetTemplate()->InventoryType == INVTYPE_2HWEAPON))
            {
                creature->MonsterWhisper(isSpanish ? "Este encantamiento requiere un arma equipada en la mano secundaria." : "This enchant needs a weapon equipped in the off-hand.", player, 0);
                player->PlayerTalkClass->SendCloseGossip();
                return false;
            }
            Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_ICY_WEAPON);
            break;
        case 235:
            if (player->HasSkill(SKILL_ENGINEERING) && player->GetSkillValue(SKILL_ENGINEERING) == 450)
            {
                Enchant(player, creature, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_MIND_AMPLIFICATION_DASH);
            }
            break;
        case 300:
        {
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Encantar Arma Principal]" : "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Enchant Main-Hand Weapon]", GOSSIP_SENDER_MAIN, 1);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Encantar Arma Secundaria]" : "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Enchant Off-Hand Weapon]", GOSSIP_SENDER_MAIN, 14);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_weapon_crossbow_38:20:20:-18|t[Encantar Arco]" : "|TInterface/ICONS/inv_weapon_crossbow_38:20:20:-18|t[Enchant Bow]", GOSSIP_SENDER_MAIN, 15);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_shield_71:20:20:-18|t[Encantar Escudo]" : "|TInterface/ICONS/Inv_shield_71:20:20:-18|t[Enchant Shield]", GOSSIP_SENDER_MAIN, 3);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_helmet_29:20:20:-18|t[Encantar Cabeza]" : "|TInterface/ICONS/inv_helmet_29:20:20:-18|t[Enchant Head]", GOSSIP_SENDER_MAIN, 4);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_shoulder_23:20:20:-18|t[Encantar Hombros]" : "|TInterface/ICONS/inv_shoulder_23:20:20:-18|t[Enchant Shoulders]", GOSSIP_SENDER_MAIN, 5);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_misc_cape_18:20:20:-18|t[Encantar Capa]" : "|TInterface/ICONS/Inv_misc_cape_18:20:20:-18|t[Enchant Cloak]", GOSSIP_SENDER_MAIN, 6);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_chest_cloth_04:20:20:-18|t[Encantar Pecho]" : "|TInterface/ICONS/inv_chest_cloth_04:20:20:-18|t[Enchant Chest]", GOSSIP_SENDER_MAIN, 7);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_bracer_14:20:20:-18|t[Encantar Brazales]" : "|TInterface/ICONS/inv_bracer_14:20:20:-18|t[Enchant Bracers]", GOSSIP_SENDER_MAIN, 8);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_gauntlets_06:20:20:-18|t[Encantar Manos]" : "|TInterface/ICONS/inv_gauntlets_06:20:20:-18|t[Enchant Gloves]", GOSSIP_SENDER_MAIN, 9);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_pants_11:20:20:-18|t[Encantar Piernas]" : "|TInterface/ICONS/inv_pants_11:20:20:-18|t[Enchant Legs]", GOSSIP_SENDER_MAIN, 10);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_boots_05:20:20:-18|t[Encantar Botas]" : "|TInterface/ICONS/inv_boots_05:20:20:-18|t[Enchant Boots]", GOSSIP_SENDER_MAIN, 11);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_belt_61:20:20:-18|t[Encantar Cinturon]" : "|TInterface/ICONS/inv_belt_61:20:20:-18|t[Enchant Belt]", GOSSIP_SENDER_MAIN, 12);

            if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_jewelry_ring_85:20:20:-18|t[Encantar Anillos]" : "|TInterface/ICONS/Inv_jewelry_ring_85:20:20:-18|t[Enchant Rings]", GOSSIP_SENDER_MAIN, 13);


            player->PlayerTalkClass->SendGossipMenu(100001, creature->GetGUID());
            return true;
            break;
        }
        case 0:
        {
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Encantar Arma Principal]" : "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Enchant Main-Hand Weapon]", GOSSIP_SENDER_MAIN, 1);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Encantar Arma Secundaria]" : "|TInterface/ICONS/Inv_mace_116:20:20:-18|t[Enchant Off-Hand Weapon]", GOSSIP_SENDER_MAIN, 14);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_weapon_crossbow_38:20:20:-18|t[Encantar Arco]" : "|TInterface/ICONS/inv_weapon_crossbow_38:20:20:-18|t[Enchant Bow]", GOSSIP_SENDER_MAIN, 15);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_shield_71:20:20:-18|t[Encantar Escudo]" : "|TInterface/ICONS/Inv_shield_71:20:20:-18|t[Enchant Shield]", GOSSIP_SENDER_MAIN, 3);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_helmet_29:20:20:-18|t[Encantar Cabeza]" : "|TInterface/ICONS/inv_helmet_29:20:20:-18|t[Enchant Head]", GOSSIP_SENDER_MAIN, 4);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_shoulder_23:20:20:-18|t[Encantar Hombros]" : "|TInterface/ICONS/inv_shoulder_23:20:20:-18|t[Enchant Shoulders]", GOSSIP_SENDER_MAIN, 5);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_misc_cape_18:20:20:-18|t[Encantar Capa]" : "|TInterface/ICONS/Inv_misc_cape_18:20:20:-18|t[Enchant Cloak]", GOSSIP_SENDER_MAIN, 6);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_chest_cloth_04:20:20:-18|t[Encantar Pecho]" : "|TInterface/ICONS/inv_chest_cloth_04:20:20:-18|t[Enchant Chest]", GOSSIP_SENDER_MAIN, 7);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_bracer_14:20:20:-18|t[Encantar Brazales]" : "|TInterface/ICONS/inv_bracer_14:20:20:-18|t[Enchant Bracers]", GOSSIP_SENDER_MAIN, 8);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_gauntlets_06:20:20:-18|t[Encantar Manos]" : "|TInterface/ICONS/inv_gauntlets_06:20:20:-18|t[Enchant Gloves]", GOSSIP_SENDER_MAIN, 9);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_pants_11:20:20:-18|t[Encantar Piernas]" : "|TInterface/ICONS/inv_pants_11:20:20:-18|t[Enchant Legs]", GOSSIP_SENDER_MAIN, 10);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_boots_05:20:20:-18|t[Encantar Botas]" : "|TInterface/ICONS/inv_boots_05:20:20:-18|t[Enchant Boots]", GOSSIP_SENDER_MAIN, 11);
            player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/inv_belt_61:20:20:-18|t[Encantar Cinturon]" : "|TInterface/ICONS/inv_belt_61:20:20:-18|t[Enchant Belt]", GOSSIP_SENDER_MAIN, 12);

            if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
                player->ADD_GOSSIP_ITEM(1, isSpanish ? "|TInterface/ICONS/Inv_jewelry_ring_85:20:20:-18|t[Encantar Anillos]" : "|TInterface/ICONS/Inv_jewelry_ring_85:20:20:-18|t[Enchant Rings]", GOSSIP_SENDER_MAIN, 13);

            player->PlayerTalkClass->SendGossipMenu(DEFAULT_MESSAGE, creature->GetGUID());
            return true;
            break;
        }
        }


        //player->PlayerTalkClass->SendCloseGossip();
        return true;
    }

    void Enchant(Player* player, Creature* creature, Item* item, uint32 enchantid)
    {
        bool isSpanish = IsSpanishPlayer(player);
        if (!item)
        {
            creature->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
            creature->MonsterWhisper(isSpanish ? "Por favor, equipa el objeto que deseas encantar." : "Please equip the item you would like to enchant!", player, 0);
            player->PlayerTalkClass->SendCloseGossip();
            return;
        }

        if (!enchantid)
        {          
            player->GetSession()->SendNotification(isSpanish ? "Algo fue mal en el modulo de encantamiento. Escribe un mensaje a los desarrolladores notificando el error para poder solucionar el problema."
                : "Something went wrong in the code. It has been logged for developers and will be looked into, sorry for the inconvenience.");
            player->PlayerTalkClass->SendCloseGossip();
            creature->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
            return;
        }

        player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, false);
        item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
        item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);
        player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, true);

        player->GetSession()->SendNotification(isSpanish ? "|cff00ff00Se ha encantado |cffDA70D6%s|cff00ff00!" : "|cff00ff00Sucessfully enchanted |cffDA70D6%s|cff00ff00!", item->GetTemplate()->Name1.c_str());
        creature->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);

        OnGossipHello(player, creature);
    }

    void EnchantPrismaticSlot(Player* player, Creature* creature, Item* item, uint32 enchantid)
    {
        bool isSpanish = IsSpanishPlayer(player);
        if (!item)
        {
            creature->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
            creature->MonsterWhisper(isSpanish ? "Por favor, equipa el objeto que deseas encantar." : "Please equip the item you would like to enchant!", player, 0);
            player->PlayerTalkClass->SendCloseGossip();
            return;
        }

        if (!enchantid)
        {
            player->GetSession()->SendNotification(isSpanish ? "Algo fue mal en el modulo de encantamiento. Escribe un mensaje a los desarrolladores notificando el error para poder solucionar el problema."
                : "Something went wrong in the code. It has been logged for developers and will be looked into, sorry for the inconvenience.");
            player->PlayerTalkClass->SendCloseGossip();
            creature->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
            return;
        }

        item->ClearEnchantment(PRISMATIC_ENCHANTMENT_SLOT);
        item->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, enchantid, 0, 0);

        player->GetSession()->SendNotification(isSpanish ? "|cff00ff00Se ha encantado |cffDA70D6%s|cff00ff00!" : "|cff00ff00Sucessfully enchanted |cffDA70D6%s|cff00ff00!", item->GetTemplate()->Name1.c_str());
        creature->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);

        OnGossipHello(player, creature);
    }

    // Passive Emotes
    struct NPC_PassiveAI : public ScriptedAI
    {
        NPC_PassiveAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 MessageTimer;

        // Called once when client is loaded
        void Reset()
        {
            MessageTimer = urand(EnchanterMessageTimer, 300000); // 1-5 minutes
        }

        // Called at World update tick
        void UpdateAI(const uint32 diff)
        {
            // If Enabled
            if (EnchanterMessageTimer != 0)
            {
                if (MessageTimer <= diff)
                {
                    std::string Message = PickPhrase();
                    me->MonsterSay(Message.c_str(), LANG_UNIVERSAL, NULL);

                    // Use gesture?
                    if (EnchanterEmoteCommand != 0)
                    {
                        me->HandleEmoteCommand(EnchanterEmoteCommand);
                    }

                    // Alert players?
                    if (EnchanterEmoteSpell != 0)
                    {
                        me->CastSpell(me, EnchanterEmoteSpell);
                    }

                    MessageTimer = urand(EnchanterMessageTimer, 300000);
                }
                else { MessageTimer -= diff; }
            }
        }
    };

    // CREATURE AI
    CreatureAI* GetAI(Creature* creature) const
    {
        return new NPC_PassiveAI(creature);
    }
};

void AddNPCEnchanterScripts()
{
    new EnchanterConfig();
    new EnchanterAnnounce();
    new npc_enchantment();
}
