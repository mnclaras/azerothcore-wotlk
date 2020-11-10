/* =============================================================
TO DO:
• Merge human sql template with alliance template
• As Barbz suggested: Rename to character_template the module 
    and all related files (to be less confusing and less generic)
• As Barbz suggested: Scaling system for twink servers
• Talent reseting doesn't work in some circumstances. See handling
    of gossip option #31 for details. Will probably require some minor DB
    restructure or a AC core change.
================================================================ */

#include "TemplateNPC.h"
#include "Chat.h"

// once talents are fully fixed
// this could get removed or moved into a config prop
//static constexpr bool OMIT_TALENT_AND_GLYPH_TEMPLATING = false;

static const std::unordered_set<std::string> knownSpecs = {
    "Discipline", "Holy",        "Shadow",      "Protection", "Retribution", "Fury",         "Arms",         "Arcane",   "Fire",          "Frost",  "Affliction", "Demonology", "Destruction",
    "Elemental",  "Enhancement", "Restoration", "Ballance",   "Feral",       "Marksmanship", "Beastmastery", "Survival", "Assassination", "Combat", "Subtlety",   "Blood",      "Unholy",
	"DisciplinePVE", "HolyPVE",        "ShadowPVE",      "ProtectionPVE", "RetributionPVE", "FuryPVE",         "ArmsPVE",         "ArcanePVE",   "FirePVE",          "FrostPVE",  "AfflictionPVE", "DemonologyPVE", "DestructionPVE",
    "ElementalPVE",  "EnhancementPVE", "RestorationPVE", "BallancePVE",   "FeralPVE",       "MarksmanshipPVE", "BeastmasteryPVE", "SurvivalPVE", "AssassinationPVE", "CombatPVE", "SubtletyPVE",   "BloodPVE",      "UnholyPVE",
};

void sTemplateNPC::ApplyBonus(Player* player, Item* item, EnchantmentSlot slot, uint32 bonusEntry)
{
    if (!item)
        return;

    if (!bonusEntry || bonusEntry == 0)
        return;

    player->ApplyEnchantment(item, slot, false);
    item->SetEnchantment(slot, bonusEntry, 0, 0);
    player->ApplyEnchantment(item, slot, true);
}

void sTemplateNPC::ApplyGlyph(Player* player, uint8 slot, uint32 glyphID)
{
    if (GlyphPropertiesEntry const* gp = sGlyphPropertiesStore.LookupEntry(glyphID))
    {
        if (uint32 oldGlyph = player->GetGlyph(slot))
        {
            player->RemoveAurasDueToSpell(sGlyphPropertiesStore.LookupEntry(oldGlyph)->SpellId);
            player->SetGlyph(slot, 0, true);
        }
        player->CastSpell(player, gp->SpellId, true);
        player->SetGlyph(slot, glyphID, true);
    }
}

void sTemplateNPC::RemoveAllGlyphs(Player* player)
{
    for (uint8 i = 0; i < MAX_GLYPH_SLOT_INDEX; ++i)
    {
        if (uint32 glyph = player->GetGlyph(i))
        {
            if (GlyphPropertiesEntry const* gp = sGlyphPropertiesStore.LookupEntry(glyph))
            {
                if (GlyphSlotEntry const* gs = sGlyphSlotStore.LookupEntry(player->GetGlyphSlot(i)))
                {
                    player->RemoveAurasDueToSpell(sGlyphPropertiesStore.LookupEntry(glyph)->SpellId);
                    player->SetGlyph(i, 0, true);
                    player->SendTalentsInfoData(false); // this is somewhat an in-game glyph realtime update (apply/remove)
                }
            }
        }
    }
}

void sTemplateNPC::Copy(Player* target, Player* src)
{

    target->resetTalents(true);

    // copy talents
    const PlayerTalentMap& talentMap = src->GetTalentMap();
    for (PlayerTalentMap::const_iterator itr = talentMap.begin(); itr != talentMap.end(); ++itr)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo)) continue;

        TalentEntry const* talentInfo = sTalentStore.LookupEntry(itr->second->talentID);
        if (!talentInfo) continue;

        TalentTabEntry const* talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if (!talentTabInfo) continue;

        uint8 currentTalentRank = 0;
        for (uint8 rank = 0; rank < MAX_TALENT_RANK; ++rank)
            if (talentInfo->RankID[rank] && itr->first == talentInfo->RankID[rank])
            {
                currentTalentRank = rank;
                break;
            }

        if (currentTalentRank == MAX_TALENT_RANK) continue;

        target->LearnTalent(talentInfo->TalentID, currentTalentRank, true);
    }
    target->SetFreeTalentPoints(0);
	target->SendTalentsInfoData(false);

    RemoveAllGlyphs(target);

    // copy glyphs
    for (uint8 slot = 0; slot < MAX_GLYPH_SLOT_INDEX; ++slot)
        ApplyGlyph(target, slot, src->GetGlyph(slot));

    target->SendTalentsInfoData(false);

    // copy gear
    // empty slots will be ignored
    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        Item* equippedItem = src->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
        if (!equippedItem)
            continue;

        target->EquipNewItem(i, equippedItem->GetEntry(), true);

        ApplyBonus(target, target->GetItemByPos(INVENTORY_SLOT_BAG_0, i), PERM_ENCHANTMENT_SLOT, equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT));
        ApplyBonus(target, target->GetItemByPos(INVENTORY_SLOT_BAG_0, i), SOCK_ENCHANTMENT_SLOT, equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT));
        ApplyBonus(target, target->GetItemByPos(INVENTORY_SLOT_BAG_0, i), SOCK_ENCHANTMENT_SLOT_2, equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2));
        ApplyBonus(target, target->GetItemByPos(INVENTORY_SLOT_BAG_0, i), SOCK_ENCHANTMENT_SLOT_3, equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3));
        ApplyBonus(target, target->GetItemByPos(INVENTORY_SLOT_BAG_0, i), BONUS_ENCHANTMENT_SLOT, equippedItem->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT));
        ApplyBonus(target, target->GetItemByPos(INVENTORY_SLOT_BAG_0, i), PRISMATIC_ENCHANTMENT_SLOT, equippedItem->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT));
    }
}

void sTemplateNPC::LearnTemplateTalents(Player* player, std::string sTalentsSpec)
{
    for (TalentContainer::const_iterator itr = m_TalentContainer.begin(); itr != m_TalentContainer.end(); ++itr)
    {
        if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
        {
            uint32 spellId = (*itr)->talentId;
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo)) continue;

            for (uint32 talentId = 0; talentId < sTalentStore.GetNumRows(); ++talentId)
            {
                TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentId);
                if (!talentInfo) continue;

                // find talent rank
                int8 talentRank = -1;
                for (int8 rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
                {
                    if (talentInfo->RankID[rank] && talentInfo->RankID[rank] == spellId)
                    {
                        talentRank = rank;
                        break;
                    }
                }
                // talent not found
                if (talentRank < 0 || talentRank == MAX_TALENT_RANK) continue;

                player->LearnTalent(talentInfo->TalentID, talentRank, true);
            }
        }
    }

    player->SetFreeTalentPoints(0);
    player->SendTalentsInfoData(false);
}

void sTemplateNPC::LearnTemplateGlyphs(Player* player, std::string sTalentsSpec)
{
    for (GlyphContainer::const_iterator itr = m_GlyphContainer.begin(); itr != m_GlyphContainer.end(); ++itr)
    {
        if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
            ApplyGlyph(player, (*itr)->slot, (*itr)->glyph);
    }
    player->SendTalentsInfoData(false);
}

void sTemplateNPC::EquipTemplateGear(Player* player, std::string sTalentsSpec)
{
    if (player->getRace() == RACE_HUMAN)
    {
        for (HumanGearContainer::const_iterator itr = m_HumanGearContainer.begin(); itr != m_HumanGearContainer.end(); ++itr)
        {
            if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
            {
                player->EquipNewItem((*itr)->pos, (*itr)->itemEntry, true); // Equip the item and apply enchants and gems
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PERM_ENCHANTMENT_SLOT, (*itr)->enchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT, (*itr)->socket1);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_2, (*itr)->socket2);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_3, (*itr)->socket3);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), BONUS_ENCHANTMENT_SLOT, (*itr)->bonusEnchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PRISMATIC_ENCHANTMENT_SLOT, (*itr)->prismaticEnchant);
            }
        }
    }
    else if (player->GetTeamId() == TEAM_ALLIANCE && player->getRace() != RACE_HUMAN)
    {
        for (AllianceGearContainer::const_iterator itr = m_AllianceGearContainer.begin(); itr != m_AllianceGearContainer.end(); ++itr)
        {
            if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
            {
                player->EquipNewItem((*itr)->pos, (*itr)->itemEntry, true); // Equip the item and apply enchants and gems
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PERM_ENCHANTMENT_SLOT, (*itr)->enchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT, (*itr)->socket1);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_2, (*itr)->socket2);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_3, (*itr)->socket3);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), BONUS_ENCHANTMENT_SLOT, (*itr)->bonusEnchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PRISMATIC_ENCHANTMENT_SLOT, (*itr)->prismaticEnchant);
            }
        }
    }
    else if (player->GetTeamId() == TEAM_HORDE)
    {
        for (HordeGearContainer::const_iterator itr = m_HordeGearContainer.begin(); itr != m_HordeGearContainer.end(); ++itr)
        {
            if ((*itr)->playerClass == GetClassString(player).c_str() && (*itr)->playerSpec == sTalentsSpec)
            {
                player->EquipNewItem((*itr)->pos, (*itr)->itemEntry, true); // Equip the item and apply enchants and gems
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PERM_ENCHANTMENT_SLOT, (*itr)->enchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT, (*itr)->socket1);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_2, (*itr)->socket2);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), SOCK_ENCHANTMENT_SLOT_3, (*itr)->socket3);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), BONUS_ENCHANTMENT_SLOT, (*itr)->bonusEnchant);
                ApplyBonus(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, (*itr)->pos), PRISMATIC_ENCHANTMENT_SLOT, (*itr)->prismaticEnchant);
            }
        }
    }
}

void sTemplateNPC::LoadTalentsContainer()
{
    for (TalentContainer::const_iterator itr = m_TalentContainer.begin(); itr != m_TalentContainer.end(); ++itr)
        delete *itr;

    m_TalentContainer.clear();

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, talentId FROM template_npc_talents;");

    if (!result)
    {
        sLog->outString(">>TEMPLATE NPC: Loaded 0 talent templates. DB table `template_npc_talents` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        TalentTemplate* pTalent = new TalentTemplate;

        pTalent->playerClass = fields[0].GetString();
        pTalent->playerSpec = fields[1].GetString();
        pTalent->talentId = fields[2].GetUInt32();

        m_TalentContainer.push_back(pTalent);
        ++count;
    } while (result->NextRow());
    sLog->outString(">>TEMPLATE NPC: Loaded %u talent templates in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sTemplateNPC::LoadGlyphsContainer()
{
    for (GlyphContainer::const_iterator itr = m_GlyphContainer.begin(); itr != m_GlyphContainer.end(); ++itr)
        delete *itr;

    m_GlyphContainer.clear();

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, slot, glyph FROM template_npc_glyphs;");

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (!result)
    {
        sLog->outString(">>TEMPLATE NPC: Loaded 0 glyph templates. DB table `template_npc_glyphs` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        GlyphTemplate* pGlyph = new GlyphTemplate;

        pGlyph->playerClass = fields[0].GetString();
        pGlyph->playerSpec = fields[1].GetString();
        pGlyph->slot = fields[2].GetUInt8();
        pGlyph->glyph = fields[3].GetUInt32();

        m_GlyphContainer.push_back(pGlyph);
        ++count;
    } while (result->NextRow());


    sLog->outString(">>TEMPLATE NPC: Loaded %u glyph templates in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sTemplateNPC::LoadHumanGearContainer()
{
    for (HumanGearContainer::const_iterator itr = m_HumanGearContainer.begin(); itr != m_HumanGearContainer.end(); ++itr)
        delete *itr;

    m_HumanGearContainer.clear();

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, pos, itemEntry, enchant, socket1, socket2, socket3, bonusEnchant, prismaticEnchant FROM template_npc_human;");

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (!result)
    {
        sLog->outString(">>TEMPLATE NPC: Loaded 0 'gear templates. DB table `template_npc_human` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        HumanGearTemplate* pItem = new HumanGearTemplate;

        pItem->playerClass = fields[0].GetString();
        pItem->playerSpec = fields[1].GetString();
        pItem->pos = fields[2].GetUInt8();
        pItem->itemEntry = fields[3].GetUInt32();
        pItem->enchant = fields[4].GetUInt32();
        pItem->socket1 = fields[5].GetUInt32();
        pItem->socket2 = fields[6].GetUInt32();
        pItem->socket3 = fields[7].GetUInt32();
        pItem->bonusEnchant = fields[8].GetUInt32();
        pItem->prismaticEnchant = fields[9].GetUInt32();

        m_HumanGearContainer.push_back(pItem);
        ++count;
    } while (result->NextRow());
    sLog->outString(">>TEMPLATE NPC: Loaded %u gear templates for Humans in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sTemplateNPC::LoadAllianceGearContainer()
{
    for (AllianceGearContainer::const_iterator itr = m_AllianceGearContainer.begin(); itr != m_AllianceGearContainer.end(); ++itr)
        delete *itr;

    m_AllianceGearContainer.clear();

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, pos, itemEntry, enchant, socket1, socket2, socket3, bonusEnchant, prismaticEnchant FROM template_npc_alliance;");

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (!result)
    {
        sLog->outString(">>TEMPLATE NPC: Loaded 0 'gear templates. DB table `template_npc_alliance` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        AllianceGearTemplate* pItem = new AllianceGearTemplate;

        pItem->playerClass = fields[0].GetString();
        pItem->playerSpec = fields[1].GetString();
        pItem->pos = fields[2].GetUInt8();
        pItem->itemEntry = fields[3].GetUInt32();
        pItem->enchant = fields[4].GetUInt32();
        pItem->socket1 = fields[5].GetUInt32();
        pItem->socket2 = fields[6].GetUInt32();
        pItem->socket3 = fields[7].GetUInt32();
        pItem->bonusEnchant = fields[8].GetUInt32();
        pItem->prismaticEnchant = fields[9].GetUInt32();

        m_AllianceGearContainer.push_back(pItem);
        ++count;
    } while (result->NextRow());
    sLog->outString(">>TEMPLATE NPC: Loaded %u gear templates for Alliances in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void sTemplateNPC::LoadHordeGearContainer()
{
    for (HordeGearContainer::const_iterator itr = m_HordeGearContainer.begin(); itr != m_HordeGearContainer.end(); ++itr)
        delete *itr;

    m_HordeGearContainer.clear();

    QueryResult result = CharacterDatabase.Query("SELECT playerClass, playerSpec, pos, itemEntry, enchant, socket1, socket2, socket3, bonusEnchant, prismaticEnchant FROM template_npc_horde;");

    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    if (!result)
    {
        sLog->outString(">>TEMPLATE NPC: Loaded 0 'gear templates. DB table `template_npc_horde` is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        HordeGearTemplate* pItem = new HordeGearTemplate;

        pItem->playerClass = fields[0].GetString();
        pItem->playerSpec = fields[1].GetString();
        pItem->pos = fields[2].GetUInt8();
        pItem->itemEntry = fields[3].GetUInt32();
        pItem->enchant = fields[4].GetUInt32();
        pItem->socket1 = fields[5].GetUInt32();
        pItem->socket2 = fields[6].GetUInt32();
        pItem->socket3 = fields[7].GetUInt32();
        pItem->bonusEnchant = fields[8].GetUInt32();
        pItem->prismaticEnchant = fields[9].GetUInt32();

        m_HordeGearContainer.push_back(pItem);
        ++count;
    } while (result->NextRow());
    sLog->outString(">>TEMPLATE NPC: Loaded %u gear templates for Hordes in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

std::string sTemplateNPC::GetClassString(Player* player)
{
    switch (player->getClass())
    {
    case CLASS_PRIEST:
        return "Priest";
        break;
    case CLASS_PALADIN:
        return "Paladin";
        break;
    case CLASS_WARRIOR:
        return "Warrior";
        break;
    case CLASS_MAGE:
        return "Mage";
        break;
    case CLASS_WARLOCK:
        return "Warlock";
        break;
    case CLASS_SHAMAN:
        return "Shaman";
        break;
    case CLASS_DRUID:
        return "Druid";
        break;
    case CLASS_HUNTER:
        return "Hunter";
        break;
    case CLASS_ROGUE:
        return "Rogue";
        break;
    case CLASS_DEATH_KNIGHT:
        return "DeathKnight";
        break;
    default:
        break;
    }
    return "Unknown"; // Fix warning, this should never happen
}

void sTemplateNPC::PurgeTemplate(Player* player, std::string& playerSpecStr, TemplateType type)
{
    //auto playerClassStr = GetClassString(player).c_str();

    // clean talents
    CharacterDatabase.PQuery("DELETE FROM template_npc_talents WHERE playerClass = '%s' and playerSpec = '%s'", GetClassString(player).c_str(), playerSpecStr.c_str());

    // clean glyphs
    CharacterDatabase.PQuery("DELETE FROM template_npc_glyphs WHERE playerClass = '%s' and playerSpec = '%s'", GetClassString(player).c_str(), playerSpecStr.c_str());
	
    // clean gear
    switch (type)
    {
    case TEMPLATE_HUMAN:
        CharacterDatabase.PQuery("DELETE FROM template_npc_human WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());
        break;
    case TEMPLATE_ALLIANCE:
        CharacterDatabase.PQuery("DELETE FROM template_npc_alliance WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());
        break;
    case TEMPLATE_HORDE:
        CharacterDatabase.PQuery("DELETE FROM template_npc_horde WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());
        break;
    }
}

void sTemplateNPC::ExtractGearTemplateToDB(Player* player, std::string& playerSpecStr, TemplateType type)
{
    PurgeTemplate(player, playerSpecStr, type);
	// player->GetSession()->SendNotification();

    ExtractTalentTemplateToDB(player, playerSpecStr);
    ExtractGlyphsTemplateToDB(player, playerSpecStr);

    for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
    {
        Item* equippedItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);

        if (equippedItem)
        {
            if (type == TEMPLATE_HUMAN)
            {
                CharacterDatabase.PExecute("INSERT INTO template_npc_human (`playerClass`, `playerSpec`, `pos`, `itemEntry`, `enchant`, `socket1`, `socket2`, `socket3`, `bonusEnchant`, "
                                           "`prismaticEnchant`) VALUES ('%s', '%s', "
                                           "'%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u');",
                                           GetClassString(player).c_str(), playerSpecStr.c_str(), equippedItem->GetSlot(), equippedItem->GetEntry(), equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT),
                    equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3),
                    equippedItem->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT));
            }
            else if (type == TEMPLATE_ALLIANCE)
            {
                CharacterDatabase.PExecute("INSERT INTO template_npc_alliance (`playerClass`, `playerSpec`, `pos`, `itemEntry`, `enchant`, `socket1`, `socket2`, `socket3`, `bonusEnchant`, "
                                           "`prismaticEnchant`) VALUES ('%s', '%s', "
                                           "'%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u');",
                                           GetClassString(player).c_str(), playerSpecStr.c_str(), equippedItem->GetSlot(), equippedItem->GetEntry(), equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT),
                    equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3),
                    equippedItem->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT));
            }
            else if (type == TEMPLATE_HORDE)
            {
                CharacterDatabase.PExecute("INSERT INTO template_npc_horde (`playerClass`, `playerSpec`, `pos`, `itemEntry`, `enchant`, `socket1`, `socket2`, `socket3`, `bonusEnchant`, "
                                           "`prismaticEnchant`) VALUES ('%s', '%s', "
                                           "'%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u');",
                                           GetClassString(player).c_str(), playerSpecStr.c_str(), equippedItem->GetSlot(), equippedItem->GetEntry(), equippedItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT),
                    equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2), equippedItem->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3),
                    equippedItem->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT), equippedItem->GetEnchantmentId(PRISMATIC_ENCHANTMENT_SLOT));
            }
        }
    }

}

void sTemplateNPC::ExtractTalentTemplateToDB(Player* player, std::string& playerSpecStr)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT spell FROM character_talent WHERE guid = '%u' "
                                                  "AND specMask = '%u';",
                                                  player->GetGUID(), player->GetActiveSpecMask());

    if (!result)
    {
        return;
    }
    else if (player->GetFreeTalentPoints() > 0)
    {
        player->GetSession()->SendAreaTriggerMessage("You have unspend talent points. Please spend all your talent points and re-extract the template.");
        return;
    }
    else
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 spell = fields[0].GetUInt32();

            CharacterDatabase.PExecute("INSERT INTO template_npc_talents (playerClass, playerSpec, talentId) "
                                       "VALUES ('%s', '%s', '%u');",
                                       GetClassString(player).c_str(), playerSpecStr.c_str(), spell);
        } while (result->NextRow());
    }
}

void sTemplateNPC::ExtractGlyphsTemplateToDB(Player* player, std::string& playerSpecStr)
{
    QueryResult result = CharacterDatabase.PQuery("SELECT glyph1, glyph2, glyph3, glyph4, glyph5, glyph6 "
                                                  "FROM character_glyphs WHERE guid = '%u' AND talentGroup = '%u';",
                                                  player->GetGUID(), player->GetActiveSpec());

    for (uint8 slot = 0; slot < MAX_GLYPH_SLOT_INDEX; ++slot)
    {
        if (!result)
        {
            player->GetSession()->SendAreaTriggerMessage("Get glyphs and re-extract the template!");
            continue;
        }

        Field* fields = result->Fetch();
        uint32 glyph1 = fields[0].GetUInt32();
        uint32 glyph2 = fields[1].GetUInt32();
        uint32 glyph3 = fields[2].GetUInt32();
        uint32 glyph4 = fields[3].GetUInt32();
        uint32 glyph5 = fields[4].GetUInt32();
        uint32 glyph6 = fields[5].GetUInt32();

        uint32 storedGlyph;

        switch (slot)
        {
        case 0:
            storedGlyph = glyph1;
            break;
        case 1:
            storedGlyph = glyph2;
            break;
        case 2:
            storedGlyph = glyph3;
            break;
        case 3:
            storedGlyph = glyph4;
            break;
        case 4:
            storedGlyph = glyph5;
            break;
        case 5:
            storedGlyph = glyph6;
            break;
        default:
            break;
        }

        CharacterDatabase.PExecute("INSERT INTO template_npc_glyphs (playerClass, playerSpec, slot, glyph) "
                                   "VALUES ('%s', '%s', '%u', '%u');",
                                   GetClassString(player).c_str(), playerSpecStr.c_str(), slot, storedGlyph);
    }
}

bool sTemplateNPC::CanEquipTemplate(Player* player, std::string& playerSpecStr)
{
    if (player->getRace() == RACE_HUMAN)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT playerClass, playerSpec FROM template_npc_human "
                                                      "WHERE playerClass = '%s' AND playerSpec = '%s';",
                                                      GetClassString(player).c_str(), playerSpecStr.c_str());

        if (!result)
            return false;
    }
    else if (player->GetTeamId() == TEAM_ALLIANCE && player->getRace() != RACE_HUMAN)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT playerClass, playerSpec FROM template_npc_alliance "
                                                      "WHERE playerClass = '%s' AND playerSpec = '%s';",
                                                      GetClassString(player).c_str(), playerSpecStr.c_str());

        if (!result)
            return false;
    }
    else if (player->GetTeamId() == TEAM_HORDE)
    {
        QueryResult result = CharacterDatabase.PQuery("SELECT playerClass, playerSpec FROM template_npc_horde "
            "WHERE playerClass = '%s' AND playerSpec = '%s';", GetClassString(player).c_str(), playerSpecStr.c_str());

        if (!result)
            return false;
    }
    return true;
}

class TemplateNPC : public CreatureScript
{
public:
    TemplateNPC() : CreatureScript("TemplateNPC") { }

    static bool IsSpanishPlayer(Player* player)
    {
        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        return (locale == LOCALE_esES || locale == LOCALE_esMX);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        bool isSpanish = IsSpanishPlayer(player);

        std::string priest_Discipline = isSpanish ? "Disciplina" : "Discipline";
        std::string priest_Shadow = isSpanish ? "Sombras" : "Shadow";
        std::string priest_Holy = isSpanish ? "Sagrado" : "Holy";
        std::string paladin_Holy = isSpanish ? "Sagrado" : "Holy";
        std::string paladin_Protection = isSpanish ? "Proteccion" : "Protection";
        std::string paladin_Retribution = isSpanish ? "Reprension" : "Retribution";
        std::string warrior_Arms = isSpanish ? "Armas" : "Arms";
        std::string warrior_Protection = isSpanish ? "Proteccion" : "Protection";
        std::string warrior_Fury = isSpanish ? "Furia" : "Fury";
        std::string mage_Arcane = isSpanish ? "Arcano" : "Arcane";
        std::string mage_Fire = isSpanish ? "Fuego" : "Fire";
        std::string mage_Frost = isSpanish ? "Escarcha" : "Frost";
        std::string warlock_Affliction = isSpanish ? "Afliccion" : "Affliction";
        std::string warlock_Destruction = isSpanish ? "Destruccion" : "Destruction";
        std::string warlock_Demonology = isSpanish ? "Demonologia" : "Demonology";
        std::string shaman_Elemental = isSpanish ? "Elemental" : "Elemental";
        std::string shaman_Enhancement = isSpanish ? "Mejora" : "Enhancement";
        std::string shaman_Restoration = isSpanish ? "Restauracion" : "Restoration";
        std::string druid_Balance = isSpanish ? "Equilibrio" : "Balance";
        std::string druid_Feral = isSpanish ? "Feral" : "Feral";
        std::string druid_Restoration = isSpanish ? "Restauracion" : "Restoration";
        std::string hunter_Marksmanship = isSpanish ? "Punteria" : "Marksmanship";
        std::string hunter_Beastmastery = isSpanish ? "Bestias" : "Beastmastery";
        std::string dk_Frost = isSpanish ? "Escarcha" : "Frost";
        std::string dk_Unholy = isSpanish ? "Profano" : "Unholy";
        std::string dk_Blood = isSpanish ? "Sangre" : "Blood";
        std::string rogue_Assasination = isSpanish ? "Asesinato" : "Assasination";
        std::string rogue_Combat = isSpanish ? "Combate" : "Combat";
        std::string rogue_Subtlety = isSpanish ? "Sutileza" : "Subtlety";

        switch (player->getClass())
        {
        case CLASS_PRIEST:
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_wordfortitude:20|t|r " + priest_Discipline, GOSSIP_SENDER_MAIN, 0);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_shadowwordpain:20|t|r " + priest_Shadow, GOSSIP_SENDER_MAIN, 2);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_wordfortitude:20|t|r " + isSpanish ? "Disciplina" : "Discipline", GOSSIP_SENDER_MAIN, 100);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_shadowwordpain:20|t|r " + isSpanish ? "Sombras" : " Shadow", GOSSIP_SENDER_MAIN, 102);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_wordfortitude:20|t|r " + priest_Discipline, GOSSIP_SENDER_MAIN, 200);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_holybolt:20|t|r " + priest_Holy, GOSSIP_SENDER_MAIN, 201);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_shadowwordpain:20|t|r " + priest_Shadow, GOSSIP_SENDER_MAIN, 202);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_wordfortitude:20|t|r " + isSpanish ? "Disciplina" : "Discipline", GOSSIP_SENDER_MAIN, 300);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_holybolt:20|t|r " + isSpanish ? "Sagrado" : "Holy", GOSSIP_SENDER_MAIN, 301);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_shadowwordpain:20|t|r " + isSpanish ? "Sombras" : "Shadow", GOSSIP_SENDER_MAIN, 302);
            break;
        case CLASS_PALADIN:
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_holybolt:20|t|r " + paladin_Holy, GOSSIP_SENDER_MAIN, 3);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_devotionaura:20|t|r " + paladin_Protection, GOSSIP_SENDER_MAIN, 4);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_auraoflight:20|t|r " + paladin_Retribution, GOSSIP_SENDER_MAIN, 5);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_holybolt:20|t|r " + isSpanish ? "Sagrado" : "Holy", GOSSIP_SENDER_MAIN, 103);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_devotionaura:20|t|r " + isSpanish ? "Proteccion" : "Protection", GOSSIP_SENDER_MAIN, 104);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_auraoflight:20|t|r " + isSpanish ? "Reprension" : "Retribution", GOSSIP_SENDER_MAIN, 105);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_holybolt:20|t|r " + paladin_Holy, GOSSIP_SENDER_MAIN, 203);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_devotionaura:20|t|r " + paladin_Protection, GOSSIP_SENDER_MAIN, 204);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_auraoflight:20|t|r " + paladin_Retribution, GOSSIP_SENDER_MAIN, 205);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_holybolt:20|t|r " + isSpanish ? "Sagrado" : "Holy", GOSSIP_SENDER_MAIN, 303);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_devotionaura:20|t|r " + isSpanish ? "Proteccion" : "Protection", GOSSIP_SENDER_MAIN, 304);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_auraoflight:20|t|r " + isSpanish ? "Reprension" : "Retribution", GOSSIP_SENDER_MAIN, 305);
            break;
        case CLASS_WARRIOR:           
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_rogue_eviscerate:20|t|r " + warrior_Arms, GOSSIP_SENDER_MAIN, 7);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_warrior_defensivestance:20|t|r " + warrior_Protection, GOSSIP_SENDER_MAIN, 8);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_rogue_eviscerate:20|t|r " + isSpanish ? "Armas" : "Arms", GOSSIP_SENDER_MAIN, 107);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_warrior_defensivestance:20|t|r " + isSpanish ? "Proteccion" : "Protection", GOSSIP_SENDER_MAIN, 108);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_warrior_innerrage:20|t|r " + warrior_Fury, GOSSIP_SENDER_MAIN, 206);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_warrior_defensivestance:20|t|r " + warrior_Protection, GOSSIP_SENDER_MAIN, 208);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_warrior_innerrage:20|t|r " + isSpanish ? "Furia" : "Fury", GOSSIP_SENDER_MAIN, 306);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_warrior_defensivestance:20|t|r " + isSpanish ? "Proteccion" : "Protection", GOSSIP_SENDER_MAIN, 308);
            break;
        case CLASS_MAGE:
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_magicalsentry:20|t|r " + mage_Arcane, GOSSIP_SENDER_MAIN, 9);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_fire_flamebolt:20|t|r " + mage_Fire, GOSSIP_SENDER_MAIN, 10);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_frost_frostbolt02:20|t|r " + mage_Frost, GOSSIP_SENDER_MAIN, 11);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_magicalsentry:20|t|r " + isSpanish ? "Arcano" : "Arcane", GOSSIP_SENDER_MAIN, 109);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_fire_flamebolt:20|t|r " + isSpanish ? "Fuego" : "Fire", GOSSIP_SENDER_MAIN, 110);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_frost_frostbolt02:20|t|r " + isSpanish ? "Escarcha" : "Frost", GOSSIP_SENDER_MAIN, 111);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_magicalsentry:20|t|r " + mage_Arcane, GOSSIP_SENDER_MAIN, 209);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_fire_flamebolt:20|t|r " + mage_Fire, GOSSIP_SENDER_MAIN, 210);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_holy_magicalsentry:20|t|r " + isSpanish ? "Arcano" : "Arcane", GOSSIP_SENDER_MAIN, 309);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_fire_flamebolt:20|t|r " + isSpanish ? "Fuego" : "Fire", GOSSIP_SENDER_MAIN, 310);
            break;
        case CLASS_WARLOCK:
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_deathcoil:20|t|r " + warlock_Affliction, GOSSIP_SENDER_MAIN, 12);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_rainoffire:20|t|r " + warlock_Destruction, GOSSIP_SENDER_MAIN, 14);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_deathcoil:20|t|r " + isSpanish ? "Afliccion" : "Affliction", GOSSIP_SENDER_MAIN, 112);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_rainoffire:20|t|r " + isSpanish ? "Destruccion" : "Destruction", GOSSIP_SENDER_MAIN, 114);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_deathcoil:20|t|r " + warlock_Affliction, GOSSIP_SENDER_MAIN, 212);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_metamorphosis:20|t|r " + warlock_Demonology, GOSSIP_SENDER_MAIN, 213);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_deathcoil:20|t|r " + isSpanish ? "Afliccion" : "Affliction", GOSSIP_SENDER_MAIN, 312);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_shadow_metamorphosis:20|t|r " + isSpanish ? "Demonologia" : "Demonology", GOSSIP_SENDER_MAIN, 313);
            break;
        case CLASS_SHAMAN:
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightning:20|t|r " + shaman_Elemental, GOSSIP_SENDER_MAIN, 15);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightningshield:20|t|r " + shaman_Enhancement, GOSSIP_SENDER_MAIN, 16);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_magicimmunity:20|t|r " + shaman_Restoration, GOSSIP_SENDER_MAIN, 17);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightning:20|t|r " + isSpanish ? "Elemental" : "Elemental", GOSSIP_SENDER_MAIN, 115);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightningshield:20|t|r " + isSpanish ? "Mejora" : "Enhancement", GOSSIP_SENDER_MAIN, 116);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_magicimmunity:20|t|r " + isSpanish ? "Restauracion" : "Restoration", GOSSIP_SENDER_MAIN, 117);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightning:20|t|r " + shaman_Elemental, GOSSIP_SENDER_MAIN, 215);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightningshield:20|t|r " + shaman_Enhancement, GOSSIP_SENDER_MAIN, 216);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_magicimmunity:20|t|r " + shaman_Restoration, GOSSIP_SENDER_MAIN, 217);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightning:20|t|r " + isSpanish ? "Elemental" : "Elemental", GOSSIP_SENDER_MAIN, 315);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_lightningshield:20|t|r " + isSpanish ? "Mejora" : "Enhancement", GOSSIP_SENDER_MAIN, 316);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_magicimmunity:20|t|r " + isSpanish ? "Restauracion" : "Restoration", GOSSIP_SENDER_MAIN, 317);
            break;
        case CLASS_DRUID:
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_starfall:20|t|r " + druid_Balance, GOSSIP_SENDER_MAIN, 18);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_racial_bearform:20|t|r " + druid_Feral, GOSSIP_SENDER_MAIN, 19);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_healingtouch:20|t|r " + druid_Restoration, GOSSIP_SENDER_MAIN, 20);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_starfall:20|t|r " + isSpanish ? "Equilibrio" : "Balance", GOSSIP_SENDER_MAIN, 118);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_racial_bearform:20|t|r " + isSpanish ? "Feral" : "Feral", GOSSIP_SENDER_MAIN, 119);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_healingtouch:20|t|r " + isSpanish ? "Restauracion" : "Restoration", GOSSIP_SENDER_MAIN, 120);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_starfall:20|t|r " + druid_Balance, GOSSIP_SENDER_MAIN, 218);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_racial_bearform:20|t|r " + druid_Feral, GOSSIP_SENDER_MAIN, 219);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_healingtouch:20|t|r " + druid_Restoration, GOSSIP_SENDER_MAIN, 220);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_starfall:20|t|r " + isSpanish ? "Equilibrio" : "Balance", GOSSIP_SENDER_MAIN, 318);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_racial_bearform:20|t|r " + isSpanish ? "Feral" : "Feral", GOSSIP_SENDER_MAIN, 319);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_nature_healingtouch:20|t|r " + isSpanish ? "Restauracion" : "Restoration", GOSSIP_SENDER_MAIN, 320);
            break;
        case CLASS_HUNTER:
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_marksmanship:20|t|r " + hunter_Marksmanship, GOSSIP_SENDER_MAIN, 21);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_hunter_beasttaming:20|t|r " + hunter_Beastmastery, GOSSIP_SENDER_MAIN, 22);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_marksmanship:20|t|r " + isSpanish ? "Punteria" : "Marksmanship", GOSSIP_SENDER_MAIN, 121);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_hunter_beasttaming:20|t|r " + isSpanish ? "Bestias" : "Beastmastery", GOSSIP_SENDER_MAIN, 122);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_marksmanship:20|t|r " + hunter_Marksmanship, GOSSIP_SENDER_MAIN, 221);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_marksmanship:20|t|r " + isSpanish ? "Punteria" : "Marksmanship", GOSSIP_SENDER_MAIN, 321);
            break;
        case CLASS_ROGUE:
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_rogue_eviscerate:20|t|r " + rogue_Assasination, GOSSIP_SENDER_MAIN, 24);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_backstab:20|t|r " + rogue_Combat, GOSSIP_SENDER_MAIN, 25);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_stealth:20|t|r " + rogue_Subtlety, GOSSIP_SENDER_MAIN, 26);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_rogue_eviscerate:20|t|r " + isSpanish ? "" : "Assasination", GOSSIP_SENDER_MAIN, 124);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_backstab:20|t|r " + isSpanish ? "" : "Combat", GOSSIP_SENDER_MAIN, 125);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_stealth:20|t|r " + isSpanish ? "" : "Subtlety", GOSSIP_SENDER_MAIN, 126);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_rogue_eviscerate:20|t|r " + rogue_Assasination, GOSSIP_SENDER_MAIN, 224);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_backstab:20|t|r " + rogue_Combat, GOSSIP_SENDER_MAIN, 225);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_rogue_eviscerate:20|t|r " + isSpanish ? "" : "Assasination", GOSSIP_SENDER_MAIN, 324);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\ability_backstab:20|t|r " + isSpanish ? "" : "Combat", GOSSIP_SENDER_MAIN, 325);
            break;
        case CLASS_DEATH_KNIGHT:
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP EQUIPO Y TALENTOS ----" : "---- PVP GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_frostpresence:20|t|r " + dk_Frost, GOSSIP_SENDER_MAIN, 28);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_unholypresence:20|t|r " + dk_Unholy, GOSSIP_SENDER_MAIN, 29);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVP SOLO TALENTOS ----" : "---- PVP ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_frostpresence:20|t|r " + isSpanish ? "" : "Frost", GOSSIP_SENDER_MAIN, 128);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_unholypresence:20|t|r " + isSpanish ? "" : "Unholy", GOSSIP_SENDER_MAIN, 129);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE EQUIPO Y TALENTOS ----" : "---- PVE GEAR AND TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_frostpresence:20|t|r " + dk_Frost, GOSSIP_SENDER_MAIN, 228);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_unholypresence:20|t|r " + dk_Unholy, GOSSIP_SENDER_MAIN, 229);
			AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_bloodpresence:20|t|r " + dk_Blood, GOSSIP_SENDER_MAIN, 227);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, isSpanish ? "---- PVE SOLO TALENTOS ----" : "---- PVE ONLY TALENTS ----", GOSSIP_SENDER_MAIN, 5000);
			//AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_bloodpresence:20|t|r " + isSpanish ? "" : "Blood", GOSSIP_SENDER_MAIN, 327);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_frostpresence:20|t|r " + isSpanish ? "" : "Frost", GOSSIP_SENDER_MAIN, 328);
            //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "|cff00ff00|TInterface\\icons\\spell_deathknight_unholypresence:20|t|r " + isSpanish ? "" : "Unholy", GOSSIP_SENDER_MAIN, 329);
            break;
        }

		player->SEND_GOSSIP_MENU(55009, creature->GetGUID());
        return true;
    }

    static void EquipFullTemplateGear(Player* player, std::string playerSpecStr) // Merge
    {
        if (TemplateExistsCheck(player, playerSpecStr) && CheckPlayerIsNaked(player) && CheckSpendTalents(player) && ApplyTalentsAndGlyphs(player, playerSpecStr) && ApplyGear(player, playerSpecStr))
        {
            bool isSpanish = IsSpanishPlayer(player);
            player->GetSession()->SendAreaTriggerMessage(isSpanish ? "Especializacion %s aplicada satisfactoriamente!" : "Successfuly applied %s spec template!", playerSpecStr.c_str());
        }
    }          

	static void LearnOnlyTalentsAndGlyphs(Player* player, std::string playerSpecStr) // Merge
    {
        if (TemplateExistsCheck(player, playerSpecStr) && CheckSpendTalents(player) && ApplyTalentsAndGlyphs(player, playerSpecStr))
        {
            bool isSpanish = IsSpanishPlayer(player);
            player->GetSession()->SendAreaTriggerMessage(isSpanish ? "Especializacion de talentos %s aprendida satisfactoriamente!" : "Successfuly learned talent spec %s!", playerSpecStr.c_str());
        }
    }

	static bool ApplyTalentsAndGlyphs(Player* player, std::string playerSpecStr)
	{
		LearnWeaponSkills(player);
		LearnPlateMailSpells(player);
				
		// Cast spells that teach dual spec
		// Both are also ImplicitTarget self and must be cast by player 
		if (!player->HasSpell(SPELL_Teach_Learn_Talent_Specialization_Switches))
			player->CastSpell(player, SPELL_Teach_Learn_Talent_Specialization_Switches, player->GetGUID());
		if (!player->HasSpell(SPELL_Learn_a_Second_Talent_Specialization))
			player->CastSpell(player, SPELL_Learn_a_Second_Talent_Specialization, player->GetGUID());
			 
		// Learn Riding/Flying
		if (!player->HasSpell(SPELL_Artisan_Riding))	 
			player->learnSpell(SPELL_Artisan_Riding);
		if (!player->HasSpell(SPELL_Cold_Weather_Flying)) 
			player->learnSpell(SPELL_Cold_Weather_Flying);

		sTemplateNpcMgr->LearnTemplateTalents(player, playerSpecStr);
		sTemplateNpcMgr->LearnTemplateGlyphs(player, playerSpecStr);

		if (player->getPowerType() == POWER_MANA)
			player->SetPower(POWER_MANA, player->GetMaxPower(POWER_MANA));

		player->SetHealth(player->GetMaxHealth());

		return true;
	}

    static bool ApplyGear(Player* player, std::string playerSpecStr)
    {
        sTemplateNpcMgr->EquipTemplateGear(player, playerSpecStr);
        return true;
    }

	static bool CheckPlayerIsNaked(Player* player)
    {
        // Don't let players to use Template feature while wearing some gear
        for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
        {
            if (Item* haveItemEquipped = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            {
                if (haveItemEquipped)
                {
                    bool isSpanish = IsSpanishPlayer(player);
                    player->GetSession()->SendAreaTriggerMessage(isSpanish ? "Necesitas desequipar todos los objetos de tu personaje!" : "You need to remove all your equipped items in order to use this feature!");
                    return false;
                }
            }
        }
        return true;
    }

    static bool TemplateExistsCheck(Player* player, std::string playerSpecStr)
    {
        if (sTemplateNpcMgr->CanEquipTemplate(player, playerSpecStr) == false)
        {
            bool isSpanish = IsSpanishPlayer(player);
            player->GetSession()->SendAreaTriggerMessage(isSpanish ? "El equipamiento para la especializacion %s no existe todavia." : "There's no templates for %s specialization yet.", playerSpecStr.c_str());
            return false;
        }
        return true;
    }

	static bool CheckSpendTalents(Player* player)
    {
        // Don't let players to use Template feature after spending some talent points
        if (player->GetFreeTalentPoints() < 71)
        {
            bool isSpanish = IsSpanishPlayer(player);
            player->GetSession()->SendAreaTriggerMessage(isSpanish ? "Tienes que resetear los talentos de tu personaje!" : "You have already spent some talent points. You need to reset your talents first!");
            return false;
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
        {
            player->PlayerTalkClass->ClearMenus();

            if (!player || !creature)
                return false;

            bool isSpanish = IsSpanishPlayer(player);

            switch (uiAction)
            {
            case 0: // Use Discipline Priest Spec
            EquipFullTemplateGear(player, "Discipline");
                break;

            case 1: // Use Holy Priest Spec
            EquipFullTemplateGear(player, "Holy");
                break;

            case 2: // Use Shadow Priest Spec
            EquipFullTemplateGear(player, "Shadow");
                break;

            case 3: // Use Holy Paladin Spec
            EquipFullTemplateGear(player, "Holy");
                break;

            case 4: // Use Protection Paladin Spec
            EquipFullTemplateGear(player, "Protection");
                break;

            case 5: // Use Retribution Paladin Spec
            EquipFullTemplateGear(player, "Retribution");
                break;

            case 6: // Use Fury Warrior Spec
            EquipFullTemplateGear(player, "Fury");
                break;

            case 7: // Use Arms Warrior Spec
            EquipFullTemplateGear(player, "Arms");
                break;

            case 8: // Use Protection Warrior Spec
            EquipFullTemplateGear(player, "Protection");
                break;

            case 9: // Use Arcane Mage Spec
            EquipFullTemplateGear(player, "Arcane");
                break;

            case 10: // Use Fire Mage Spec
            EquipFullTemplateGear(player, "Fire");
                break;

            case 11: // Use Frost Mage Spec
            EquipFullTemplateGear(player, "Frost");
                break;

            case 12: // Use Affliction Warlock Spec
            EquipFullTemplateGear(player, "Affliction");
                break;

            case 13: // Use Demonology Warlock Spec
            EquipFullTemplateGear(player, "Demonology");
                break;

            case 14: // Use Destruction Warlock Spec
            EquipFullTemplateGear(player, "Destruction");
                break;

            case 15: // Use Elemental Shaman Spec
            EquipFullTemplateGear(player, "Elemental");
                break;

            case 16: // Use Enhancement Shaman Spec
            EquipFullTemplateGear(player, "Enhancement");
                break;

            case 17: // Use Restoration Shaman Spec
            EquipFullTemplateGear(player, "Restoration");
                break;

            case 18: // Use Ballance Druid Spec
            EquipFullTemplateGear(player, "Ballance");
                break;

            case 19: // Use Feral Druid Spec
            EquipFullTemplateGear(player, "Feral");
                break;

            case 20: // Use Restoration Druid Spec
            EquipFullTemplateGear(player, "Restoration");
                break;

            case 21: // Use Marksmanship Hunter Spec
            EquipFullTemplateGear(player, "Marksmanship");
                break;

            case 22: // Use Beastmastery Hunter Spec
            EquipFullTemplateGear(player, "Beastmastery");
                break;

            case 23: // Use Survival Hunter Spec
            EquipFullTemplateGear(player, "Survival");
                break;

            case 24: // Use Assassination Rogue Spec
            EquipFullTemplateGear(player, "Assassination");
                break;

            case 25: // Use Combat Rogue Spec
            EquipFullTemplateGear(player, "Combat");
                break;

            case 26: // Use Subtlety Rogue Spec
            EquipFullTemplateGear(player, "Subtlety");
                break;

            case 27: // Use Blood DK Spec
            EquipFullTemplateGear(player, "Blood");
                break;

            case 28: // Use Frost DK Spec
            EquipFullTemplateGear(player, "Frost");
                break;

            case 29: // Use Unholy DK Spec
            EquipFullTemplateGear(player, "Unholy");
                break;

            case 30:
                sTemplateNpcMgr->RemoveAllGlyphs(player);
                player->GetSession()->SendAreaTriggerMessage(isSpanish ? "Tus glifos han sido eliminados." : "Your glyphs have been removed.");
                break;

            case 31:
            // there is a issue with how talent teaching
            // and talent reset works
            // (addTalent() doesn't bump m_usedTalentCount)
            // m_usedTalentCount will however get set correctly
            // on relog.
            if (player->resetTalents(true))
            {
                player->SendTalentsInfoData(false);
                player->GetSession()->SendAreaTriggerMessage(isSpanish ? "Tus talentos han sido reiniciados" : "Your talents have been reset.");
            }
            else
            {
                player->GetSession()->SendAreaTriggerMessage(isSpanish ? "Tus talentos no pudieron ser reiniciados. Prueba a relogear." : "Your talent's couldn't be reset. Try to re-log.");
            }
                break;
            case 32:
                for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
                {
                    if (Item* haveItemEquipped = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    {
                        if (haveItemEquipped)
                        {
                            player->DestroyItemCount(haveItemEquipped->GetEntry(), 1, true, true);

                            if (haveItemEquipped->IsInWorld())
                            {
                                haveItemEquipped->RemoveFromWorld();
                                haveItemEquipped->DestroyForPlayer(player);
                            }

                            haveItemEquipped->SetUInt64Value(ITEM_FIELD_CONTAINED, 0);
                            haveItemEquipped->SetSlot(NULL_SLOT);
                            haveItemEquipped->SetState(ITEM_REMOVED, player);
                        }
                    }
                }
                player->GetSession()->SendAreaTriggerMessage(isSpanish ? "Tu equipamiento ha sido destruido." : "Your equipped gear has been destroyed.");
                break;

                //Priest
            case 100:
            LearnOnlyTalentsAndGlyphs(player, "Discipline");
                break;

            case 101:
            LearnOnlyTalentsAndGlyphs(player, "Holy");
                break;

            case 102:
            LearnOnlyTalentsAndGlyphs(player, "Shadow");
                break;

                //Paladin
            case 103:
            LearnOnlyTalentsAndGlyphs(player, "Holy");
                break;

            case 104:
            LearnOnlyTalentsAndGlyphs(player, "Protection");
                break;

            case 105:
            LearnOnlyTalentsAndGlyphs(player, "Retribution");
                break;

                //Warrior
            case 106:
            LearnOnlyTalentsAndGlyphs(player, "Fury");
                break;

            case 107:
            LearnOnlyTalentsAndGlyphs(player, "Arms");
                break;

            case 108:
            LearnOnlyTalentsAndGlyphs(player, "Protection");
                break;

                //Mage
            case 109:
            LearnOnlyTalentsAndGlyphs(player, "Arcane");
                break;

            case 110:
            LearnOnlyTalentsAndGlyphs(player, "Fire");
                break;

            case 111:
            LearnOnlyTalentsAndGlyphs(player, "Frost");
                break;

                //Warlock
            case 112:
            LearnOnlyTalentsAndGlyphs(player, "Affliction");
                break;

            case 113:
            LearnOnlyTalentsAndGlyphs(player, "Demonology");
                break;

            case 114:
            LearnOnlyTalentsAndGlyphs(player, "Destruction");
                break;

                //Shaman
            case 115:
            LearnOnlyTalentsAndGlyphs(player, "Elemental");
                break;

            case 116:
            LearnOnlyTalentsAndGlyphs(player, "Enhancement");
                break;

            case 117:
            LearnOnlyTalentsAndGlyphs(player, "Restoration");
                break;

                //Druid
            case 118:
            LearnOnlyTalentsAndGlyphs(player, "Ballance");
                break;

            case 119:
            LearnOnlyTalentsAndGlyphs(player, "Feral");
                break;

            case 120:
            LearnOnlyTalentsAndGlyphs(player, "Restoration");
                break;

                //Hunter
            case 121:
            LearnOnlyTalentsAndGlyphs(player, "Marksmanship");
                break;

            case 122:
            LearnOnlyTalentsAndGlyphs(player, "Beastmastery");
                break;

            case 123:
            LearnOnlyTalentsAndGlyphs(player, "Survival");
                break;

                //Rogue
            case 124:
            LearnOnlyTalentsAndGlyphs(player, "Assasination");
                break;

            case 125:
            LearnOnlyTalentsAndGlyphs(player, "Combat");
                break;

            case 126:
            LearnOnlyTalentsAndGlyphs(player, "Subtlety");
                break;

                //DK
            case 127:
            LearnOnlyTalentsAndGlyphs(player, "Blood");
                break;

            case 128:
            LearnOnlyTalentsAndGlyphs(player, "Frost");
                break;

            case 129:
                LearnOnlyTalentsAndGlyphs(player, "Unholy");
                break;

			case 200: // Use Discipline Priest Spec
				EquipFullTemplateGear(player, "DisciplinePVE");
                break;

            case 201: // Use Holy Priest Spec
                EquipFullTemplateGear(player, "HolyPVE");
                break;

            case 202: // Use Shadow Priest Spec
                EquipFullTemplateGear(player, "ShadowPVE");
                break;

            case 203: // Use Holy Paladin Spec
                EquipFullTemplateGear(player, "HolyPVE");
                break;

            case 204: // Use Protection Paladin Spec
                EquipFullTemplateGear(player, "ProtectionPVE");
                break;

            case 205: // Use Retribution Paladin Spec
                EquipFullTemplateGear(player, "RetributionPVE");
                break;

            case 206: // Use Fury Warrior Spec
                EquipFullTemplateGear(player, "FuryPVE");
                break;

            case 207: // Use Arms Warrior Spec
                EquipFullTemplateGear(player, "ArmsPVE");
                break;

            case 208: // Use Protection Warrior Spec
                EquipFullTemplateGear(player, "ProtectionPVE");
                break;

            case 209: // Use Arcane Mage Spec
                EquipFullTemplateGear(player, "ArcanePVE");
                break;

            case 210: // Use Fire Mage Spec
                EquipFullTemplateGear(player, "FirePVE");
                break;

            case 211: // Use Frost Mage Spec
                EquipFullTemplateGear(player, "FrostPVE");
                break;

            case 212: // Use Affliction Warlock Spec
                EquipFullTemplateGear(player, "AfflictionPVE");
                break;

            case 213: // Use Demonology Warlock Spec
                EquipFullTemplateGear(player, "DemonologyPVE");
                break;

            case 214: // Use Destruction Warlock Spec
                EquipFullTemplateGear(player, "DestructionPVE");
                break;

            case 215: // Use Elemental Shaman Spec
                EquipFullTemplateGear(player, "ElementalPVE");
                break;

            case 216: // Use Enhancement Shaman Spec
                EquipFullTemplateGear(player, "EnhancementPVE");
                break;

            case 217: // Use Restoration Shaman Spec
                EquipFullTemplateGear(player, "RestorationPVE");
                break;

            case 218: // Use Ballance Druid Spec
                EquipFullTemplateGear(player, "BallancePVE");
                break;

            case 219: // Use Feral Druid Spec
                EquipFullTemplateGear(player, "FeralPVE");
                break;

            case 220: // Use Restoration Druid Spec
                EquipFullTemplateGear(player, "RestorationPVE");
                break;

            case 221: // Use Marksmanship Hunter Spec
                EquipFullTemplateGear(player, "MarksmanshipPVE");
                break;

            case 222: // Use Beastmastery Hunter Spec
                EquipFullTemplateGear(player, "BeastmasteryPVE");
                break;

            case 223: // Use Survival Hunter Spec
                EquipFullTemplateGear(player, "SurvivalPVE");
                break;

            case 224: // Use Assassination Rogue Spec
                EquipFullTemplateGear(player, "AssassinationPVE");
                break;

            case 225: // Use Combat Rogue Spec
                EquipFullTemplateGear(player, "CombatPVE");
                break;

            case 226: // Use Subtlety Rogue Spec
                EquipFullTemplateGear(player, "SubtletyPVE");
                break;

            case 227: // Use Blood DK Spec
                EquipFullTemplateGear(player, "BloodPVE");
                break;

            case 228: // Use Frost DK Spec
                EquipFullTemplateGear(player, "FrostPVE");
                break;

            case 229: // Use Unholy DK Spec
                EquipFullTemplateGear(player, "UnholyPVE");
                break;

			                //Priest
            case 300:
                LearnOnlyTalentsAndGlyphs(player, "DisciplinePVE");
                break;

            case 301:
                LearnOnlyTalentsAndGlyphs(player, "HolyPVE");
                break;

            case 302:
                LearnOnlyTalentsAndGlyphs(player, "ShadowPVE");
                break;

                //Paladin
            case 303:
                LearnOnlyTalentsAndGlyphs(player, "HolyPVE");
                break;

            case 304:
                LearnOnlyTalentsAndGlyphs(player, "ProtectionPVE");
                break;

            case 305:
                LearnOnlyTalentsAndGlyphs(player, "RetributionPVE");
                break;

                //Warrior
            case 306:
                LearnOnlyTalentsAndGlyphs(player, "FuryPVE");
                break;

            case 307:
                LearnOnlyTalentsAndGlyphs(player, "ArmsPVE");
                break;

            case 308:
                LearnOnlyTalentsAndGlyphs(player, "ProtectionPVE");
                break;

                //Mage
            case 309:
                LearnOnlyTalentsAndGlyphs(player, "ArcanePVE");
                break;

            case 310:
                LearnOnlyTalentsAndGlyphs(player, "FirePVE");
                break;

            case 311:
                LearnOnlyTalentsAndGlyphs(player, "FrostPVE");
                break;

                //Warlock
            case 312:
                LearnOnlyTalentsAndGlyphs(player, "AfflictionPVE");
                break;

            case 313:
                LearnOnlyTalentsAndGlyphs(player, "DemonologyPVE");
                break;

            case 314:
                LearnOnlyTalentsAndGlyphs(player, "DemonologyPVE");
                break;

                //Shaman
            case 315:
                LearnOnlyTalentsAndGlyphs(player, "ElementalPVE");
                break;

            case 316:
                LearnOnlyTalentsAndGlyphs(player, "EnhancementPVE");
                break;

            case 317:
                LearnOnlyTalentsAndGlyphs(player, "RestorationPVE");
                break;

                //Druid
            case 318:
                LearnOnlyTalentsAndGlyphs(player, "BallancePVE");
                break;

            case 319:
                LearnOnlyTalentsAndGlyphs(player, "FeralPVE");
                break;

            case 320:
                LearnOnlyTalentsAndGlyphs(player, "RestorationPVE");
                break;

                //Hunter
            case 321:
                LearnOnlyTalentsAndGlyphs(player, "MarksmanshipPVE");
                break;

            case 322:
                LearnOnlyTalentsAndGlyphs(player, "BeastmasteryPVE");
                break;

            case 323:
                LearnOnlyTalentsAndGlyphs(player, "SurvivalPVE");
                break;

                //Rogue
            case 324:
                LearnOnlyTalentsAndGlyphs(player, "AssasinationPVE");
                break;

            case 325:
                LearnOnlyTalentsAndGlyphs(player, "CombatPVE");
                break;

            case 326:
                LearnOnlyTalentsAndGlyphs(player, "SubtletyPVE");
                break;

                //DK
            case 327:
                LearnOnlyTalentsAndGlyphs(player, "BloodPVE");
                break;

            case 328:
                LearnOnlyTalentsAndGlyphs(player, "FrostPVE");
                break;

            case 329:
                LearnOnlyTalentsAndGlyphs(player, "UnholyPVE");
                break;

            case 5000:
                // return to OnGossipHello menu, otherwise it will freeze every menu
                OnGossipHello(player, creature);
                break;

            default: // Just in case
                player->GetSession()->SendAreaTriggerMessage(isSpanish ? "Algo fue mal. Por favor, contacta con un administrador." : "Something went wrong in the code. Please contact the administrator.");
                break;
            }

        player->CLOSE_GOSSIP_MENU();
            player->UpdateSkillsForLevel();
        player->SaveToDB(false, false);

            return true;
        }
};

class TemplateNPC_command : public CommandScript
{
public:
    TemplateNPC_command() : CommandScript("TemplateNPC_command") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> saveTable = {
            {"horde", SEC_ADMINISTRATOR, false, &HandleSaveGearHorde, "<spec>, example: `.template save horde Arms`"},
            {"alliance", SEC_ADMINISTRATOR, false, &HandleSaveGearAlly, "<spec>, example: `.template save alliance Arms`"},
            {"human", SEC_ADMINISTRATOR, false, &HandleSaveGearHuman, "<spec>, example: `.template save human Arms`"},
        };

        static std::vector<ChatCommand> TemplateNPCTable = {
            {"copy", SEC_ADMINISTRATOR, false, &HandleCopyCommand, "Copies your target's gear onto your character. example: `.template copy`"},
            {"save", SEC_ADMINISTRATOR, false, nullptr, "", saveTable},
            {"reload", SEC_ADMINISTRATOR, true, &HandleReloadTemplateNPCCommand, ""},
        };

        static std::vector<ChatCommand> commandTable = {
            {"template", SEC_ADMINISTRATOR, true, nullptr, "", TemplateNPCTable},
        };

        return commandTable;
    }

    static bool HandleCopyCommand(ChatHandler* handler, const char* _args)
    {
        Player* caller = handler->GetSession()->GetPlayer();
        if (!caller)
        {
            assert(false);
        }
        uint64 selected = handler->GetSession()->GetPlayer()->GetTarget();
        if (!selected)
        {
            handler->SendGlobalGMSysMessage("You have to select a player");
            return false;
        }
        Player* target = ObjectAccessor::FindPlayerInOrOutOfWorld(selected);

        if (!target)
        {
            handler->SendGlobalGMSysMessage("You have to select a player.");
            return false;
        }

        if (caller == target)
        {
            handler->SendGlobalGMSysMessage("You are trying to copy your own gear, which is pointless.");
            return false;
        }

        if (caller->getClass() != target->getClass())
        {
            handler->SendGlobalGMSysMessage("In order to copy someone you need to be of the same class.");
            return false;
        }

        sTemplateNpcMgr->Copy(caller, target);

        handler->SendGlobalGMSysMessage("Character copied.");

        return true;
    }

    static bool HandleSaveGearHorde(ChatHandler* handler, const char* _args)
    {
        return HandleSaveCommon(handler, _args, TEMPLATE_HORDE);
    }

    static bool HandleSaveGearAlly(ChatHandler* handler, const char* _args)
    {
        return HandleSaveCommon(handler, _args, TEMPLATE_ALLIANCE);
    }

    static bool HandleSaveGearHuman(ChatHandler* handler, const char* _args)
    {
        return HandleSaveCommon(handler, _args, TEMPLATE_HUMAN);
    }

    static bool HandleSaveCommon(ChatHandler* handler, const char* _args, TemplateType type)
    {
        if (!*_args)
            return false;

        auto spec = std::string(_args);

        auto selectedSpec = knownSpecs.find(spec);
        if (selectedSpec == knownSpecs.end())
        {
            handler->SendGlobalGMSysMessage("Unknown spec passed");
            return false;
        }

        Player* p = handler->getSelectedPlayerOrSelf();
        if (p == nullptr)
            return false;

        sTemplateNpcMgr->ExtractGearTemplateToDB(p, spec, type);

        handler->SendGlobalGMSysMessage("Template extracted to DB. You might want to type \".template reload\".");

        return true;
    }

    static bool HandleReloadTemplateNPCCommand(ChatHandler* handler, const char* /*args*/)
    {
        sLog->outString("Reloading templates for Template NPC table...");
        sTemplateNpcMgr->LoadTalentsContainer();
        sTemplateNpcMgr->LoadGlyphsContainer();
        sTemplateNpcMgr->LoadHumanGearContainer();
        sTemplateNpcMgr->LoadAllianceGearContainer();
        sTemplateNpcMgr->LoadHordeGearContainer();
        handler->SendGlobalGMSysMessage("Template NPC templates reloaded.");
        return true;
    }
};

class TemplateNPC_World : public WorldScript
{
public:
    TemplateNPC_World() : WorldScript("TemplateNPC_World") { }

    void OnStartup() override
    {
        // Load templates for Template NPC #1
        sLog->outString("== TEMPLATE NPC ===========================================================================");
        sLog->outString("Loading Template Talents...");
        sTemplateNpcMgr->LoadTalentsContainer();

        // Load templates for Template NPC #2
        sLog->outString("Loading Template Glyphs...");
        sTemplateNpcMgr->LoadGlyphsContainer();

        // Load templates for Template NPC #3
        sLog->outString("Loading Template Gear for Humans...");
        sTemplateNpcMgr->LoadHumanGearContainer();

        // Load templates for Template NPC #4
        sLog->outString("Loading Template Gear for Alliances...");
        sTemplateNpcMgr->LoadAllianceGearContainer();

        // Load templates for Template NPC #5
        sLog->outString("Loading Template Gear for Hordes...");
        sTemplateNpcMgr->LoadHordeGearContainer();
        sLog->outString("== END TEMPLATE NPC ===========================================================================");
    }
};

void AddSC_TemplateNPC()
{
    new TemplateNPC();
    new TemplateNPC_command();
    new TemplateNPC_World();
}
