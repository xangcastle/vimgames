/*
 * Sokoban - Main Game Logic
 * Inspired by PacVim UI/UX
 */

#include "game.h"
#include "renderer.h"
#include "globals.h"
#include <vector>
#include <string>
#include <stack>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unistd.h>

enum Tile
{
    EMPTY = 0,
    WALL = 1,
    BLOCK = 2,
    TARGET = 3,
    FILLED_TARGET = 4
};

struct State
{
    std::vector<std::vector<Tile>> map;
    int px, py;
    bool holding_block;
    int moves;
};

State current_state;
std::stack<State> history;

// Player starting position from map file
int START_X = 1;
int START_Y = 1;

void reset_globals()
{
    LEVEL_MOVES = 0;
    LEVEL_COMPLETE = false;
}

void load_map(const char* filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        // Fallback to simple map
        MAP_WIDTH = 15;
        MAP_HEIGHT = 10;
        return;
    }

    GAME_BOARD.clear();
    std::string line;
    MAP_WIDTH = 0;
    MAP_HEIGHT = 0;

    while (std::getline(in, line))
    {
        // Check for player position line (starts with 'p')
        if (!line.empty() && line[0] == 'p')
        {
            std::string str = line.substr(1);
            size_t space_pos = str.find(' ');
            if (space_pos != std::string::npos)
            {
                START_X = std::stoi(str.substr(0, space_pos));
                START_Y = std::stoi(str.substr(space_pos + 1));
            }
            break;
        }

        GAME_BOARD.push_back(line);
        if (line.length() > MAP_WIDTH)
        {
            MAP_WIDTH = line.length();
        }
        MAP_HEIGHT++;
    }

    in.close();

    // Ensure all lines have same width
    for (auto& row : GAME_BOARD)
    {
        row.resize(MAP_WIDTH, '.');
    }
}

void init_game()
{
    // Load the map for current level
    std::string mapName = MAPS_LOCATION
    "/map";
    std::stringstream ss;
    ss << CURRENT_LEVEL;
    mapName += ss.str() + ".txt";

    load_map(mapName.c_str());

    // Initialize state
    current_state.px = START_X;
    current_state.py = START_Y;
    current_state.holding_block = false;
    current_state.moves = 0;

    // Parse map from GAME_BOARD
    current_state.map.clear();
    current_state.map.resize(MAP_HEIGHT);

    for (int y = 0; y < MAP_HEIGHT; ++y)
    {
        current_state.map[y].resize(MAP_WIDTH);
        for (int x = 0; x < MAP_WIDTH; ++x)
        {
            char c = (y < GAME_BOARD.size() && x < GAME_BOARD[y].size())
                         ? GAME_BOARD[y][x]
                         : '.';

            if (c == '#') current_state.map[y][x] = WALL;
            else if (c == 'B') current_state.map[y][x] = BLOCK;
            else if (c == 'T') current_state.map[y][x] = TARGET;
            else current_state.map[y][x] = EMPTY;
        }
    }

    // Clear history
    while (!history.empty()) history.pop();
    history.push(current_state);

    reset_globals();
}

void save_state()
{
    history.push(current_state);
}

void undo()
{
    if (history.size() > 1)
    {
        history.pop();
        current_state = history.top();
        if (LEVEL_MOVES > 0) LEVEL_MOVES--;
        if (TOTAL_MOVES > 0) TOTAL_MOVES--;
    }
}

bool check_win()
{
    for (int y = 0; y < MAP_HEIGHT; ++y)
    {
        for (int x = 0; x < MAP_WIDTH; ++x)
        {
            if (current_state.map[y][x] == TARGET)
            {
                return false; // Empty target found
            }
        }
    }
    // Check if holding a block that needs placement
    if (current_state.holding_block)
    {
        return false;
    }
    return true;
}

void update_input(int ch)
{
    int dx = 0, dy = 0;
    bool moved = false;

    if (ch == 'q')
    {
        close_renderer();
        exit(0);
    }

    if (ch == 'u')
    {
        undo();
        return;
    }

    // Movement
    if (ch == 'h')
    {
        dx = -1;
        moved = true;
    }
    else if (ch == 'l')
    {
        dx = 1;
        moved = true;
    }
    else if (ch == 'j')
    {
        dy = 1;
        moved = true;
    }
    else if (ch == 'k')
    {
        dy = -1;
        moved = true;
    }

    // Vim Actions
    else if (ch == 'x' || ch == 'd')
    {
        // Cut/Delete block under player
        if (!current_state.holding_block)
        {
            Tile t = current_state.map[current_state.py][current_state.px];
            if (t == BLOCK || t == FILLED_TARGET)
            {
                save_state();
                current_state.holding_block = true;
                if (t == FILLED_TARGET)
                {
                    current_state.map[current_state.py][current_state.px] = TARGET;
                }
                else
                {
                    current_state.map[current_state.py][current_state.px] = EMPTY;
                }
                LEVEL_MOVES++;
                TOTAL_MOVES++;
            }
        }
    }
    else if (ch == 'p')
    {
        // Paste/Put block
        if (current_state.holding_block)
        {
            Tile t = current_state.map[current_state.py][current_state.px];
            if (t == EMPTY || t == TARGET)
            {
                save_state();
                current_state.holding_block = false;
                if (t == TARGET)
                {
                    current_state.map[current_state.py][current_state.px] = FILLED_TARGET;
                }
                else
                {
                    current_state.map[current_state.py][current_state.px] = BLOCK;
                }
                LEVEL_MOVES++;
                TOTAL_MOVES++;
            }
        }
    }

    // Apply movement
    if (moved && (dx != 0 || dy != 0))
    {
        int nx = current_state.px + dx;
        int ny = current_state.py + dy;

        // Check bounds
        if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT)
        {
            // Check walls
            if (current_state.map[ny][nx] != WALL)
            {
                save_state();
                current_state.px = nx;
                current_state.py = ny;
                LEVEL_MOVES++;
                TOTAL_MOVES++;
            }
        }
    }
}

void draw_game_board()
{
    // Draw Map
    for (int y = 0; y < MAP_HEIGHT; ++y)
    {
        for (int x = 0; x < MAP_WIDTH; ++x)
        {
            Tile t = current_state.map[y][x];
            std::string s = "  ";
            int color = 7; // White default

            if (t == WALL)
            {
                s = "🧱";
                color = 3; // Yellow
            }
            else if (t == BLOCK)
            {
                s = "📦";
                color = 6; // Cyan
            }
            else if (t == TARGET)
            {
                s = "⭕";
                color = 1; // Red
            }
            else if (t == FILLED_TARGET)
            {
                s = "✅";
                color = 2; // Green
            }

            // Draw player on top
            if (x == current_state.px && y == current_state.py)
            {
                s = "🧙";
                color = 5; // Magenta
            }

            draw_entity_colored(x, y, s, color);
        }
    }
}

void draw_ui()
{
    int uiBaseY = MAP_HEIGHT + OFFSET_Y + 1;
    int uiBaseX = OFFSET_X;

    // Stats
    std::stringstream ss;
    ss << "Level: " << (CURRENT_LEVEL + 1) << "/" << NUM_OF_LEVELS;
    draw_text(uiBaseX, uiBaseY, ss.str());

    ss.str("");
    ss << "Moves: " << LEVEL_MOVES << " (Total: " << TOTAL_MOVES << ")";
    draw_text(uiBaseX, uiBaseY + 1, ss.str());

    ss.str("");
    ss << "Held: " << (current_state.holding_block ? "📦 Box" : "Nothing");
    draw_text(uiBaseX, uiBaseY + 2, ss.str());

    // Vim keys diagram
    draw_vim_keys(uiBaseY + 4, uiBaseX);

    // Instructions
    draw_text(uiBaseX, uiBaseY + 9, "[x/d] Pick Box  [p] Place Box  [u] Undo");
    draw_text(uiBaseX, uiBaseY + 10, "[q] Quit");
}

void draw_game()
{
    clear_screen();

    // Draw border
    int totalWidth = (MAP_WIDTH * 2) + (OFFSET_X * 2);
    int totalHeight = MAP_HEIGHT + OFFSET_Y + 16;
    draw_box(totalWidth, totalHeight);

    draw_game_board();
    draw_ui();

    // Victory message
    if (check_win())
    {
        LEVEL_COMPLETE = true;
        int centerX = MAP_WIDTH;
        int centerY = MAP_HEIGHT / 2;
        draw_text_colored(centerX * 2, centerY + OFFSET_Y, "🎉 LEVEL COMPLETE! 🎉", 2);
    }

    refresh_screen();
}

void level_message()
{
    clear_screen();

    int y = 8;
    std::stringstream ss;
    ss << "╔═══════════════════════════════╗";
    draw_text_colored(20, y++, ss.str(), 3);

    ss.str("");
    ss << "║                               ║";
    draw_text_colored(20, y++, ss.str(), 3);

    ss.str("");
    ss << "║       LEVEL " << (CURRENT_LEVEL + 1);
    if (CURRENT_LEVEL + 1 < 10) ss << " ";
    ss << " / " << NUM_OF_LEVELS << "           ║";
    draw_text_colored(20, y++, ss.str(), 6);

    ss.str("");
    ss << "║                               ║";
    draw_text_colored(20, y++, ss.str(), 3);

    ss.str("");
    ss << "╚═══════════════════════════════╝";
    draw_text_colored(20, y++, ss.str(), 3);

    y += 2;
    draw_text(20, y++, "📦 → ⭕ → ✅");
    y++;
    draw_text(20, y++, "Get Ready...");

    refresh_screen();
    usleep(1500000); // 1.5 seconds
}

void welcome_screen()
{
    clear_screen();

    // Draw a nice title
    int y = 3;
    draw_text_colored(12, y++, "╔═══════════════════════════════════════════╗", 3);
    draw_text_colored(12, y++, "║                                           ║", 3);
    draw_text_colored(12, y++, "║       🧙  SOKOBAN VIM  📦                ║", 6);
    draw_text_colored(12, y++, "║                                           ║", 3);
    draw_text_colored(12, y++, "║   Master Vim Commands Through Puzzles!    ║", 3);
    draw_text_colored(12, y++, "║                                           ║", 3);
    draw_text_colored(12, y++, "╚═══════════════════════════════════════════╝", 3);

    y += 1;
    draw_text_colored(12, y++, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", 7);

    y += 1;
    draw_text_colored(12, y++, "📖 OBJECTIVE:", 3);
    draw_text(12, y++, "   Place all boxes 📦 on target circles ⭕");
    draw_text(12, y++, "   When a box is correctly placed, it shows ✅");

    y += 1;
    draw_text_colored(12, y++, "🎮 MOVEMENT:", 3);
    draw_vim_keys(y, 12);
    y += 5;

    draw_text_colored(12, y++, "🛠️  ACTIONS (Vim-style):", 3);
    draw_text(12, y++, "   [x] or [d] - Delete/Cut box (pick it up)");
    draw_text(12, y++, "   [p]        - Paste box (place it down)");
    draw_text(12, y++, "   [u]        - Undo last move");
    draw_text(12, y++, "   [q]        - Quit game");

    y += 1;
    draw_text_colored(12, y++, "💡 TIPS:", 3);
    draw_text(12, y++, "   • Plan your moves carefully!");
    draw_text(12, y++, "   • Boxes can't be pushed into corners");
    draw_text(12, y++, "   • Use [u] to experiment with solutions");
    draw_text(12, y++, "   • Try to minimize your move count!");

    y += 1;
    draw_text_colored(12, y++, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━", 7);

    y += 1;
    draw_text_colored(18, y++, "✨ PRESS ENTER TO BEGIN YOUR JOURNEY! ✨", 2);
    draw_text_colored(22, y++, "Press ESC or [q] to exit", 7);

    refresh_screen();

    // Wait for Enter
    while (true)
    {
        int ch = get_input();
        if (ch == '\n' || ch == '\r')
        {
            break;
        }
        else if (ch == 27 || ch == 'q')
        {
            close_renderer();
            exit(0);
        }
    }
}

void level_complete_screen()
{
    clear_screen();

    int y = 6;

    if (CURRENT_LEVEL >= NUM_OF_LEVELS - 1)
    {
        // All levels completed!
        draw_text_colored(15, y++, "╔═════════════════════════════════════════╗", 3);
        draw_text_colored(15, y++, "║                                         ║", 3);
        draw_text_colored(15, y++, "║  🏆 CONGRATULATIONS! ALL LEVELS DONE! 🏆 ║", 2);
        draw_text_colored(15, y++, "║                                         ║", 3);
        draw_text_colored(15, y++, "╚═════════════════════════════════════════╝", 3);

        y += 2;
        draw_text_colored(15, y++, "    🎊 You are a Sokoban Master! 🎊", 6);

        y += 2;
        std::stringstream ss;
        ss << "📊 Final Statistics:";
        draw_text_colored(15, y++, ss.str(), 3);

        y++;
        ss.str("");
        ss << "   • Total Moves: " << TOTAL_MOVES;
        draw_text(15, y++, ss.str());

        ss.str("");
        ss << "   • Levels Completed: " << NUM_OF_LEVELS << "/" << NUM_OF_LEVELS;
        draw_text(15, y++, ss.str());

        ss.str("");
        double avg_moves = (double)TOTAL_MOVES / NUM_OF_LEVELS;
        ss << "   • Average Moves/Level: " << (int)avg_moves;
        draw_text(15, y++, ss.str());

        y += 2;
        draw_text(15, y++, "💡 Want more challenge?");
        draw_text(15, y++, "   Create your own levels! See MAP_CREATION_GUIDE.md");

        y += 2;
        draw_text_colored(15, y++, "Press ENTER to play again from Level 1", 2);
        draw_text(15, y++, "Press [q] to quit");
    }
    else
    {
        // Level completed
        draw_text_colored(18, y++, "╔═════════════════════════════╗", 3);
        draw_text_colored(18, y++, "║                             ║", 3);
        draw_text_colored(18, y++, "║   ✨ LEVEL COMPLETE! ✨     ║", 2);
        draw_text_colored(18, y++, "║                             ║", 3);
        draw_text_colored(18, y++, "╚═════════════════════════════╝", 3);

        y += 2;
        std::stringstream ss;
        ss << "📊 Level " << (CURRENT_LEVEL + 1) << " Statistics:";
        draw_text_colored(18, y++, ss.str(), 3);

        y++;
        ss.str("");
        ss << "   • Moves: " << LEVEL_MOVES;
        draw_text(18, y++, ss.str());

        ss.str("");
        ss << "   • Total Moves So Far: " << TOTAL_MOVES;
        draw_text(18, y++, ss.str());

        // Performance rating
        y++;
        std::string rating;
        int rating_color = 2;
        if (LEVEL_MOVES <= 10)
        {
            rating = "⭐⭐⭐ PERFECT! ⭐⭐⭐";
            rating_color = 2;
        }
        else if (LEVEL_MOVES <= 20)
        {
            rating = "⭐⭐ GREAT! ⭐⭐";
            rating_color = 3;
        }
        else if (LEVEL_MOVES <= 40)
        {
            rating = "⭐ GOOD! ⭐";
            rating_color = 6;
        }
        else
        {
            rating = "✓ Completed";
            rating_color = 7;
        }
        draw_text_colored(18, y++, rating, rating_color);

        y += 2;
        draw_text_colored(18, y++, "Press ENTER for next level", 2);
        draw_text(18, y++, "Press [q] to quit");
    }

    refresh_screen();

    // Wait for input
    while (true)
    {
        int ch = get_input();
        if (ch == '\n' || ch == '\r')
        {
            if (CURRENT_LEVEL >= NUM_OF_LEVELS - 1)
            {
                // Reset to first level
                CURRENT_LEVEL = 0;
                TOTAL_MOVES = 0;
            }
            else
            {
                CURRENT_LEVEL++;
            }
            break;
        }
        else if (ch == 'q' || ch == 27)
        {
            close_renderer();
            exit(0);
        }
    }
}

bool check_params(int argc, char** argv)
{
    // Parse command line arguments for level selection
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        // Check if it's a number (level selection)
        bool is_number = true;
        for (char c : arg)
        {
            if (!isdigit(c))
            {
                is_number = false;
                break;
            }
        }

        if (is_number)
        {
            int level = std::stoi(arg);
            if (level >= 0 && level < NUM_OF_LEVELS)
            {
                CURRENT_LEVEL = level;
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

int sokoban_main(int argc, char** argv)
{
    init_renderer();

    // Parse command line arguments
    if (!check_params(argc, argv))
    {
        close_renderer();
        return 1;
    }

    // Show welcome screen
    welcome_screen();

    // Main game loop
    while (true)
    {
        level_message();
        init_game();

        // Play current level
        while (!LEVEL_COMPLETE)
        {
            draw_game();
            int ch = get_input();
            update_input(ch);

            if (check_win())
            {
                LEVEL_COMPLETE = true;
                draw_game(); // Show final state
                usleep(1000000); // 1 second pause
            }
        }

        // Show completion screen
        level_complete_screen();
    }

    close_renderer();
    return 0;
}