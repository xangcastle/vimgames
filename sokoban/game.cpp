#include "game.h"
#include "renderer.h"
#include <vector>
#include <string>
#include <stack>
#include <cstdlib>

const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 10;

enum Tile {
    EMPTY = 0,
    WALL = 1,
    BLOCK = 2,
    TARGET = 3,
    FILLED_TARGET = 4
};

struct State {
    Tile map[MAP_HEIGHT][MAP_WIDTH];
    int px, py;
    bool holding_block;
};

State current_state;
std::stack<State> history;

// Simple level layout
// # = Wall, . = Empty, B = Block, T = Target
std::string LEVEL_LAYOUT[MAP_HEIGHT] = {
    "###############",
    "#.....#.......#",
    "#..B..#...T...#",
    "#.....#.......#",
    "###.#####.#####",
    "#.......#.....#",
    "#...T...#..B..#",
    "#.......#.....#",
    "#.............#",
    "###############"
};

void init_game() {
    current_state.px = 1; 
    current_state.py = 1;
    current_state.holding_block = false;

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            char c = LEVEL_LAYOUT[y][x];
            if (c == '#') current_state.map[y][x] = WALL;
            else if (c == 'B') current_state.map[y][x] = BLOCK;
            else if (c == 'T') current_state.map[y][x] = TARGET;
            else current_state.map[y][x] = EMPTY;
        }
    }
    history.push(current_state);
}

void save_state() {
    history.push(current_state);
}

void undo() {
    if (history.size() > 1) {
        history.pop();
        current_state = history.top();
    }
}

bool check_win() {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (current_state.map[y][x] == TARGET) return false;
            // If target is covered by block, it becomes FILLED_TARGET
            // If we have any empty TARGET, not won.
        }
    }
    // Also check if we are holding a block needed? No, win if all targets filled.
    // Wait, if map[y][x] is TARGET, it means it's empty target.
    // If it is FILLED_TARGET, it is filled.
    return true;
}

void update_input(int ch) {
    int dx = 0, dy = 0;

    if (ch == 'q') exit(0);
    if (ch == 'u') {
        undo();
        return;
    }
    
    // Movement
    if (ch == 'h') dx = -1;
    else if (ch == 'l') dx = 1;
    else if (ch == 'j') dy = 1;
    else if (ch == 'k') dy = -1;
    
    // Vim Actions
    else if (ch == 'x') {
        // Cut/Delete block under player
        if (!current_state.holding_block) {
            Tile t = current_state.map[current_state.py][current_state.px];
            if (t == BLOCK || t == FILLED_TARGET) {
                save_state();
                current_state.holding_block = true;
                if (t == FILLED_TARGET) current_state.map[current_state.py][current_state.px] = TARGET;
                else current_state.map[current_state.py][current_state.px] = EMPTY;
            }
        }
    }
    else if (ch == 'p') {
        // Paste/Put block
        if (current_state.holding_block) {
            Tile t = current_state.map[current_state.py][current_state.px];
            if (t == EMPTY || t == TARGET) {
                save_state();
                current_state.holding_block = false;
                if (t == TARGET) current_state.map[current_state.py][current_state.px] = FILLED_TARGET;
                else current_state.map[current_state.py][current_state.px] = BLOCK;
            }
        }
    }

    // Apply movement
    if (dx != 0 || dy != 0) {
        int nx = current_state.px + dx;
        int ny = current_state.py + dy;
        
        // Check bounds and walls
        if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT) {
            if (current_state.map[ny][nx] != WALL) {
                current_state.px = nx;
                current_state.py = ny;
            }
        }
    }
}

void draw_game() {
    clear_screen();
    
    // Draw Map
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Tile t = current_state.map[y][x];
            std::string s = "  ";
            if (t == WALL) s = "🧱";
            else if (t == BLOCK) s = "📦";
            else if (t == TARGET) s = "⭕";
            else if (t == FILLED_TARGET) s = "✅";
            
            // Draw player on top?
            if (x == current_state.px && y == current_state.py) {
                 s = "🧙"; 
            }
            
            draw_entity(x, y, s);
        }
    }

    // HUD
    draw_text(0, MAP_HEIGHT + 1, "Held: " + std::string(current_state.holding_block ? "📦" : "None"));
    draw_text(0, MAP_HEIGHT + 2, "h,j,k,l: Move  x: Cut(Pick)  p: Paste(Drop)  u: Undo  q: Quit");
    
    if (check_win()) {
        draw_text(MAP_WIDTH, MAP_HEIGHT / 2, "VICTORY!");
    }

    refresh_screen();
}

int sokoban_main(int argc, char** argv) {
    init_renderer();
    init_game();

    while (true) {
        draw_game();
        int ch = get_input();
        update_input(ch);
    }

    close_renderer();
    return 0;
}