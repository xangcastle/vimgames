#include "renderer.h"
#include "raylib.h"

// Assets
Texture2D texWall;
Texture2D texCrate;
Texture2D texTarget;
Texture2D texFilledTarget;
Texture2D texPlayer;

const int CELL_SIZE = 40;
const int OFFSET_X = 20;
const int OFFSET_Y = 20;

void init_renderer() {
  InitWindow(800, 600, "Sokoban Vim");
  SetTargetFPS(60);

  texWall = LoadTexture("sokoban/assets/wall.png");
  texCrate = LoadTexture("sokoban/assets/crate.png");
  texTarget = LoadTexture("sokoban/assets/target.png");
  texFilledTarget = LoadTexture("sokoban/assets/filled_target.png");
  texPlayer = LoadTexture("sokoban/assets/player.png");

  // Pixel perfect
  SetTextureFilter(texWall, TEXTURE_FILTER_POINT);
  SetTextureFilter(texCrate, TEXTURE_FILTER_POINT);
  SetTextureFilter(texTarget, TEXTURE_FILTER_POINT);
  SetTextureFilter(texPlayer, TEXTURE_FILTER_POINT);
}

void close_renderer() {
  UnloadTexture(texWall);
  UnloadTexture(texCrate);
  UnloadTexture(texTarget);
  UnloadTexture(texFilledTarget);
  UnloadTexture(texPlayer);
  CloseWindow();
}

void clear_screen() {
  BeginDrawing();
  ClearBackground(BLACK);
}

void refresh_screen() { EndDrawing(); }

int get_input() { return GetCharPressed(); }

bool is_key_pressed(int key) { return IsKeyPressed(key); }

bool window_should_close() { return WindowShouldClose(); }

void draw_texture(int x, int y, const Texture2D &tex) {
  DrawTexture(tex, x * CELL_SIZE + OFFSET_X, y * CELL_SIZE + OFFSET_Y, WHITE);
}

void draw_entity(int x, int y, EntityType type) {
  switch (type) {
  case ENTITY_WALL:
    draw_texture(x, y, texWall);
    break;
  case ENTITY_CRATE:
    draw_texture(x, y, texCrate);
    break;
  case ENTITY_TARGET:
    draw_texture(x, y, texTarget);
    break;
  case ENTITY_FILLED_TARGET:
    draw_texture(x, y, texFilledTarget);
    break;
  case ENTITY_PLAYER:
    draw_texture(x, y, texPlayer);
    break;
  default:
    break;
  }
}

void draw_text(int x, int y, const char *text, int fontSize, Color color) {
  DrawText(text, x, y, fontSize, color);
}

void draw_text_centered(int y, const char *text, int fontSize, Color color) {
  int w = MeasureText(text, fontSize);
  DrawText(text, 400 - w / 2, y, fontSize, color);
}