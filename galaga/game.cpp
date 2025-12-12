#include "game.h"
#include "globals.h"
#include "renderer.h"
#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

struct Bullet {
  int x, y;
  bool active;
};

struct Enemy {
  int x, y;
  bool active;
};

std::vector<Bullet> bullets;
std::vector<Enemy> enemies;
int tick_counter = 0;
bool game_running = true; // For outer loop

void reset_game() {
  enemies.clear();
  bullets.clear();
  GAME_OVER = false;
  SCORE = 0;
  LIVES = 3;
  PLAYER_X = GAME_WIDTH / 2;
  PLAYER_Y = GAME_HEIGHT - 1;
  tick_counter = 0;
}

void spawn_enemies() {
  // Simple formation
  for (int i = 0; i < 6; ++i) {
    for (int j = 0; j < 3; ++j) {
      // Center the enemies: Width 40. 6*4=24. Margin (40-24)/2 = 8.
      enemies.push_back({8 + i * 4, 2 + j * 2, true});
    }
  }
}

void update_bullets() {
  for (auto &b : bullets) {
    if (b.active) {
      b.y--;
      if (b.y < 1)
        b.active = false; // Top border is at 0
    }
  }
  bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                               [](const Bullet &b) { return !b.active; }),
                bullets.end());
}

void update_enemies() {
  tick_counter++;
  // Move enemies
  int speed = 5;
  if (SCORE > 1000)
    speed = 4;
  if (SCORE > 2000)
    speed = 3;

  if (tick_counter % speed == 0) {
    for (auto &e : enemies) {
      if (e.active) {
        e.y++;
        if (e.y >= GAME_HEIGHT - 1) {
          // Reached bottom
          GAME_OVER = true;
        }
      }
    }
  }
}

void check_collisions() {
  for (auto &b : bullets) {
    if (!b.active)
      continue;
    for (auto &e : enemies) {
      if (!e.active)
        continue;
      // Collision: Bullet at (x,y) hits Enemy at (x,y)
      // Since x is discrete logic coord, exact match is fine.
      if (b.x == e.x && b.y == e.y) {
        b.active = false;
        e.active = false;
        SCORE += 100;
      }
    }
  }

  // Check collision with player
  for (const auto &e : enemies) {
    if (e.active && e.x == PLAYER_X && e.y == PLAYER_Y) {
      GAME_OVER = true;
    }
  }
}

void draw_game() {
  clear_screen();

  // Draw main border
  // logical width GAME_WIDTH maps to 2*GAME_WIDTH characters.
  // height is GAME_HEIGHT
  draw_box(GAME_WIDTH, GAME_HEIGHT);

  // Draw Player
  draw_entity_colored(PLAYER_X, PLAYER_Y, "🚀", 6); // Cyan Ship

  // Draw Enemies
  for (const auto &e : enemies) {
    if (e.active)
      draw_entity_colored(e.x, e.y, "👾", 5); // Magenta Aliens
  }

  // Draw Bullets
  for (const auto &b : bullets) {
    if (b.active)
      draw_entity_colored(b.x, b.y, "🔥", 3); // Yellow Fire
  }

  // UI Stats
  int uiY = GAME_HEIGHT + 1;
  std::stringstream ss;
  ss << "Score: " << SCORE;
  draw_text_colored(2, uiY, ss.str(), 3); // Yellow

  ss.str("");
  ss << "Lives: ";
  draw_text_colored(20, uiY, ss.str(), 3);

  std::string hearts = "";
  for (int i = 0; i < LIVES; ++i)
    hearts += "❤️ ";
  draw_text(27, uiY, hearts);

  // Controls hint
  uiY++;
  draw_text_colored(
      2, uiY, "[h] Left [l] Right [k] Up [j] Down [SPACE] Shoot [q] Quit", 7);

  refresh_screen();
}

void welcome_screen() {
  clear_screen();

  int y = 3;
  draw_text_centered_colored(
      y++, "╔═══════════════════════════════════════════╗", 3);
  draw_text_centered_colored(
      y++, "║                                           ║", 3);
  draw_text_centered_colored(
      y++, "║         🚀  GALAGA VIM  👾                ║", 6);
  draw_text_centered_colored(
      y++, "║                                           ║", 3);
  draw_text_centered_colored(
      y++, "║      Defend the galaxy with Vim keys!     ║", 3);
  draw_text_centered_colored(
      y++, "║                                           ║", 3);
  draw_text_centered_colored(
      y++, "╚═══════════════════════════════════════════╝", 3);

  y += 2;
  draw_text_centered_colored(y++, "🎮 CONTROLS:", 3);
  draw_text_centered(y++, "Use [h][j][k][l] to Move Ship 🚀");
  draw_text_centered(y++, "Press [SPACE] to Fire 🔥");

  y += 2;
  draw_text_centered_colored(y++, "⚠️  WARNING:", 1);
  draw_text_centered(y++, "Don't let the aliens reach the bottom!");

  y += 3;
  draw_text_centered_colored(y++, "✨ PRESS ENTER TO LAUNCH ✨", 2);
  draw_text_centered_colored(y++, "Press 'q' to abort mission", 7);

  refresh_screen();

  while (true) {
    int ch = get_input(); // getch wrapper
    if (ch == '\n' || ch == '\r')
      return;
    if (ch == 'q') {
      close_renderer();
      exit(0);
    }
  }
}

void game_over_screen() {
  clear_screen();

  int y = 5;
  draw_text_centered_colored(y++, "╔═════════════════════════════╗", 1);
  draw_text_centered_colored(y++, "║                             ║", 1);
  draw_text_centered_colored(y++, "║       💥  GAME OVER  💥     ║", 1);
  draw_text_centered_colored(y++, "║                             ║", 1);
  draw_text_centered_colored(y++, "╚═════════════════════════════╝", 1);

  y += 2;
  std::stringstream ss;
  ss << "Final Score: " << SCORE;
  draw_text_centered_colored(y++, ss.str(), 3);

  y += 2;
  draw_text_centered_colored(y++, "Press ENTER to Deploy Again", 2);
  draw_text_centered_colored(y++, "Press 'q' to Quit", 7);

  refresh_screen();

  while (true) {
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

int galaga_main(int /*argc*/, char ** /*argv*/) {
  init_renderer();

  welcome_screen();

  while (game_running) {
    reset_game();
    spawn_enemies();

    while (!GAME_OVER) {
      int ch = get_input(); // getch()
      if (ch == 'q') {
        GAME_OVER = true; // End round
      }

      // Movement
      if (ch == 'h') {
        if (PLAYER_X > 1)
          PLAYER_X--;
      } else if (ch == 'l') {
        if (PLAYER_X < GAME_WIDTH - 2)
          PLAYER_X++; // Boundary check adjusted for box
      } else if (ch == 'j') {
        if (PLAYER_Y < GAME_HEIGHT - 2)
          PLAYER_Y++; // Boundary check
      } else if (ch == 'k') {
        if (PLAYER_Y > 1)
          PLAYER_Y--;
      }
      // Action
      else if (ch == ' ') {
        bullets.push_back({PLAYER_X, PLAYER_Y - 1, true});
      }

      update_bullets();
      update_enemies();
      check_collisions();
      draw_game();

      // Check wave clear
      bool all_dead = true;
      for (const auto &e : enemies) {
        if (e.active) {
          all_dead = false;
          break;
        }
      }
      if (all_dead) {
        spawn_enemies();
        // Maybe increase difficulty/speed?
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    game_over_screen();
  }

  close_renderer();
  return 0;
}