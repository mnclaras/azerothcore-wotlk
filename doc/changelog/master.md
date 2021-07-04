## 4.0.0-dev.7 | Commit: [59a3912a3b3bd4dd2d8e2b1c2cdd225b9c4d6244
](https://github.com/azerothcore/azerothcore-wotlk/commit/59a3912a3b3bd4dd2d8e2b1c2cdd225b9c4d6244


### Removed
- Old gossips api [#5414](https://github.com/azerothcore/azerothcore-wotlk/pull/5414)

### How to upgrade
- `player->ADD_GOSSIP_ITEM(whatever)` -> `AddGossipItemFor(player, whatever)`
- `player->ADD_GOSSIP_ITEM_DB(whatever)` -> `AddGossipItemFor(player, whatever)`
- `player->ADD_GOSSIP_ITEM_EXTENDED(whatever)` -> `AddGossipItemFor(player, whatever)`
- `player->CLOSE_GOSSIP_MENU()` -> `CloseGossipMenuFor(player)`
- `player->SEND_GOSSIP_MENU(textid, creature->GetGUID())` -> `SendGossipMenuFor(player, textid, creature->GetGUID())`

You also need  `#include "ScriptedGossip.h"` in your cpp files

## 4.0.0-dev.6 | Commit: [59a3912a3b3bd4dd2d8e2b1c2cdd225b9c4d6244
](https://github.com/azerothcore/azerothcore-wotlk/commit/59a3912a3b3bd4dd2d8e2b1c2cdd225b9c4d6244


### Changed
- New options for loading scripts `static dynamic minimal-static minimal-dynamic` [#5346](https://github.com/azerothcore/azerothcore-wotlk/pull/5346)
```
static - Build statically. Default option. for all scripts (As it was before)
dynamic - Build dynamically. After start support Dynamic Linking Library (DLL) can make separated library for each script. Now don't support
minimal-static - builds commands and spells statically
minimal-dynamic - builds commands and spells dynamically. Now don't support
```
- Also the default value which is provided by the `SCRIPTS` variable is overwriteable through the `SCRIPTS_COMMANDS, SCRIPTS_SPELLS...` variable.
- Each subdirectory contains it's own translation unit now which is responsible for loading it's directory
- If module using deprecated script loader api, you get error message.
```cmake
> Module (mod-ah-bot) using deprecated loader api
```

### How to upgrade
- For most modules, the `CMakeLists.txt' file is no longer needed
- Need change script loader file.
```
1. Rename extension in file to `.cpp`
2. Rename general loading function to `Add(module name with replace all whitespace to '_')Scripts()`.
3. Delete macros `AC_ADD_SCRIPT_LOADER` from `CMakeLists.txt`
```
- Example loader script for modules:
```cpp
/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

// From SC
void AddSC_ServerAutoShutdown();

// Add all scripts
void Addmod_server_auto_shutdownScripts()
{
    AddSC_ServerAutoShutdown();
}
```
- List modules support new script loader api:
https://github.com/azerothcore/mod-server-auto-shutdown

## 4.0.0-dev.5 | Commit: [59a3912a3b3bd4dd2d8e2b1c2cdd225b9c4d6244
](https://github.com/azerothcore/azerothcore-wotlk/commit/59a3912a3b3bd4dd2d8e2b1c2cdd225b9c4d6244


### Added
- New cmake option `WITH_STRICT_DATABASE_TYPE_CHECKS` [#5611](https://github.com/azerothcore/azerothcore-wotlk/pull/5611)

### Changed
- Prevent mixing databases with query holders [#5611](https://github.com/azerothcore/azerothcore-wotlk/pull/5611)
- Prevent using prepared statements on wrong database [#5611](https://github.com/azerothcore/azerothcore-wotlk/pull/5611)
- Prevent committing transactions started on a different database [#5611](https://github.com/azerothcore/azerothcore-wotlk/pull/5611)
- Convert async queries to new query callbacks [#5611](https://github.com/azerothcore/azerothcore-wotlk/pull/5611)

### How to upgrade
- `PreparedStatement`
```diff
- PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LOGONPROOF);
+ LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_LOGONPROOF);
```
- `SQLTransaction`
```diff
- SQLTransaction trans = CharacterDatabase.BeginTransaction();
+ CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
```
## 4.0.0-dev.4 | Commit: [fbad1f3d6c27a5d3eea22483913c67a827ab01be
](https://github.com/azerothcore/azerothcore-wotlk/commit/fbad1f3d6c27a5d3eea22483913c67a827ab01be


### Added
- new hook `OnBeforeSendJoinMessageArenaQueue` and `OnBeforeSendExitMessageArenaQueue`

### Changed
- Rename `CanExitJoinMessageArenaQueue` to `OnBeforeSendExitMessageArenaQueue`
- Rename `CanSendJoinMessageArenaQueue` to `OnBeforeSendJoinMessageArenaQueue`

### How to upgrade
- Just rename all hooks from `CanExitJoinMessageArenaQueue` and `CanSendMessageArenaQueue`, to `OnBeforeSendExitMessageArenaQueue`
- Just rename all hooks from `CanSendJoinMessageArenaQueue` and `OnBeforeSendJoinMessageArenaQueue`

## 4.0.0-dev.3 | Commit: [c35dde6fae732269357b78fb796fba21956b83fc
](https://github.com/azerothcore/azerothcore-wotlk/commit/c35dde6fae732269357b78fb796fba21956b83fc


Changelog for commit "[refactor(Collision): Update some methods to UpperCamelCase](https://github.com/azerothcore/azerothcore-wotlk/commit/b84f9b8a4b334632cb37dcebbb2dd4e087f65610)"

### Changes

```diff
- getPosition
- getBounds
- getBounds2
- getInstanceMapTree
- getModelInstances
- getPosInfo
- getMeshData
- getGroupModels
- getIntersectionTime
- getObjectHitPos
- getAreaInfo
+ GetPosition
+ GetBounds
+ GetBounds2
+ GetInstanceMapTree
+ GetModelInstances
+ GetPosInfo
+ GetMeshData
+ GetGroupModels
+ GetIntersectionTime
+ GetObjectHitPos
+ GetAreaInfo
```

### How to upgrade

If you are using any of those methods, simply rename it by changing the first letter of the method from lowercase to uppercase.

Example: `getAreaInfo` -> `GetAreaInfo`

## 4.0.0-dev.2 | Commit: [3f70d0b80ff483f142ffbebf8960aeb503913a35](https://github.com/azerothcore/azerothcore-wotlk/commit/3f70d0b80ff483f142ffbebf8960aeb503913a35)


### Added
- Created new changelog system.

### How to upgrade

To create a new changelog please follow the instructions on our [wiki page](https://www.azerothcore.org/wiki/how-to-use-changelog)

