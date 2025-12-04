#include <iostream>
#include "game.h"
#include "common/launcher.h"
#include "renderer.h" // Include the renderer header

int main(int argc, char** argv) {
    checkAndLaunchInWindow(argc, argv);
    init_renderer(); // Initialize ncurses
    
    welcome_screen();

    VimNetGame game;
    game.run();

    close_renderer(); // Close ncurses
    return 0;
}
