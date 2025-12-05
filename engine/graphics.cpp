#define _XOPEN_SOURCE_EXTENDED
#include "graphics.h"
#include <ncurses.h>

namespace engine {

void draw_string(int x, int y, const std::string &text, int color_pair) {
  if (color_pair > 0) {
    attron(COLOR_PAIR(color_pair));
  }
  mvprintw(y, x, "%s", text.c_str());
  if (color_pair > 0) {
    attroff(COLOR_PAIR(color_pair));
  }
}

void draw_box(int x, int y, int width, int height, int color_pair) {
  if (color_pair > 0) {
    attron(COLOR_PAIR(color_pair));
  }

  // Top border
  mvaddch(y, x, ACS_ULCORNER);
  for (int i = 1; i < width - 1; i++)
    mvaddch(y, x + i, ACS_HLINE);
  mvaddch(y, x + width - 1, ACS_URCORNER);

  // Side borders
  for (int i = 1; i < height - 1; i++) {
    mvaddch(y + i, x, ACS_VLINE);
    mvaddch(y + i, x + width - 1, ACS_VLINE);
  }

  // Bottom border
  mvaddch(y + height - 1, x, ACS_LLCORNER);
  for (int i = 1; i < width - 1; i++)
    mvaddch(y + height - 1, x + i, ACS_HLINE);
  mvaddch(y + height - 1, x + width - 1, ACS_LRCORNER);

  if (color_pair > 0) {
    attroff(COLOR_PAIR(color_pair));
  }
}

void clear_screen() { clear(); }

void refresh_screen() { refresh(); }

int get_screen_width() { return getmaxx(stdscr); }

int get_screen_height() { return getmaxy(stdscr); }

void capture_screen(const std::string &filename) {
  FILE *f = fopen(filename.c_str(), "w");
  if (!f)
    return;

  int h = get_screen_height();
  int w = get_screen_width();

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      cchar_t wc;
      // Try to read wide character
      if (mvin_wch(y, x, &wc) != ERR) {
        wchar_t wch[CCHARW_MAX];
        attr_t attrs;
        short color_pair;
        if (getcchar(&wc, wch, &attrs, &color_pair, NULL) == OK) {
          // Convert to UTF-8 and write
          fprintf(f, "%ls", wch);
        } else {
          fputc(' ', f);
        }
      } else {
        fputc(' ', f);
      }
    }
    fputc('\n', f);
  }
  fclose(f);
}

int get_char_at(int x, int y) { return mvinch(y, x) & A_CHARTEXT; }

} // namespace engine
