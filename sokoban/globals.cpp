/*
 * Sokoban - Global Variables Implementation
 */

#include "globals.h"
#include <vector>
#include <string>

int CURRENT_LEVEL = 0;
const int NUM_OF_LEVELS = 10;
int TOTAL_MOVES = 0;
int LEVEL_MOVES = 0;
bool GAME_WON = false;
bool LEVEL_COMPLETE = false;

int MAP_WIDTH = 0;
int MAP_HEIGHT = 0;
int OFFSET_X = 2;
int OFFSET_Y = 1;

std::vector<std::string> GAME_BOARD;