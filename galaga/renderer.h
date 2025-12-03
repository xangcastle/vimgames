#ifndef GALAGA_RENDERER_H
#define GALAGA_RENDERER_H

#include <string>

void init_renderer();
void close_renderer();
void draw_entity(int x, int y, const std::string& text);
void draw_entity_colored(int x, int y, const std::string& text, int color);
void draw_text(int x, int y, const std::string& text);
void draw_text_colored(int x, int y, const std::string& text, int color);
void draw_text_centered(int y, const std::string& text);
void draw_text_centered_colored(int y, const std::string& text, int color);
void draw_box(int width, int height);
void clear_screen();
void refresh_screen();
int get_input();

#endif