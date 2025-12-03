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
    timeout(50); // Non-blocking getch with 50ms delay

    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
}

void close_renderer()
{
    endwin();
}

void draw_entity(int x, int y, const std::string& text)
{
    // x is logical coordinate, we multiply by 2 for terminal width because we want spacious grid
    // and emojis are often double width
    mvprintw(y, x * 2, "%s", text.c_str());
}

void draw_entity_colored(int x, int y, const std::string& text, int color)
{
    attron(COLOR_PAIR(color));
    mvprintw(y, x * 2, "%s", text.c_str());
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

void draw_text_centered(int y, const std::string& text)
{
    int cols = getmaxx(stdscr);
    int x = (cols - text.length()) / 2;
    mvprintw(y, x, "%s", text.c_str());
}

void draw_text_centered_colored(int y, const std::string& text, int color)
{
    int cols = getmaxx(stdscr);
    int x = (cols - text.length()) / 2;
    attron(COLOR_PAIR(color));
    mvprintw(y, x, "%s", text.c_str());
    attroff(COLOR_PAIR(color));
}

void draw_box(int width, int height)
{
    // Use width * 2 because of the spacing logic in draw_entity
    int real_width = width * 2;

    // Top border
    mvaddch(0, 0, ACS_ULCORNER);
    for (int i = 1; i < real_width - 1; i++) mvaddch(0, i, ACS_HLINE);
    mvaddch(0, real_width - 1, ACS_URCORNER);

    // Side borders
    for (int i = 1; i < height - 1; i++)
    {
        mvaddch(i, 0, ACS_VLINE);
        mvaddch(i, real_width - 1, ACS_VLINE);
    }

    // Bottom border
    mvaddch(height - 1, 0, ACS_LLCORNER);
    for (int i = 1; i < real_width - 1; i++) mvaddch(height - 1, i, ACS_HLINE);
    mvaddch(height - 1, real_width - 1, ACS_LRCORNER);
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