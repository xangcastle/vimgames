#include "game.h"
#include "renderer.h"
#include <cstdlib>
#include <ctime>

#include <string>
#include <vector>

#include <chrono>
#include <sstream>
#include <thread>

#define ERR -1
#define KEY_BACKSPACE 127

// Game Constants
const int WIDTH = 60;
const int HEIGHT = 20;
const int PLAYER_X = WIDTH / 2;
const int PLAYER_Y = HEIGHT - 2;

struct Enemy {
  int x, y;
  char trigger_char;
  std::string trigger_word;
  bool is_word; // true if it requires '/' command
  bool active;
};

std::vector<Enemy> enemies;
int score = 0;
int lives = 3;
bool game_over = false;
bool game_running = true; // For the outer loop
std::string command_buffer = "";
bool command_mode = false; // true if typing a command like /word
bool find_mode = false;    // true if waiting for char after 'f'

// Words for boss enemies
std::vector<std::string> dictionary = {
    "vim", "code", "exit", "quit",  "save",  "edit",  "bash", "grep", "sed",
    "awk", "find", "make", "build", "debug", "shell", "path", "root", "user"};

void reset_game() {
  enemies.clear();
  score = 0;
  lives = 3;
  game_over = false;
  command_buffer = "";
  command_mode = false;
  find_mode = false;
}

void spawn_enemy() {
  Enemy e;
  // Ensure x is within the box (1 to WIDTH-2)
  e.x = rand() % (WIDTH - 4) + 2;
  e.y = 1;
  e.active = true;

  if (rand() % 5 == 0) {
    // Boss enemy (Word)
    e.is_word = true;
    e.trigger_word = dictionary[rand() % dictionary.size()];
    e.trigger_char = 0;
  } else {
    // Normal enemy (Char)
    e.is_word = false;
    e.trigger_char = 'a' + (rand() % 26);
    e.trigger_word = "";
  }
  enemies.push_back(e);
}

void update_game() {
  static int tick = 0;
  tick++;

  // Spawn enemies occasionally
  // Increase difficulty by spawning faster as score increases?
  int spawn_rate = 40;
  if (score > 500)
    spawn_rate = 30;
  if (score > 1000)
    spawn_rate = 20;
  if (score > 2000)
    spawn_rate = 10;

  if (tick % spawn_rate == 0) {
    spawn_enemy();
  }

  // Move enemies
  if (tick % 10 == 0) {
    for (auto &e : enemies) {
      if (e.active) {
        e.y++;
        if (e.y >= HEIGHT - 1) {
          e.active = false;
          lives--;
          if (lives <= 0)
            game_over = true;
        }
      }
    }
  }
}

void handle_input(int ch) {
  if (ch == ERR)
    return;

  if (command_mode) {
    // Typing a command (search)
    if (ch == '\n' || ch == 10 || ch == 13) {
      // Enter
      // Check matches
      for (auto &e : enemies) {
        if (e.active && e.is_word && e.trigger_word == command_buffer) {
          e.active = false;
          score += 50;
        }
      }
      command_mode = false;
      command_buffer = "";
    } else if (ch == 27) {
      // ESC
      command_mode = false;
      command_buffer = "";
    } else if (ch == 127 || ch == '\b' || ch == KEY_BACKSPACE) {
      if (!command_buffer.empty())
        command_buffer.pop_back();
    } else {
      command_buffer += (char)ch;
    }
  } else if (find_mode) {
    // Waiting for char after 'f'
    // Vim 'f' jumps to the *next* occurrence.
    // We will target the lowest enemy (closest to player) that matches.
    int lowest_y = -1;
    int target_index = -1;

    for (size_t i = 0; i < enemies.size(); ++i) {
      if (enemies[i].active && !enemies[i].is_word &&
          enemies[i].trigger_char == ch) {
        if (enemies[i].y > lowest_y) {
          lowest_y = enemies[i].y;
          target_index = i;
        }
      }
    }

    if (target_index != -1) {
      enemies[target_index].active = false;
      score += 10;
    }

    find_mode = false;
  } else {
    // Normal mode
    if (ch == 'q') {
      game_over = true; // Just end the round, let game_over_screen handle quit
    } else if (ch == 'f') {
      find_mode = true;
    } else if (ch == '/') {
      command_mode = true;
      command_buffer = "";
    }
  }
}

void draw_game() {
  clear_screen();

  // Draw main border
  draw_box(WIDTH, HEIGHT);

  // Player
  draw_entity_colored(PLAYER_X, PLAYER_Y, "🧙", 5); // Magenta Wizard

  // Enemies
  for (const auto &e : enemies) {
    if (!e.active)
      continue;
    if (e.is_word) {
      draw_entity_colored(e.x, e.y, "👹 " + e.trigger_word, 1); // Red Demon
    } else {
      std::string s = "👻 ";
      s += e.trigger_char;
      draw_entity_colored(e.x, e.y, s, 6); // Cyan Ghost
    }
  }

  // UI Stats
  int uiY = HEIGHT + 1; // Row 26
  std::stringstream ss;
  ss << "Score: " << score;
  draw_text_colored(2, uiY, ss.str(), 3); // Yellow

  ss.str("");
  ss << "Lives: ";
  draw_text_colored(20, uiY, ss.str(), 3);

  // Draw hearts for lives
  std::string hearts = "";
  for (int i = 0; i < lives; ++i)
    hearts += "[[H]] ";
  draw_text(27, uiY, hearts);

  // Controls hint
  uiY++; // Row 27
  draw_text_colored(
      2, uiY, "[f]<char>: Hunt Ghost   [/]<word>: Hunt Demon   [q]: Give Up",
      7);

  // Status Line (Vim style)
  // Let's put it inside the box at the bottom, or just below the box
  // Actually, let's overwrite the bottom border line for the status bar effect

  // Status Line (Vim style) - Row 25 (Immediately below grid)
  std::string status = "";
  int statusY = HEIGHT;
  if (command_mode) {
    status = "/" + command_buffer;
    draw_text_colored(1, statusY, status, 7); // White
    // Cursor effect
    draw_text_colored(1 + status.length(), statusY, "█", 7);
  } else if (find_mode) {
    status = "f_";
    draw_text_colored(1, statusY, status, 3); // Yellow
  } else {
    status = "NORMAL";
    draw_text_colored(1, statusY, status, 2); // Green
  }

  refresh_screen();
}

void welcome_screen() {
  while (true) {
    if (renderer_should_close()) {
      close_renderer();
      exit(0);
    }
    clear_screen();

    int y = 3;
    draw_text_centered_colored(
        y++, "+-------------------------------------------+", 3);
    draw_text_centered_colored(
        y++, "|                                           |", 3);
    draw_text_centered_colored(
        y++, "|              HUNTER VIM                   |", 6);
    draw_text_centered_colored(
        y++, "|                                           |", 3);
    draw_text_centered_colored(
        y++, "|     Defend the code from bugs & demons!   |", 3);
    draw_text_centered_colored(
        y++, "|                                           |", 3);
    draw_text_centered_colored(
        y++, "+-------------------------------------------+", 3);

    y += 2;
    draw_text_centered_colored(y++, "CONTROLS:", 3);
    draw_text_centered(y++, "Type 'f' then a character to hunt ghosts (G)");
    draw_text_centered(y++, "(Example: 'fa' kills ghost 'a')");
    y++;
    draw_text_centered(y++, "Type '/' then a word + ENTER to hunt demons (D)");
    draw_text_centered(y++, "(Example: '/vim' kills demon 'vim')");

    y += 2;
    draw_text_centered_colored(y++, "WARNING:", 1);
    draw_text_centered(y++, "Don't let them reach the bottom line!");

    y += 3;
    draw_text_centered_colored(y++, "✨ PRESS ENTER TO START ✨", 2);
    draw_text_centered_colored(y++, "Press 'q' to quit", 7);

    refresh_screen();

    int ch = get_input(); // Non-blocking
    if (ch == '\n' || ch == '\r')
      return;
    if (ch == 'q') {
      close_renderer();
      exit(0);
    }
  }
}

void game_over_screen() {
  while (true) {
    if (renderer_should_close()) {
      game_running = false;
      return;
    }
    clear_screen();

    int y = 5;
    draw_text_centered_colored(y++, "+-----------------------------+", 1);
    draw_text_centered_colored(y++, "|                             |", 1);
    draw_text_centered_colored(y++, "|          GAME OVER          |", 1);
    draw_text_centered_colored(y++, "|                             |", 1);
    draw_text_centered_colored(y++, "+-----------------------------+", 1);

    y += 2;
    std::stringstream ss;
    ss << "Final Score: " << score;
    draw_text_centered_colored(y++, ss.str(), 3);

    y += 2;
    draw_text_centered_colored(y++, "Press ENTER to Try Again", 2);
    draw_text_centered_colored(y++, "Press 'q' to Quit", 7);

    refresh_screen();

    int ch = get_input();
    if (ch == '\n' || ch == '\r') {
      return; // Return to main loop, which will call reset_game
    }
    if (ch == 'q') {
      game_running = false;
      return;
    }
  }
}

int hunter_main(int /*argc*/, char ** /*argv*/) {
  srand(time(0));
  init_renderer();

  welcome_screen();

  while (game_running) {
    reset_game();

    // Game Loop
    while (!game_over) {
      int ch = get_input();
      handle_input(ch);
      update_game();
      draw_game();

      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    game_over_screen();
  }

  close_renderer();
  return 0;
}