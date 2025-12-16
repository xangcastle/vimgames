#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include <string>

enum EntityType {
  ENTITY_EMPTY = 0,
  ENTITY_WALL,
  ENTITY_CRATE,
  ENTITY_TARGET,
  ENTITY_FILLED_TARGET,
  ENTITY_PLAYER
};

void init_renderer();
void close_renderer();

void clear_screen();
void refresh_screen();

int get_input();
bool is_key_pressed(int key);
bool window_should_close();

void draw_entity(int x, int y, EntityType type);
void draw_text(int x, int y, const char *text, int fontSize, Color color);
void draw_text_centered(int y, const char *text, int fontSize, Color color);

#endif