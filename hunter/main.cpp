#include "game.h"
#include "common/launcher.h"

int main(int argc, char** argv) {
  checkAndLaunchInWindow(argc, argv);
  return hunter_main(argc, argv);
}