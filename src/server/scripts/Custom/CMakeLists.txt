# Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# file(GLOB_RECURSE sources_Custom Custom/*.cpp Custom/*.h)

set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  ${AC_SCRIPTS_DIR}/Custom/ArenaSpectator/arena_spectator.cpp
  ${AC_SCRIPTS_DIR}/Custom/ArenaSpectator/custom_npc_leave_spectator.cpp
  ${AC_SCRIPTS_DIR}/Custom/Dementia/Arena_Anti-Draw.cpp
  ${AC_SCRIPTS_DIR}/Custom/PVP/arena_crystal.cpp
  ${AC_SCRIPTS_DIR}/Custom/PVP/arenaladder.cpp
  ${AC_SCRIPTS_DIR}/Custom/PVP/pvp_area.cpp
  ${AC_SCRIPTS_DIR}/Custom/PVP/bg_arena_reward.cpp
  ${AC_SCRIPTS_DIR}/Custom/PVP/gurubashi_chest_event.cpp
  ${AC_SCRIPTS_DIR}/Custom/PVP/deathmatch_area.cpp
  ${AC_SCRIPTS_DIR}/Custom/PVP/mod_bg_auras.cpp
  ${AC_SCRIPTS_DIR}/Custom/PVP/custom_npc_anti_pvp.cpp
  ${AC_SCRIPTS_DIR}/Custom/HolidayEvents/custom_hallows_end_event.cpp
  ${AC_SCRIPTS_DIR}/Custom/HolidayEvents/custom_christmas_event.cpp
  ${AC_SCRIPTS_DIR}/Custom/Events/events_automation.cpp
  ${AC_SCRIPTS_DIR}/Custom/Guild/mod_guild_points.h
  ${AC_SCRIPTS_DIR}/Custom/Guild/mod_guild_points.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_player_tools.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_amnesia_coin_exchange.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_global_trainer.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_npc_lottery.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_npc_pvp_titles.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_npc_reset_cds.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_npc_reset_combat.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_npc_reset_instances.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_npc_reset_bg_deserter.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_teleporter.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_item_teleporter.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_vip_only_npc.cpp
  ${AC_SCRIPTS_DIR}/Custom/zombie_event.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_npc_tier_nine.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_shop_collapsed_vendor.cpp
  ${AC_SCRIPTS_DIR}/Custom/custom_double_emblems_alliance.cpp  
)

AC_ADD_SCRIPT_LOADER("Custom" "ScriptLoader.h")

message("  -> Prepared: Custom")
