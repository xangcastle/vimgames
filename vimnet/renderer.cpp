#include "renderer.h"
#include <ncurses.h>
#include <string>
#include <vector>
#include <algorithm> // For std::min

// Global window pointer for ncurses operations
static WINDOW *main_window;

void init_renderer() {
    main_window = initscr(); // Initialize the curses screen
    cbreak();                // Line buffering disabled, pass everything to us
    noecho();                // Don't echo input characters
    keypad(stdscr, TRUE);    // Enable special keys (like F1, arrow keys)
    curs_set(0);             // Hide the cursor
    
    if (has_colors()) {
        start_color();
        // Define some color pairs
        init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_PAIR_RED, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_PAIR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_PAIR_BLUE, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_PAIR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_PAIR_CYAN, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_PAIR_WHITE, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_PAIR_VISUAL_SELECTION, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOR_PAIR_CURSOR, COLOR_WHITE, COLOR_CYAN);
    }
}

void close_renderer() {
    endwin(); // End curses mode
}

void clear_screen() {
    clear();
}

void refresh_screen() {
    refresh();
}

int get_input() {
    return getch();
}

void draw_char(int y, int x, char c) {
    mvaddch(y, x, c);
}

void draw_text(int y, int x, const std::string& text) {
    mvprintw(y, x, "%s", text.c_str());
}

void draw_text_colored(int y, int x, const std::string& text, int color_pair_idx) {
    attron(COLOR_PAIR(color_pair_idx));
    mvprintw(y, x, "%s", text.c_str());
    attroff(COLOR_PAIR(color_pair_idx));
}

void draw_text_centered(int y, const std::string& text) {
    int max_x = get_screen_width();
    int start_x = (max_x - text.length()) / 2;
    mvprintw(y, start_x, "%s", text.c_str());
}

void draw_text_centered_colored(int y, const std::string& text, int color_pair_idx) {
    attron(COLOR_PAIR(color_pair_idx));
    draw_text_centered(y, text);
    attroff(COLOR_PAIR(color_pair_idx));
}

void draw_box(int width, int height) {
    // Top border
    draw_text_colored(0, 0, "╔", COLOR_PAIR_CYAN);
    for (int x = 1; x < width - 1; ++x) {
        draw_text_colored(0, x, "═", COLOR_PAIR_CYAN);
    }
    draw_text_colored(0, width - 1, "╗", COLOR_PAIR_CYAN);

    // Side borders and fill
    for (int y = 1; y < height - 1; ++y) {
        draw_text_colored(y, 0, "║", COLOR_PAIR_CYAN);
        draw_text_colored(y, width - 1, "║", COLOR_PAIR_CYAN);
    }

    // Bottom border
    draw_text_colored(height - 1, 0, "╚", COLOR_PAIR_CYAN);
    for (int x = 1; x < width - 1; ++x) {
        draw_text_colored(height - 1, x, "═", COLOR_PAIR_CYAN);
    }
    draw_text_colored(height - 1, width - 1, "╝", COLOR_PAIR_CYAN);
}

void draw_buffer(const std::vector<std::string>& buffer, int start_y, int start_x, int cursor_y, int cursor_x, bool in_visual_mode, int visual_start_y, int visual_start_x, int visual_end_y, int visual_end_x) {
    int max_draw_y = get_screen_height() - 2; // Reserve 1 line for bottom border and 1 for status line
    int max_draw_x = get_screen_width() - 1; // Reserve 1 for right border

    for (size_t i = 0; i < buffer.size(); ++i) {
        // Only draw lines that fit within the allowed vertical space
        if (start_y + i >= max_draw_y) break;
        if (start_y + i < 1) continue; // Don't draw over top border

        const std::string& line = buffer[i];
        
        // Draw the line
        for (size_t j = 0; j < line.length(); ++j) {
            // Only draw characters that fit within the allowed horizontal space
            if (start_x + j >= max_draw_x) break;
            if (start_x + j < 1) continue; // Don't draw over left border

            bool is_in_selection = false;
            if (in_visual_mode) {
                // Determine the correct order of start and end points for comparison
                int sel_start_y = std::min(visual_start_y, visual_end_y);
                int sel_start_x = (visual_start_y < visual_end_y || (visual_start_y == visual_end_y && visual_start_x < visual_end_x)) ? visual_start_x : visual_end_x;
                int sel_end_y = std::max(visual_start_y, visual_end_y);
                int sel_end_x = (visual_start_y > visual_end_y || (visual_start_y == visual_end_y && visual_start_x > visual_end_x)) ? visual_start_x : visual_end_x;

                if (i > sel_start_y && i < sel_end_y) {
                    is_in_selection = true;
                } else if (i == sel_start_y && i == sel_end_y) {
                    if (j >= sel_start_x && j < sel_end_x) {
                        is_in_selection = true;
                    }
                } else if (i == sel_start_y) {
                    if (j >= sel_start_x) {
                        is_in_selection = true;
                    }
                } else if (i == sel_end_y) {
                    if (j < sel_end_x) {
                        is_in_selection = true;
                    }
                }
            }
            
            bool is_cursor = (i == cursor_y && j == cursor_x);
            
            if (is_in_selection) {
                attron(COLOR_PAIR(COLOR_PAIR_VISUAL_SELECTION));
            } else if (is_cursor) {
                attron(COLOR_PAIR(COLOR_PAIR_CURSOR));
            }

            mvaddch(start_y + i, start_x + j, line[j]);
            
            if (is_in_selection || is_cursor) {
                attroff(COLOR_PAIR(is_in_selection ? COLOR_PAIR_VISUAL_SELECTION : COLOR_PAIR_CURSOR));
            }
        }

        // Handle drawing cursor on an empty line or at the end of a line
        if (i == cursor_y && (line.empty() || cursor_x == line.length())) {
             attron(COLOR_PAIR(COLOR_PAIR_CURSOR));
             mvaddch(start_y + i, start_x + cursor_x, ' ');
             attroff(COLOR_PAIR(COLOR_PAIR_CURSOR));
        }

        // Place the ncurses cursor at the game's cursor position (for blinking)
        if (i == cursor_y) {
            // Ensure ncurses cursor doesn't go beyond end of screen or current line length
            int actual_cursor_x = std::min(start_x + cursor_x, max_draw_x - 1);
            int actual_cursor_y = start_y + cursor_y;
            if (actual_cursor_y < max_draw_y) {
                move(actual_cursor_y, actual_cursor_x);
            }
        }
    }
}

void welcome_screen() {
    clear_screen();
    int y = get_screen_height() / 2 - 8; // Adjust starting Y to fit new text

    draw_text_centered_colored(y++, "VIMNET", COLOR_PAIR_CYAN);
    y++;
    draw_text_centered(y++, "The last game of the saga to learn to use vim");
    y++;
    draw_text_centered_colored(y++, "--- INSTRUCTIONS ---", COLOR_PAIR_YELLOW);
    draw_text_centered(y++, "Move cursor with: h (left), j (down), k (up), l (right)");
    draw_text_centered(y++, "Switch to INSERT mode: i");
    draw_text_centered(y++, "Exit INSERT/VISUAL/COMMAND mode: ESC");
    draw_text_centered(y++, "Delete character: x");
    draw_text_centered(y++, "Delete word: dw");
    draw_text_centered(y++, "Undo last change: u");
    draw_text_centered(y++, "Search: / (then type query and ENTER)");
    draw_text_centered(y++, "Next search result: n, Previous search result: N");
    draw_text_centered(y++, "Go to line: : (then type number and ENTER)");
    draw_text_centered(y++, "Quit game: q");
    y++;
    draw_text_centered_colored(y++, "Press any key to start", COLOR_PAIR_GREEN);

    refresh_screen();
    get_input();
}

void set_cursor_visibility(bool visible) {
    curs_set(visible ? 1 : 0);
}

int get_screen_height() {
    return LINES;
}

int get_screen_width() {
    return COLS;
}
