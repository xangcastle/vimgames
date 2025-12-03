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
}

void close_renderer() {
    endwin();
}

void draw_entity(int x, int y, const std::string& text) {
    // x * 2 for wider grid
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

int get_input() {
    return getch();
}