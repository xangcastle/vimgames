#include "renderer.h"
#include <ncurses.h>
#include <clocale>

void init_renderer() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(50); // Non-blocking getch
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
}

void close_renderer() {
    endwin();
}

void draw_entity(int x, int y, const std::string& text) {
    mvprintw(y, x, "%s", text.c_str());
}

void draw_text(int x, int y, const std::string& text) {
    mvprintw(y, x, "%s", text.c_str());
}

void draw_text_centered(int y, const std::string& text) {
    int cols = getmaxx(stdscr);
    int x = (cols - text.length()) / 2;
    mvprintw(y, x, "%s", text.c_str());
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