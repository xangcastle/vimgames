#include "renderer.h"
#include "globals.h"
#include <ncurses.h>
#include <clocale>

void init_renderer() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(50); // Non-blocking getch with 50ms delay
}

void close_renderer() {
    endwin();
}

void draw_entity(int x, int y, const std::string& text) {
    // x is logical coordinate, we multiply by 2 for terminal width because we want spacious grid
    // and emojis are often double width
    mvprintw(y, x * 2, "%s", text.c_str());
}

void draw_text(int x, int y, const std::string& text) {
    mvprintw(y, x, "%s", text.c_str());
}

void clear_screen() {
    clear();
}

void refresh_screen() {
    refresh();
}