#include "engine/engine.h"
#include <string>

int main() {
  engine::init();

  int x = 10;
  int y = 10;
  int dx = 1;
  int dy = 1;

  engine::run([&]() {
    engine::clear_screen();

    engine::draw_string(x, y, "Hello Engine! (Press 'q' to quit)");

    x += dx;
    y += dy;

    if (x <= 0 || x >= 50)
      dx = -dx;
    if (y <= 0 || y >= 20)
      dy = -dy;

    engine::refresh_screen();

    int key = engine::get_key();
    if (key == 'q') {
      return false;
    }

    return true;
  });

  engine::shutdown();
  return 0;
}
