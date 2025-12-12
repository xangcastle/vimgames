#include "renderer.h"
#include <raylib.h>
#include <string>
#include <unordered_map>

// Constants for grid to pixel conversion
// Constants for grid to pixel conversion
const int FONT_SIZE = 40;
const int CELL_WIDTH = 28; // Slightly wider for better spacing
const int CELL_HEIGHT = 40;

// Color mapping from ncurses to Raylib
Color GetRaylibColor(int ncurses_color) {
  switch (ncurses_color) {
  case 1:
    return RED;
  case 2:
    return GREEN;
  case 3:
    return YELLOW;
  case 4:
    return BLUE;
  case 5:
    return MAGENTA;
  case 6:
    return SKYBLUE; // Cyan
  case 7:
    return RAYWHITE;
  default:
    return RAYWHITE;
  }
}

// Textures & Fonts
Texture2D texWizard;
Texture2D texGhost;
Texture2D texDemon;
Texture2D texHeart;
Font customFont;

// Audio
Sound sndShoot;
Sound sndHit;
Sound sndDie;
Music musicBg;

// Grid dimensions match game.cpp
const int GRID_WIDTH = 60;
const int GRID_HEIGHT = 25;

void init_renderer() {
  // 60x20 grid roughly
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  // Increase height to accommodate UI lines (Grid + Status + Score + Hints) +
  // Padding
  InitWindow(GRID_WIDTH * CELL_WIDTH + 40, (GRID_HEIGHT + 4) * CELL_HEIGHT + 40,
             "Hunter Vim");
  InitAudioDevice();
  SetTargetFPS(60);

  // Load Font
  // Use default font
  customFont = GetFontDefault();
  SetTextureFilter(customFont.texture,
                   TEXTURE_FILTER_POINT); // Pixel look for default font

  // Load Assets
  Image imgWizard = LoadImage("hunter/assets/wizard.png");
  ImageResize(&imgWizard, 40, 40);
  texWizard = LoadTextureFromImage(imgWizard);
  UnloadImage(imgWizard);

  Image imgGhost = LoadImage("hunter/assets/ghost.png");
  ImageResize(&imgGhost, 40, 40);
  texGhost = LoadTextureFromImage(imgGhost);
  UnloadImage(imgGhost);

  Image imgDemon = LoadImage("hunter/assets/demon.png");
  ImageResize(&imgDemon, 40, 40);
  texDemon = LoadTextureFromImage(imgDemon);
  UnloadImage(imgDemon);

  Image imgHeart = LoadImage("hunter/assets/heart.png");
  ImageResize(&imgHeart, 40, 40);
  texHeart = LoadTextureFromImage(imgHeart);
  UnloadImage(imgHeart);

  // Load Audio
  sndShoot = LoadSound("hunter/assets/shoot.wav");
  sndHit = LoadSound("hunter/assets/hit.wav");
  sndDie = LoadSound("hunter/assets/die.wav");
  musicBg = LoadMusicStream("hunter/assets/music.wav");
  musicBg.looping = true;
  PlayMusicStream(musicBg);
}

void close_renderer() {
  UnloadTexture(texWizard);
  UnloadTexture(texGhost);
  UnloadTexture(texDemon);
  UnloadTexture(texHeart);
  UnloadTexture(texHeart);

  UnloadSound(sndShoot);
  UnloadSound(sndHit);
  UnloadSound(sndDie);
  UnloadMusicStream(musicBg);
  CloseAudioDevice();

  // UnloadFont(customFont); // Do not unload default font
  CloseWindow();
}

void play_sound(const std::string &sound) {
  if (sound == "shoot")
    PlaySound(sndShoot);
  else if (sound == "hit")
    PlaySound(sndHit);
  else if (sound == "die")
    PlaySound(sndDie);
}

void draw_entity(int x, int y, const std::string &text) {
  int screenW = GetScreenWidth();
  int screenH = GetScreenHeight();
  int offX = (screenW - (GRID_WIDTH * CELL_WIDTH)) / 2;
  int offY = (screenH - (GRID_HEIGHT * CELL_HEIGHT)) / 2;
  if (offX < 0)
    offX = 0;
  if (offY < 0)
    offY = 0;

  Vector2 pos = {(float)(offX + x * CELL_WIDTH),
                 (float)(offY + y * CELL_HEIGHT)};
  DrawTextEx(customFont, text.c_str(), pos, (float)FONT_SIZE, 2, RAYWHITE);
}

// Helper to check for emoji presence
bool Contains(const std::string &text, const std::string &substr) {
  return text.find(substr) != std::string::npos;
}

void DrawPlayer(int pixelX, int pixelY) {
  DrawTexture(texWizard, pixelX, pixelY, WHITE);
}

void DrawGhost(int pixelX, int pixelY, Color color) {
  DrawTexture(texGhost, pixelX, pixelY, color);
}

void DrawDemon(int pixelX, int pixelY) {
  DrawTexture(texDemon, pixelX, pixelY, WHITE);
}

void DrawHeart(int pixelX, int pixelY) {
  DrawTexture(texHeart, pixelX, pixelY, WHITE);
}

void draw_entity_colored(int x, int y, const std::string &text, int color) {
  int screenW = GetScreenWidth();
  int screenH = GetScreenHeight();
  int offX = (screenW - (GRID_WIDTH * CELL_WIDTH)) / 2;
  int offY = (screenH - (GRID_HEIGHT * CELL_HEIGHT)) / 2;

  // Clamp offsets to 0 if screen is smaller than grid
  if (offX < 0)
    offX = 0;
  if (offY < 0)
    offY = 0;

  int px = offX + x * CELL_WIDTH;
  int py = offY + y * CELL_HEIGHT;
  Color rayColor = GetRaylibColor(color);

  if (Contains(text, "🧙")) {
    DrawPlayer(px, py);
  } else if (Contains(text, "👻")) {
    DrawGhost(px, py, rayColor);
    // Draw char overlay
    std::string label = text;
    size_t emojiPos = label.find("👻");
    if (emojiPos != std::string::npos) {
      char trigger = label.back();
      if (isalnum(trigger)) {
        std::string s(1, trigger);
        // Position to the right of the sprite (40px width)
        Vector2 pos = {(float)(px + 40), (float)(py)};
        // Use full font size
        DrawTextEx(customFont, s.c_str(), pos, (float)FONT_SIZE, 2, WHITE);
      }
    }
  } else if (Contains(text, "👹")) {
    DrawDemon(px, py);
    std::string label = text;
    size_t emojiPos = label.find("👹");
    if (emojiPos != std::string::npos) {
      std::string word = label.substr(emojiPos + 4);
      // Position to the right of the sprite
      Vector2 pos = {(float)(px + 40), (float)(py)};
      DrawTextEx(customFont, word.c_str(), pos, (float)FONT_SIZE, 2, WHITE);
    }
  } else if (text.find("[[H]]") != std::string::npos) {
    // Draw Heart Sprite for lives or pickups
    DrawHeart(px, py);
  } else {
    Vector2 pos = {(float)px, (float)py};
    DrawTextEx(customFont, text.c_str(), pos, (float)FONT_SIZE, 2, rayColor);
  }
}

// Helper to check prefix
bool starts_with(const std::string &s, const std::string &prefix) {
  return s.rfind(prefix, 0) == 0;
}

void draw_text(int x, int y, const std::string &text) {
  int screenW = GetScreenWidth();
  int screenH = GetScreenHeight();
  int offX = (screenW - (GRID_WIDTH * CELL_WIDTH)) / 2;
  int offY = (screenH - (GRID_HEIGHT * CELL_HEIGHT)) / 2;
  if (offX < 0)
    offX = 0;
  if (offY < 0)
    offY = 0;

  int px = offX + x * CELL_WIDTH;
  int py = offY + y * CELL_HEIGHT;

  // Check for hearts (Lives) using ASCII sentinel
  if (text.find("[[H]]") != std::string::npos) {
    // Simple loop to draw heart for every occurrence
    int currentX = px;
    size_t pos = 0;
    while ((pos = text.find("[[H]]", pos)) != std::string::npos) {
      DrawHeart(currentX, py);
      currentX += 40; // Next sprite position
      pos += 5;       // Length of [[H]]
    }
    return;
  }

  Vector2 pos = {(float)px,
                 (float)(py + 10)}; // center text vertically in cell approx
  DrawTextEx(customFont, text.c_str(), pos, (float)FONT_SIZE, 2, RAYWHITE);
}

void draw_text_colored(int x, int y, const std::string &text, int color) {
  int screenW = GetScreenWidth();
  int screenH = GetScreenHeight();
  int offX = (screenW - (GRID_WIDTH * CELL_WIDTH)) / 2;
  int offY = (screenH - (GRID_HEIGHT * CELL_HEIGHT)) / 2;
  if (offX < 0)
    offX = 0;
  if (offY < 0)
    offY = 0;

  Vector2 pos = {(float)(offX + x * CELL_WIDTH),
                 (float)(offY + y * CELL_HEIGHT)};
  DrawTextEx(customFont, text.c_str(), pos, (float)FONT_SIZE, 2,
             GetRaylibColor(color));
}

void draw_text_centered(int y, const std::string &text) {
  int screenW = GetScreenWidth();
  int screenH = GetScreenHeight();
  // Center vertically based on grid, but horizontally based on screen
  int offY = (screenH - (GRID_HEIGHT * CELL_HEIGHT)) / 2;
  if (offY < 0)
    offY = 0;

  Vector2 textSize =
      MeasureTextEx(customFont, text.c_str(), (float)FONT_SIZE, 2);
  Vector2 pos = {(float)(screenW - textSize.x) / 2,
                 (float)(offY + y * CELL_HEIGHT)};
  DrawTextEx(customFont, text.c_str(), pos, (float)FONT_SIZE, 2, RAYWHITE);
}

void draw_text_centered_colored(int y, const std::string &text, int color) {
  int screenW = GetScreenWidth();
  int screenH = GetScreenHeight();
  int offY = (screenH - (GRID_HEIGHT * CELL_HEIGHT)) / 2;
  if (offY < 0)
    offY = 0;

  Vector2 textSize =
      MeasureTextEx(customFont, text.c_str(), (float)FONT_SIZE, 2);
  Vector2 pos = {(float)(screenW - textSize.x) / 2,
                 (float)(offY + y * CELL_HEIGHT)};
  DrawTextEx(customFont, text.c_str(), pos, (float)FONT_SIZE, 2,
             GetRaylibColor(color));
}

void draw_box(int width, int height) {
  // Draw logical box
  DrawRectangleLines(10, 10, width * CELL_WIDTH, height * CELL_HEIGHT, GRAY);
}

void clear_screen() {
  if (!IsWindowReady())
    return;
  BeginDrawing();
  ClearBackground(BLACK);
}

void refresh_screen() {
  if (!IsWindowReady())
    return;
  UpdateMusicStream(musicBg);
  EndDrawing();
}

int get_input() {
  // Check key presses and return their ASCII equivalent if possible

  // Priority to special keys
  if (IsKeyPressed(KEY_Q))
    return 'q'; // Quit
  if (IsKeyPressed(KEY_ESCAPE))
    return 27; // ESC
  if (IsKeyPressed(KEY_ENTER))
    return '\n'; // Enter
  if (IsKeyPressed(KEY_BACKSPACE))
    return 127; // Backspace
  if (IsKeyPressed(KEY_SLASH))
    return '/'; // Command mode

  // Movement keys
  if (IsKeyPressed(KEY_H))
    return 'h';
  if (IsKeyPressed(KEY_J))
    return 'j';
  if (IsKeyPressed(KEY_K))
    return 'k';
  if (IsKeyPressed(KEY_L))
    return 'l';
  if (IsKeyPressed(KEY_F))
    return 'f';

  int key = GetCharPressed();
  if (key > 0)
    return key;

  return -1; // ERR
}

bool renderer_should_close() { return WindowShouldClose(); }
