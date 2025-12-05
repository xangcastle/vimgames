#include "input.h"
#include <ncurses.h>

namespace engine {

int get_key() { return getch(); }

} // namespace engine
