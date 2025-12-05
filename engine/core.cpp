#define _XOPEN_SOURCE_EXTENDED
#include "core.h"
#include <chrono>
#include <clocale>
#include <ncurses.h>
#include <thread>

namespace engine {

void init(const EngineConfig &config) {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  use_default_colors();

  // Initialize standard colors (1-7) to match common expectations
  init_pair(COLOR_RED, COLOR_RED, -1);
  init_pair(COLOR_GREEN, COLOR_GREEN, -1);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, -1);
  init_pair(COLOR_BLUE, COLOR_BLUE, -1);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, -1);
  init_pair(COLOR_CYAN, COLOR_CYAN, -1);
  init_pair(COLOR_WHITE, COLOR_WHITE, -1);

  timeout(0); // Non-blocking input
}

void run(std::function<bool()> update_callback) {
  bool running = true;
  while (running) {
    running = update_callback();
    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS cap
  }
}

void shutdown() { endwin(); }

} // namespace engine
