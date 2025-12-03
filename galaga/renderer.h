#ifndef GALAGA_RENDERER_H
#define GALAGA_RENDERER_H

#include <string>

void init_renderer();
void close_renderer();
void draw_entity(int x, int y, const std::string& text);
void draw_text(int x, int y, const std::string& text);
void clear_screen();
void refresh_screen();

#endif