#ifndef VIMNET_GAME_H
#define VIMNET_GAME_H

#include <vector>
#include <string>

// Define game modes
enum class GameMode {
    NORMAL,
    INSERT,
    VISUAL,
    COMMAND,
    REPLACE
};

// Define a structure for a level
struct Level {
    std::vector<std::string> initial_buffer;
    std::vector<std::string> target_buffer;
    // Potentially add level specific mechanics, intro text, boss info etc.
};

class VimNetGame {
public:
    VimNetGame(); // Constructor to initialize buffer
    void run();

    // Buffer manipulation methods
    const std::string& get_line(int line_num) const;
    int get_line_count() const;
    void insert_char(int line_num, int col, char c);
    void delete_char(int line_num, int col);
    void new_line(int line_num, int col);
    void join_lines(int line_num);

    // Cursor methods
    int get_cursor_x() const { return cursor_x; }
    int get_cursor_y() const { return cursor_y; }
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    void set_cursor_position(int y, int x);

    // Word movement
    void move_word_forward();
    void move_word_backward();
    void move_to_end_of_word();
    void delete_word();
    void undo();
    void yank();
    void paste();
    void delete_line();
    void search_forward(const std::string& query);
    void search_backward(const std::string& query);
    void find_next();
    void find_previous();
    void find_char_forward(char c);
    void find_char_forward_before(char c);
    void replace_char(char c);
    void change_word();

    // Mode methods
    GameMode get_mode() const { return current_mode; }
    void set_mode(GameMode new_mode);

    // Level management
    void load_level(const Level& level);
    bool is_level_complete() const;


private:
    std::vector<std::string> buffer;
    std::vector<std::string> target_buffer; // Added to store target state
    std::vector<std::vector<std::string>> history; // For undo
    std::vector<std::string> yank_register;
    int cursor_x;
    int cursor_y;
    int current_level_index;
    GameMode current_mode;
    int visual_start_y, visual_start_x;
    int visual_end_y, visual_end_x;
    std::string command_buffer;
    std::string last_search_query;

    void clamp_cursor_position();
    void save_history();
    void handle_visual_mode_input(int ch);
    void handle_normal_mode_input(int ch);
    void handle_insert_mode_input(int ch);
    void handle_command_mode_input(int ch);
    void handle_replace_mode_input(int ch);
};

#endif // VIMNET_GAME_H
