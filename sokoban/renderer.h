#ifndef SOKOBAN_RENDERER_H
#define SOKOBAN_RENDERER_H

#include <string>

void init_renderer();
void close_renderer();
void draw_entity(int x, int y, const std::string& text);
void draw_entity_colored(int x, int y, const std::string& text, int color);
void draw_text(int x, int y, const std::string& text);
void draw_text_colored(int x, int y, const std::string& text, int color);
void clear_screen();
void refresh_screen();
int get_input();
void draw_box(int width, int height);
void draw_vim_keys(int y, int x);
void define_colors();

#endif