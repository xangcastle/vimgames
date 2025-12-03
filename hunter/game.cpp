#include "game.h"
#include "renderer.h"
#include <ncurses.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <thread>
#include <chrono>

// Game Constants
const int WIDTH = 60;
const int HEIGHT = 20;
const int PLAYER_X = WIDTH / 2;
const int PLAYER_Y = HEIGHT - 2;

struct Enemy {
    int x, y;
    char trigger_char;
    std::string trigger_word;
    bool is_word; // true if it requires '/' command
    bool active;
};

std::vector<Enemy> enemies;
int score = 0;
int lives = 3;
bool game_over = false;
std::string command_buffer = "";
bool command_mode = false; // true if typing a command like /word
bool find_mode = false; // true if waiting for char after 'f'

// Words for boss enemies
std::vector<std::string> dictionary = {"vim", "code", "exit", "quit", "save", "edit", "bash", "grep", "sed", "awk", "find", "make"};

void spawn_enemy() {
    Enemy e;
    e.x = rand() % (WIDTH - 4) + 2;
    e.y = 1;
    e.active = true;
    
    if (rand() % 5 == 0) {
        // Boss enemy (Word)
        e.is_word = true;
        e.trigger_word = dictionary[rand() % dictionary.size()];
        e.trigger_char = 0;
    } else {
        // Normal enemy (Char)
        e.is_word = false;
        e.trigger_char = 'a' + (rand() % 26);
        e.trigger_word = "";
    }
    enemies.push_back(e);
}

void update_game() {
    static int tick = 0;
    tick++;
    
    // Spawn enemies occasionally
    if (tick % 40 == 0) {
        spawn_enemy();
    }
    
    // Move enemies
    if (tick % 10 == 0) {
        for (auto& e : enemies) {
            if (e.active) {
                e.y++;
                if (e.y >= HEIGHT - 1) {
                    e.active = false;
                    lives--;
                    if (lives <= 0) game_over = true;
                }
            }
        }
    }
}

void handle_input(int ch) {
    if (ch == ERR) return;
    
    if (command_mode) {
        // Typing a command (search)
        if (ch == '\n' || ch == 10 || ch == 13) { // Enter
            // Check matches
            for (auto& e : enemies) {
                if (e.active && e.is_word && e.trigger_word == command_buffer) {
                    e.active = false;
                    score += 50;
                }
            }
            command_mode = false;
            command_buffer = "";
        } else if (ch == 27) { // ESC
            command_mode = false;
            command_buffer = "";
        } else if (ch == 127 || ch == '\b' || ch == KEY_BACKSPACE) {
             if (!command_buffer.empty()) command_buffer.pop_back();
        } else {
            command_buffer += (char)ch;
        }
    } else if (find_mode) {
        // Waiting for char after 'f'
        for (auto& e : enemies) {
            if (e.active && !e.is_word && e.trigger_char == ch) {
                e.active = false;
                score += 10;
                // Only kill the closest one? Or all? Let's kill one for balance or all?
                // Vim 'f' jumps to the *next* occurrence. Let's kill the closest (lowest y).
                // Actually, simple implementation: kill all matching on screen is satisfying.
                // But to be precise, let's kill the one closest to player (highest y).
                break; // Kill only one
            }
        }
        find_mode = false;
    } else {
        // Normal mode
        if (ch == 'q') {
            game_over = true;
        } else if (ch == 'f') {
            find_mode = true;
        } else if (ch == '/') {
            command_mode = true;
            command_buffer = ""; // Reset buffer, prompt will show '/'
        }
    }
}

void draw_game() {
    clear_screen();
    
    // Borders
    for (int i = 0; i < WIDTH; ++i) draw_text(i, 0, "-");
    for (int i = 0; i < WIDTH; ++i) draw_text(i, HEIGHT, "-");
    
    // Player
    draw_entity(PLAYER_X, PLAYER_Y, "🎯");
    
    // Enemies
    for (const auto& e : enemies) {
        if (!e.active) continue;
        if (e.is_word) {
            draw_entity(e.x, e.y, "👹 " + e.trigger_word);
        } else {
            std::string s = "👻 ";
            s += e.trigger_char;
            draw_entity(e.x, e.y, s);
        }
    }
    
    // UI
    draw_text(2, HEIGHT + 2, "Score: " + std::to_string(score) + "  Lives: " + std::to_string(lives));
    
    std::string help = "f<char>: Jump/Kill  /: Search/Kill Word  q: Quit";
    draw_text(2, HEIGHT + 3, help);

    // Status Line
    if (command_mode) {
        draw_text(0, HEIGHT + 1, "/" + command_buffer);
    } else if (find_mode) {
        draw_text(0, HEIGHT + 1, "f_");
    }
    
    refresh_screen();
}

int hunter_main(int argc, char** argv) {
    srand(time(0));
    init_renderer();
    
    while (!game_over) {
        int ch = get_input();
        handle_input(ch);
        update_game();
        draw_game();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    close_renderer();
    return 0;
}