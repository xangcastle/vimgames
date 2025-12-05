#pragma once

#include <string>

namespace engine {

enum Color {
  COLOR_DEFAULT = 0,
  COLOR_RED,
  COLOR_GREEN,
  COLOR_BLUE,
  COLOR_YELLOW,
  COLOR_CYAN,
  COLOR_MAGENTA,
  COLOR_WHITE
};

void draw_string(int x, int y, const std::string &text, int color_pair = 0);
void draw_box(int x, int y, int width, int height, int color_pair = 0);
void clear_screen();
void refresh_screen();
int get_screen_width();
int get_screen_height();
void capture_screen(const std::string &filename);
int get_char_at(int x, int y);

} // namespace engine
