#pragma once
#include "../utility/EventManager.h"
#include "EnergyTypes.h"
#include <string>
class Playership;
class Enemyship;
class Terrain;
class ParserSection;
class Crater;
class Spawnpoint;

EVENT_WITH_CONST_REFERENCE(Level_Load, ParserSection);
SIMPLE_EVENT(Level_Unload);
EVENT_WITH_POINTER(Terrain_Changed, Terrain);
EVENT_WITH_POINTER(Player_Spawned, Playership);
EVENT_WITH_POINTER(Enemy_Spawned, Enemyship);
EVENT_WITH_POINTER(Enemy_Destroyed, Enemyship);
EVENT_WITH_POINTER_AND_VALUE(Player_Destroyed, Playership, EnergyType);
EVENT_WITH_POINTER_AND_2VALUES(Player_Drained, Playership, EnergyType, int); // int is amount of energy drained
EVENT_WITH_POINTER(Crater_Spawned, Crater);
EVENT_WITH_POINTER(Spawnpoint_Spawned, Spawnpoint);
EVENT_WITH_POINTER_AND_2VALUES(Enemy_Hit, Enemyship, EnergyType, int);
EVENT_WITH_2POINTERS(Enemy_Attached_To_Player, Enemyship, Playership);
