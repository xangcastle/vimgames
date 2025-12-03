/*
 * Sokoban - Global Variables and Constants
 */

#ifndef SOKOBAN_GLOBALS_H
#define SOKOBAN_GLOBALS_H

#include <string>
#include <vector>

// Game state
extern int CURRENT_LEVEL;
extern const int NUM_OF_LEVELS;
extern int TOTAL_MOVES;
extern int LEVEL_MOVES;
extern bool GAME_WON;
extern bool LEVEL_COMPLETE;

// Map dimensions
extern int MAP_WIDTH;
extern int MAP_HEIGHT;
extern int OFFSET_X;
extern int OFFSET_Y;

// Visual settings
extern std::vector<std::string> GAME_BOARD;

#ifndef MAPS_LOCATION
#define MAPS_LOCATION "maps"
#endif

#endif