
#include "game.h"
#include "globals.h"
#include "renderer.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// Game States
enum GameState {
  STATE_WELCOME,
  STATE_LEVEL_MSG,
  STATE_PLAYING,
  STATE_LEVEL_COMPLETE,
  STATE_ALL_COMPLETE
};

GameState current_game_state = STATE_WELCOME;

enum Tile { EMPTY = 0, WALL = 1, BLOCK = 2, TARGET = 3, FILLED_TARGET = 4 };

struct UndoState {
  std::vector<std::vector<Tile>> map;
  int px, py;
  bool holding_block;
  int moves;
};

struct State {
  std::vector<std::vector<Tile>> map;
  int px, py;
  bool holding_block;
  int moves;
};

State current_state;
std::stack<State> history;

// Player starting position from map file
int START_X = 1;
int START_Y = 1;

// Message timer
float msg_timer = 0.0f;

void reset_globals_sokoban() {
  LEVEL_MOVES = 0;
  LEVEL_COMPLETE = false;
}

void load_map(const char *filename) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    MAP_WIDTH = 15;
    MAP_HEIGHT = 10;
    return;
  }

  GAME_BOARD.clear();
  std::string line;
  MAP_WIDTH = 0;
  MAP_HEIGHT = 0;

  while (std::getline(in, line)) {
    if (!line.empty() && line[0] == 'p') {
      std::string str = line.substr(1);
      size_t space_pos = str.find(' ');
      if (space_pos != std::string::npos) {
        START_X = std::stoi(str.substr(0, space_pos));
        START_Y = std::stoi(str.substr(space_pos + 1));
      }
      break;
    }

    GAME_BOARD.push_back(line);
    if (line.length() > MAP_WIDTH) {
      MAP_WIDTH = line.length();
    }
    MAP_HEIGHT++;
  }
  in.close();

  for (auto &row : GAME_BOARD) {
    row.resize(MAP_WIDTH, '.');
  }
}

void init_game() {
  std::string mapName = MAPS_LOCATION "/map";
  std::stringstream ss;
  ss << CURRENT_LEVEL;
  mapName += ss.str() + ".txt";

  load_map(mapName.c_str());

  current_state.px = START_X;
  current_state.py = START_Y;
  current_state.holding_block = false;
  current_state.moves = 0;

  current_state.map.clear();
  current_state.map.resize(MAP_HEIGHT);

  for (int y = 0; y < MAP_HEIGHT; ++y) {
    current_state.map[y].resize(MAP_WIDTH);
    for (int x = 0; x < MAP_WIDTH; ++x) {
      char c = (y < GAME_BOARD.size() && x < GAME_BOARD[y].size())
                   ? GAME_BOARD[y][x]
                   : '.';

      if (c == '#')
        current_state.map[y][x] = WALL;
      else if (c == 'B')
        current_state.map[y][x] = BLOCK;
      else if (c == 'T')
        current_state.map[y][x] = TARGET;
      else
        current_state.map[y][x] = EMPTY;
    }
  }

  while (!history.empty())
    history.pop();
  history.push(current_state);

  reset_globals_sokoban();
}

void save_state() { history.push(current_state); }

void undo() {
  if (history.size() > 1) {
    history.pop();
    current_state = history.top();
    if (LEVEL_MOVES > 0)
      LEVEL_MOVES--;
    if (TOTAL_MOVES > 0)
      TOTAL_MOVES--;
  }
}

bool check_win() {
  for (int y = 0; y < MAP_HEIGHT; ++y) {
    for (int x = 0; x < MAP_WIDTH; ++x) {
      if (current_state.map[y][x] == TARGET)
        return false;
    }
  }
  if (current_state.holding_block)
    return false;
  return true;
}

void update_play_input() {
  int dx = 0, dy = 0;
  bool moved = false;

  if (is_key_pressed(KEY_U)) {
    undo();
    return;
  }

  // Movement
  if (is_key_pressed(KEY_H) || is_key_pressed(KEY_LEFT)) {
    dx = -1;
    moved = true;
  } else if (is_key_pressed(KEY_L) || is_key_pressed(KEY_RIGHT)) {
    dx = 1;
    moved = true;
  } else if (is_key_pressed(KEY_J) || is_key_pressed(KEY_DOWN)) {
    dy = 1;
    moved = true;
  } else if (is_key_pressed(KEY_K) || is_key_pressed(KEY_UP)) {
    dy = -1;
    moved = true;
  }

  // Actions
  if (is_key_pressed(KEY_X) || is_key_pressed(KEY_D)) {
    if (!current_state.holding_block) {
      Tile t = current_state.map[current_state.py][current_state.px];
      if (t == BLOCK || t == FILLED_TARGET) {
        save_state();
        current_state.holding_block = true;
        if (t == FILLED_TARGET)
          current_state.map[current_state.py][current_state.px] = TARGET;
        else
          current_state.map[current_state.py][current_state.px] = EMPTY;
        LEVEL_MOVES++;
        TOTAL_MOVES++;
      }
    }
  } else if (is_key_pressed(KEY_P)) {
    if (current_state.holding_block) {
      Tile t = current_state.map[current_state.py][current_state.px];
      if (t == EMPTY || t == TARGET) {
        save_state();
        current_state.holding_block = false;
        if (t == TARGET)
          current_state.map[current_state.py][current_state.px] = FILLED_TARGET;
        else
          current_state.map[current_state.py][current_state.px] = BLOCK;
        LEVEL_MOVES++;
        TOTAL_MOVES++;
      }
    }
  }

  if (moved && (dx != 0 || dy != 0)) {
    int nx = current_state.px + dx;
    int ny = current_state.py + dy;
    if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT) {
      if (current_state.map[ny][nx] != WALL) {
        save_state();
        current_state.px = nx;
        current_state.py = ny;
        LEVEL_MOVES++;
        TOTAL_MOVES++;
      }
    }
  }

  // Check Win
  if (check_win()) {
    LEVEL_COMPLETE = true;
    current_game_state = STATE_LEVEL_COMPLETE;
  }
}

void draw_play() {
  for (int y = 0; y < MAP_HEIGHT; ++y) {
    for (int x = 0; x < MAP_WIDTH; ++x) {
      Tile t = current_state.map[y][x];
      if (t == WALL)
        draw_entity(x, y, ENTITY_WALL);
      else if (t == BLOCK)
        draw_entity(x, y, ENTITY_CRATE);
      else if (t == TARGET)
        draw_entity(x, y, ENTITY_TARGET);
      else if (t == FILLED_TARGET)
        draw_entity(x, y, ENTITY_FILLED_TARGET);

      if (x == current_state.px && y == current_state.py) {
        draw_entity(x, y, ENTITY_PLAYER);
      }
    }
  }

  // UI
  draw_text(20, 500,
            TextFormat("Level: %d/%d", CURRENT_LEVEL + 1, NUM_OF_LEVELS), 20,
            WHITE);
  draw_text(20, 525,
            TextFormat("Moves: %d (Total: %d)", LEVEL_MOVES, TOTAL_MOVES), 20,
            WHITE);
  draw_text(20, 550,
            current_state.holding_block ? "Holding Box" : "Empty Handed", 20,
            current_state.holding_block ? YELLOW : GRAY);

  draw_text(400, 500, "[h,j,k,l] Move  [x] Pick  [p] Put  [u] Undo", 20,
            LIGHTGRAY);
}

// --- Screens ---

void draw_welcome() {
  draw_text_centered(100, "SOKOBAN VIM", 40, MAGENTA);
  draw_text_centered(200, "Objective: Place all boxes on targets", 20, WHITE);
  draw_text_centered(250, "Controls: Vim Keys (h, j, k, l)", 20, WHITE);
  draw_text_centered(300, "x: Pick Box | p: Place Box | u: Undo", 20, WHITE);
  draw_text_centered(450, "PRESS ENTER TO START", 20, GREEN);
}

void update_welcome() {
  if (is_key_pressed(KEY_ENTER)) {
    current_game_state = STATE_LEVEL_MSG;
    msg_timer = 0.0f;
  }
}

void draw_level_msg() {
  draw_text_centered(250, TextFormat("LEVEL %d", CURRENT_LEVEL + 1), 40,
                     YELLOW);
  draw_text_centered(320, "Get Ready...", 20, WHITE);
}

void update_level_msg() {
  msg_timer += GetFrameTime();
  if (msg_timer > 2.0f) {
    init_game();
    current_game_state = STATE_PLAYING;
  }
}

void draw_level_complete() {
  draw_text_centered(200, "LEVEL COMPLETE!", 40, GREEN);
  draw_text_centered(300, TextFormat("Moves: %d", LEVEL_MOVES), 30, WHITE);
  draw_text_centered(450, "PRESS ENTER FOR NEXT LEVEL", 20, YELLOW);
}

void update_level_complete() {
  if (is_key_pressed(KEY_ENTER)) {
    if (CURRENT_LEVEL >= NUM_OF_LEVELS - 1) {
      current_game_state = STATE_ALL_COMPLETE;
    } else {
      CURRENT_LEVEL++;
      current_game_state = STATE_LEVEL_MSG;
      msg_timer = 0.0f;
    }
  }
}

void draw_all_complete() {
  draw_text_centered(200, "ALL LEVELS CLEARED!", 40, GOLD);
  draw_text_centered(300, TextFormat("Total Moves: %d", TOTAL_MOVES), 30,
                     WHITE);
  draw_text_centered(450, "PRESS ENTER TO RESTART", 20, GREEN);
}

void update_all_complete() {
  if (is_key_pressed(KEY_ENTER)) {
    CURRENT_LEVEL = 0;
    TOTAL_MOVES = 0;
    current_game_state = STATE_WELCOME;
  }
}

int sokoban_main(int argc, char **argv) {
  init_renderer();

  while (!window_should_close()) {
    clear_screen();

    switch (current_game_state) {
    case STATE_WELCOME:
      update_welcome();
      draw_welcome();
      break;
    case STATE_LEVEL_MSG:
      update_level_msg();
      draw_level_msg();
      break;
    case STATE_PLAYING:
      update_play_input();
      draw_play();
      break;
    case STATE_LEVEL_COMPLETE:
      update_level_complete();
      draw_level_complete();
      break;
    case STATE_ALL_COMPLETE:
      update_all_complete();
      draw_all_complete();
      break;
    }

    refresh_screen();
  }

  close_renderer();
  return 0;
}