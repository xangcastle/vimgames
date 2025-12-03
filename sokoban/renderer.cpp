#include "renderer.h"
#include "globals.h"
#include <ncurses.h>
#include <clocale>

void init_renderer()
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    define_colors();
}

void close_renderer()
{
    endwin();
}

void define_colors()
{
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
}

void draw_entity(int x, int y, const std::string& text)
{
    // x * 2 for wider grid, apply offsets
    mvprintw(y + OFFSET_Y, x * 2 + OFFSET_X, "%s", text.c_str());
}

void draw_entity_colored(int x, int y, const std::string& text, int color)
{
    attron(COLOR_PAIR(color));
    mvprintw(y + OFFSET_Y, x * 2 + OFFSET_X, "%s", text.c_str());
    attroff(COLOR_PAIR(color));
}

void draw_text(int x, int y, const std::string& text)
{
    mvprintw(y, x, "%s", text.c_str());
}

void draw_text_colored(int x, int y, const std::string& text, int color)
{
    attron(COLOR_PAIR(color));
    mvprintw(y, x, "%s", text.c_str());
    attroff(COLOR_PAIR(color));
}

void clear_screen()
{
    clear();
}

void refresh_screen()
{
    refresh();
}

int get_input()
{
    return getch();
}

void draw_box(int width, int height)
{
    // Top border
    mvaddch(0, 0, ACS_ULCORNER);
    for (int i = 1; i < width - 1; i++) mvaddch(0, i, ACS_HLINE);
    mvaddch(0, width - 1, ACS_URCORNER);

    // Side borders
    for (int i = 1; i < height - 1; i++)
    {
        mvaddch(i, 0, ACS_VLINE);
        mvaddch(i, width - 1, ACS_VLINE);
    }

    // Bottom border
    mvaddch(height - 1, 0, ACS_LLCORNER);
    for (int i = 1; i < width - 1; i++) mvaddch(height - 1, i, ACS_HLINE);
    mvaddch(height - 1, width - 1, ACS_LRCORNER);
}

void draw_vim_keys(int y, int x)
{
    mvaddstr(y++, x, " .---. .---. .---. .---.");
    mvaddstr(y++, x, " | h | | j | | k | | l |");
    mvaddstr(y++, x, " '---' '---' '---' '---'");
    mvaddstr(y++, x, " Left  Down   Up   Right");
}