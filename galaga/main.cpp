#include "game.h"
#include "common/launcher.h"

int main(int argc, char** argv) {
  checkAndLaunchInWindow(argc, argv);
  return galaga_main(argc, argv);
}