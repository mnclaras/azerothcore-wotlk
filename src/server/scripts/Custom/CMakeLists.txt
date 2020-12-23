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
  ${AC_SCRIPTS_DIR}/Custom/Dementia/Arena_Anti-Draw.cpp
  ${AC_SCRIPTS_DIR}/Custom/HolidayEvents/custom_hallows_end_event.cpp
  ${AC_SCRIPTS_DIR}/Custom/HolidayEvents/custom_christmas_event.cpp
  ${AC_SCRIPTS_DIR}/Custom/Others/custom_npc_tools_reset.cpp
  ${AC_SCRIPTS_DIR}/Custom/Others/custom_vip_only_npc.cpp
  ${AC_SCRIPTS_DIR}/Custom/PVP/arenaladder.cpp
)

AC_ADD_SCRIPT_LOADER("Custom" "ScriptLoader.h")

message("  -> Prepared: Custom Scripts")
