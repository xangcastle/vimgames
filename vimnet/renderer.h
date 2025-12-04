#ifndef VIMNET_RENDERER_H
#define VIMNET_RENDERER_H

#include <string>
#include <vector>

// Initialize ncurses
void init_renderer();

// Close ncurses
void close_renderer();

// Clear the screen
void clear_screen();

// Refresh the screen
void refresh_screen();

// Get user input (blocking)
int get_input();

// Draw a single character at a specific position
void draw_char(int y, int x, char c);

// Draw text at a specific position
void draw_text(int y, int x, const std::string& text);

// Draw text with color
void draw_text_colored(int y, int x, const std::string& text, int color_pair_idx);

// Draw text centered on a line
void draw_text_centered(int y, const std::string& text);

// Draw text centered with color
void draw_text_centered_colored(int y, const std::string& text, int color_pair_idx);

// Draw a box (border)
void draw_box(int width, int height);

// Draw the game buffer
void draw_buffer(const std::vector<std::string>& buffer, int start_y, int start_x, int cursor_y, int cursor_x, bool in_visual_mode, int visual_start_y, int visual_start_x, int visual_end_y, int visual_end_x);

// Show a welcome screen
void welcome_screen();

// Control cursor visibility
void set_cursor_visibility(bool visible);

// Get screen dimensions
int get_screen_height();
int get_screen_width();

// Constants for color pairs (can be extended)
#define COLOR_PAIR_DEFAULT 1
#define COLOR_PAIR_RED     2
#define COLOR_PAIR_GREEN   3
#define COLOR_PAIR_YELLOW  4
#define COLOR_PAIR_BLUE    5
#define COLOR_PAIR_MAGENTA 6
#define COLOR_PAIR_CYAN    7
#define COLOR_PAIR_WHITE   8
#define COLOR_PAIR_VISUAL_SELECTION 9
#define COLOR_PAIR_CURSOR  10

#endif // VIMNET_RENDERER_H
