#include "renderer.h"
#include "engine/engine.h"
#include <string>

void init_renderer() {
  // Engine init handles ncurses setup and colors
}

void close_renderer() {
  // Engine shutdown handles this
}

void draw_entity(int x, int y, const std::string &text) {
  engine::draw_string(x, y, text);
}

void draw_entity_colored(int x, int y, const std::string &text, int color) {
  engine::draw_string(x, y, text, color);
}

void draw_text(int x, int y, const std::string &text) {
  engine::draw_string(x, y, text);
}

void draw_text_centered(int y, const std::string &text) {
  int cols = engine::get_screen_width();
  int x = (cols - text.length()) / 2;
  engine::draw_string(x, y, text);
}

void draw_text_colored(int x, int y, const std::string &text, int color) {
  engine::draw_string(x, y, text, color);
}

void draw_text_centered_colored(int y, const std::string &text, int color) {
  int cols = engine::get_screen_width();
  int x = (cols - text.length()) / 2;
  engine::draw_string(x, y, text, color);
}

void draw_box(int width, int height) { engine::draw_box(0, 0, width, height); }

void clear_screen() { engine::clear_screen(); }

void refresh_screen() { engine::refresh_screen(); }

int get_input() { return engine::get_key(); }