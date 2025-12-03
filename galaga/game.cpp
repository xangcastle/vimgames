#include "game.h"
#include "globals.h"
#include "renderer.h"
#include <ncurses.h>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>

struct Bullet {
    int x, y;
    bool active;
};

struct Enemy {
    int x, y;
    bool active;
};

std::vector<Bullet> bullets;
std::vector<Enemy> enemies;
int tick_counter = 0;

void spawn_enemies() {
    enemies.clear();
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 3; ++j) {
            // Center the enemies
            // i * 4 allows some spacing. Width is 40. 6*4 = 24. Centered around 20.
            // 20 - 12 + i*4 = 8 + i*4
            enemies.push_back({8 + i * 4, 2 + j * 2, true});
        }
    }
}

void update_bullets() {
    for (auto& b : bullets) {
        if (b.active) {
            b.y--;
            if (b.y < 1) b.active = false; // Top border is at 0
        }
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), 
                  [](const Bullet& b){ return !b.active; }), bullets.end());
}

void update_enemies() {
    tick_counter++;
    if (tick_counter % 5 == 0) { // Move enemies faster (every 5 ticks)
        for (auto& e : enemies) {
            if (e.active) {
                e.y++; 
                if (e.y >= GAME_HEIGHT) {
                    GAME_OVER = true;
                }
            }
        }
    }
}

void check_collisions() {
    for (auto& b : bullets) {
        if (!b.active) continue;
        for (auto& e : enemies) {
            if (!e.active) continue;
            // Simple collision
            if (b.x == e.x && b.y == e.y) {
                b.active = false;
                e.active = false;
                SCORE += 100;
            }
        }
    }
    
    // Check collision with player
    for (const auto& e : enemies) {
        if (e.active && e.x == PLAYER_X && e.y == PLAYER_Y) {
            GAME_OVER = true;
        }
    }
}

void draw() {
    clear_screen();
    
    // Draw borders
    std::string horizontal_border = "";
    for (int i = 0; i <= GAME_WIDTH; ++i) horizontal_border += "__"; 
    
    draw_text(0, 0, horizontal_border);
    draw_text(0, GAME_HEIGHT + 1, horizontal_border);

    // Draw Player
    draw_entity(PLAYER_X, PLAYER_Y, "🚀");

    // Draw Enemies
    for (const auto& e : enemies) {
        if (e.active) draw_entity(e.x, e.y, "👾");
    }

    // Draw Bullets
    for (const auto& b : bullets) {
        if (b.active) draw_entity(b.x, b.y, "🔥"); // Fire emoji for bullet
    }

    // Draw UI
    draw_text(0, GAME_HEIGHT + 3, "Score: " + std::to_string(SCORE));
    draw_text(20, GAME_HEIGHT + 3, "Lives: " + std::to_string(LIVES));
    draw_text(2, GAME_HEIGHT + 4, "h:Left j:Down k:Up l:Right Space:Shoot q:Quit");

    if (GAME_OVER) {
         draw_text(GAME_WIDTH, GAME_HEIGHT / 2, "GAME OVER");
    }

    refresh_screen();
}

int galaga_main(int argc, char** argv) {
    init_renderer();
    spawn_enemies();
    PLAYER_X = GAME_WIDTH / 2;
    PLAYER_Y = GAME_HEIGHT; // Player at bottom
    
    GAME_OVER = false;
    SCORE = 0;

    while (!GAME_OVER) {
        int ch = getch();
        if (ch == 'q') break;
        
        // Movement
        if (ch == 'h') {
            if (PLAYER_X > 1) PLAYER_X--;
        }
        else if (ch == 'l') {
            if (PLAYER_X < GAME_WIDTH - 1) PLAYER_X++;
        }
        else if (ch == 'j') {
            if (PLAYER_Y < GAME_HEIGHT) PLAYER_Y++;
        }
        else if (ch == 'k') {
            if (PLAYER_Y > 1) PLAYER_Y--;
        }
        // Action
        else if (ch == ' ') {
            bullets.push_back({PLAYER_X, PLAYER_Y - 1, true});
        }

        update_bullets();
        update_enemies();
        check_collisions();
        draw();
        
        // Check win condition
        bool all_dead = true;
        for(const auto& e : enemies) {
            if(e.active) {
                all_dead = false;
                break;
            }
        }
        if(all_dead) {
             spawn_enemies();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    if(GAME_OVER) {
        draw();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    close_renderer();
    return 0;
}