#include "game.h"
#include "levels.h"
#include "renderer.h"
#include <algorithm>
#include <chrono>
#include <ncurses.h>
#include <stdexcept>
#include <thread>

VimNetGame::VimNetGame()
    : cursor_x(0), cursor_y(0), current_level_index(0),
      current_mode(GameMode::NORMAL) {
  load_level(get_level(current_level_index)); // Load the first level
  clamp_cursor_position();
}

void VimNetGame::run() {
  bool game_running = true;
  while (game_running) {
    clear_screen();

    draw_box(get_screen_width(), get_screen_height());

    int status_line_y = get_screen_height() - 2; // Line above the bottom border

    if (current_mode == GameMode::VISUAL) {
      draw_buffer(buffer, 1, 1, cursor_y, cursor_x, true, visual_start_y,
                  visual_start_x, visual_end_y, visual_end_x);
    } else {
      draw_buffer(buffer, 1, 1, cursor_y, cursor_x, false, 0, 0, 0, 0);
    }

    // Mode display
    std::string mode_str;
    switch (current_mode) {
    case GameMode::NORMAL:
      mode_str = "-- NORMAL --";
      break;
    case GameMode::INSERT:
      mode_str = "-- INSERT --";
      break;
    case GameMode::VISUAL:
      mode_str = "-- VISUAL --";
      break;
    case GameMode::COMMAND:
      mode_str = command_buffer;
      break;
    case GameMode::REPLACE:
      mode_str = "-- REPLACE --";
      break;
    }
    draw_text_colored(status_line_y, 1, mode_str, COLOR_PAIR_BLUE);

    // Level status
    if (is_level_complete()) {
      current_level_index++;
      if (current_level_index < get_total_levels()) {
        load_level(get_level(current_level_index));
      } else {
        // Game finished
        draw_text_colored(status_line_y, get_screen_width() - 20,
                          "== GAME COMPLETE ==", COLOR_PAIR_GREEN);
        refresh_screen();
        std::this_thread::sleep_for(std::chrono::seconds(5));
        game_running = false;
      }
    } else {
      std::string level_str = "Level " +
                              std::to_string(current_level_index + 1) + "/" +
                              std::to_string(get_total_levels());
      draw_text_colored(status_line_y,
                        get_screen_width() - level_str.length() - 2, level_str,
                        COLOR_PAIR_YELLOW);
    }

    refresh_screen();

    // Handle input
    int ch = get_input();

    // Global quit check (optional, but good for safety) - restricting to Normal
    // mode for Vim realism if (ch == 'q' && current_mode == GameMode::NORMAL) {
    // game_running = false; }

    switch (current_mode) {
    case GameMode::NORMAL:
      if (ch == 'q') {
        game_running = false;
      } else {
        handle_normal_mode_input(ch);
      }
      break;
    case GameMode::INSERT:
      handle_insert_mode_input(ch);
      break;
    case GameMode::VISUAL:
      handle_visual_mode_input(ch);
      break;
    case GameMode::COMMAND:
      handle_command_mode_input(ch);
      break;
    case GameMode::REPLACE:
      handle_replace_mode_input(ch);
      break;
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(50)); // Small delay for responsiveness
  }
}

void VimNetGame::handle_normal_mode_input(int ch) {
  if (ch == 'h') {
    move_left();
  } else if (ch == 'l') {
    move_right();
  } else if (ch == 'j') {
    move_down();
  } else if (ch == 'k') {
    move_up();
  } else if (ch == 'w') {
    move_word_forward();
  } else if (ch == 'b') {
    move_word_backward();
  } else if (ch == 'e') {
    move_to_end_of_word();
  } else if (ch == 'g') {
    int next_ch = get_input();
    if (next_ch == 'g') {
      cursor_y = 0;
      cursor_x = 0;
      clamp_cursor_position();
    }
  } else if (ch == 'G') {
    cursor_y = buffer.size() - 1;
    cursor_x = 0;
    clamp_cursor_position();
  } else if (ch == 'f') {
    int next_ch = get_input();
    find_char_forward((char)next_ch);
  } else if (ch == 't') {
    int next_ch = get_input();
    find_char_forward_before((char)next_ch);
  } else if (ch == 'r') {
    int next_ch = get_input();
    if (next_ch >= 32 && next_ch <= 126) {
      save_history();
      replace_char((char)next_ch);
    }
  } else if (ch == 'R') {
    save_history();
    set_mode(GameMode::REPLACE);
  } else if (ch == 'c') {
    int next_ch = get_input();
    if (next_ch == 'w') {
      save_history();
      change_word();
    }
  } else if (ch == 'd') {
    int next_ch = get_input();
    if (next_ch == 'w') {
      save_history();
      delete_word();
    } else if (next_ch == 'd') {
      delete_line();
    }
  } else if (ch == 'p') {
    paste();
  } else if (ch == 'u') {
    undo();
  } else if (ch == 'v') {
    set_mode(GameMode::VISUAL);
  } else if (ch == '/' || ch == ':') {
    command_buffer.clear();
    command_buffer += (char)ch;
    set_mode(GameMode::COMMAND);
  } else if (ch == 'n') {
    find_next();
  } else if (ch == 'N') {
    find_previous();
  } else if (ch == 'i') {
    save_history();
    set_mode(GameMode::INSERT);
  } else if (ch == 'x') { // Simple delete character
    if (!buffer.empty() && !buffer[cursor_y].empty() &&
        cursor_x < buffer[cursor_y].length()) {
      save_history();
      delete_char(cursor_y, cursor_x);
    }
  }
}

void VimNetGame::handle_insert_mode_input(int ch) {
  if (ch == 27) { // ESC key
    set_mode(GameMode::NORMAL);
  } else if (ch >= 32 && ch <= 126) { // Printable characters
    insert_char(cursor_y, cursor_x, (char)ch);
    move_right();        // Move cursor after inserting
  } else if (ch == 13) { // Enter key
    new_line(cursor_y, cursor_x);
    move_down();
    cursor_x = 0;
  } else if (ch == 127 || ch == 8 || ch == KEY_BACKSPACE) { // Backspace
    if (cursor_x > 0) {
      delete_char(cursor_y, cursor_x - 1);
      move_left();
    } else if (cursor_y > 0) {
      // Join with previous line
      int prev_line_len = buffer[cursor_y - 1].length();
      join_lines(cursor_y - 1);
      cursor_y--;
      cursor_x = prev_line_len;
    }
  }
}

void VimNetGame::handle_replace_mode_input(int ch) {
  if (ch == 27) { // ESC key
    set_mode(GameMode::NORMAL);
  } else if (ch >= 32 && ch <= 126) { // Printable characters
    replace_char((char)ch);
    move_right();
  }
}

void VimNetGame::handle_command_mode_input(int ch) {
  if (ch == 13) { // Enter
    if (command_buffer[0] == '/') {
      last_search_query = command_buffer.substr(1);
      set_mode(GameMode::NORMAL);
      search_forward(last_search_query);
    } else if (command_buffer[0] == ':') {
      std::string cmd = command_buffer.substr(1);
      if (cmd.rfind("s/", 0) == 0) { // starts with s/
        std::string parts[3];
        int part_idx = 0;
        size_t last_pos = 2;
        for (size_t i = 2; i < cmd.length() && part_idx < 2; ++i) {
          if (cmd[i] == '/') {
            parts[part_idx++] = cmd.substr(last_pos, i - last_pos);
            last_pos = i + 1;
          }
        }
        parts[part_idx] = cmd.substr(last_pos);

        save_history();
        size_t pos = buffer[cursor_y].find(parts[0]);
        if (pos != std::string::npos) {
          buffer[cursor_y].replace(pos, parts[0].length(), parts[1]);
        }
      } else {
        try {
          int line_num = std::stoi(cmd);
          cursor_y = line_num - 1; // Convert to 0-based index
          clamp_cursor_position();
        } catch (const std::invalid_argument &e) {
          // Not a number, do nothing for now
        }
      }
      set_mode(GameMode::NORMAL);
    }
  } else if (ch == 27) { // Esc
    set_mode(GameMode::NORMAL);
  } else if (ch >= 32 && ch <= 126) {
    command_buffer += (char)ch;
  } else if (ch == 127 || ch == 8 || ch == KEY_BACKSPACE) {
    if (command_buffer.length() > 0) {
      command_buffer.pop_back();
      if (command_buffer.empty()) {
        set_mode(GameMode::NORMAL);
      }
    }
  }
}

const std::string &VimNetGame::get_line(int line_num) const {
  if (line_num < 0 || line_num >= buffer.size()) {
    throw std::out_of_range("Line number out of bounds.");
  }
  return buffer[line_num];
}

int VimNetGame::get_line_count() const { return buffer.size(); }

void VimNetGame::insert_char(int line_num, int col, char c) {
  if (line_num < 0 || line_num >= buffer.size()) {
    throw std::out_of_range("Line number out of bounds.");
  }
  if (col < 0 || col > buffer[line_num].length()) {
    throw std::out_of_range("Column out of bounds.");
  }
  buffer[line_num].insert(col, 1, c);
  clamp_cursor_position();
}

void VimNetGame::delete_char(int line_num, int col) {
  if (line_num < 0 || line_num >= buffer.size()) {
    throw std::out_of_range("Line number out of bounds.");
  }
  if (col < 0 || col >= buffer[line_num].length()) {
    throw std::out_of_range("Column out of bounds.");
  }
  buffer[line_num].erase(col, 1);
  clamp_cursor_position();
}

void VimNetGame::new_line(int line_num, int col) {
  if (line_num < 0 || line_num >= buffer.size()) {
    throw std::out_of_range("Line number out of bounds.");
  }
  if (col < 0 || col > buffer[line_num].length()) {
    throw std::out_of_range("Column out of bounds.");
  }

  std::string current_line = buffer[line_num];
  buffer[line_num] = current_line.substr(0, col);
  buffer.insert(buffer.begin() + line_num + 1, current_line.substr(col));
  clamp_cursor_position();
}

void VimNetGame::join_lines(int line_num) {
  if (line_num < 0 ||
      line_num >= buffer.size() - 1) { // Need at least two lines to join
    throw std::out_of_range(
        "Line number out of bounds or no next line to join.");
  }
  buffer[line_num] += buffer[line_num + 1];
  buffer.erase(buffer.begin() + line_num + 1);
  clamp_cursor_position();
}

void VimNetGame::clamp_cursor_position() {
  // Clamp cursor_y
  cursor_y = std::max(0, std::min(cursor_y, (int)buffer.size() - 1));
  if (buffer.empty()) {
    cursor_x = 0;
    cursor_y = 0;
    return;
  }
  // Clamp cursor_x
  cursor_x = std::max(0, std::min(cursor_x, (int)buffer[cursor_y].length()));
}

void VimNetGame::move_left() {
  if (cursor_x > 0) {
    cursor_x--;
  } else if (cursor_y > 0) {
    cursor_y--;
    cursor_x = buffer[cursor_y].length(); // Move to end of previous line
  }
  clamp_cursor_position();
}

void VimNetGame::move_right() {
  if (!buffer.empty() && cursor_x < buffer[cursor_y].length()) {
    cursor_x++;
  } else if (cursor_y < buffer.size() - 1) {
    cursor_y++;
    cursor_x = 0; // Move to beginning of next line
  }
  clamp_cursor_position();
}

void VimNetGame::move_up() {
  if (cursor_y > 0) {
    cursor_y--;
  }
  clamp_cursor_position();
}

void VimNetGame::move_down() {
  if (cursor_y < buffer.size() - 1) {
    cursor_y++;
  }
  clamp_cursor_position();
}

void VimNetGame::move_word_forward() {
  // Simplified vim 'w' motion. Skips to the start of the next word.
  // A word is a sequence of non-whitespace characters.
  if (buffer.empty())
    return;

  // Find the next word start
  while (true) {
    // Move to the right
    if (cursor_x < buffer[cursor_y].length()) {
      cursor_x++;
    } else if (cursor_y < buffer.size() - 1) {
      cursor_y++;
      cursor_x = 0;
    } else {
      // End of buffer
      break;
    }

    // If we are at the start of a word, stop
    if (!isspace(buffer[cursor_y][cursor_x]) &&
        (cursor_x == 0 || isspace(buffer[cursor_y][cursor_x - 1]))) {
      break;
    }
  }
  clamp_cursor_position();
}

void VimNetGame::move_word_backward() {
  // Simplified vim 'b' motion. Skips to the start of the previous word.
  if (buffer.empty())
    return;

  while (true) {
    // Move to the left
    if (cursor_x > 0) {
      cursor_x--;
    } else if (cursor_y > 0) {
      cursor_y--;
      cursor_x = buffer[cursor_y].length();
    } else {
      // Beginning of buffer
      break;
    }

    // If we are at the start of a word, stop
    if (!isspace(buffer[cursor_y][cursor_x]) &&
        (cursor_x == 0 || isspace(buffer[cursor_y][cursor_x - 1]))) {
      break;
    }
  }
  clamp_cursor_position();
}

void VimNetGame::move_to_end_of_word() {
  // Simplified vim 'e' motion. Skips to the end of the current/next word.
  if (buffer.empty())
    return;

  while (true) {
    // Move to the right
    if (cursor_x < buffer[cursor_y].length()) {
      cursor_x++;
    } else if (cursor_y < buffer.size() - 1) {
      cursor_y++;
      cursor_x = 0;
    } else {
      // End of buffer
      cursor_x =
          buffer[cursor_y].length() > 0 ? buffer[cursor_y].length() - 1 : 0;
      break;
    }

    // If we are at the end of a word, stop
    if (!isspace(buffer[cursor_y][cursor_x]) &&
        (cursor_x == buffer[cursor_y].length() - 1 ||
         isspace(buffer[cursor_y][cursor_x + 1]))) {
      break;
    }
  }
  clamp_cursor_position();
}

void VimNetGame::search_forward(const std::string &query) {
  if (query.empty())
    return;

  for (int y = cursor_y; y < buffer.size(); ++y) {
    size_t found_pos =
        buffer[y].find(query, (y == cursor_y) ? cursor_x + 1 : 0);
    if (found_pos != std::string::npos) {
      cursor_y = y;
      cursor_x = found_pos;
      return;
    }
  }
  // Wrap around search
  for (int y = 0; y < cursor_y; ++y) {
    size_t found_pos = buffer[y].find(query);
    if (found_pos != std::string::npos) {
      cursor_y = y;
      cursor_x = found_pos;
      return;
    }
  }
}

void VimNetGame::search_backward(const std::string &query) {
  if (query.empty())
    return;

  for (int y = cursor_y; y >= 0; --y) {
    size_t found_pos = buffer[y].rfind(
        query, (y == cursor_y) ? cursor_x - 1 : std::string::npos);
    if (found_pos != std::string::npos) {
      cursor_y = y;
      cursor_x = found_pos;
      return;
    }
  }
  // Wrap around search
  for (int y = buffer.size() - 1; y > cursor_y; --y) {
    size_t found_pos = buffer[y].rfind(query);
    if (found_pos != std::string::npos) {
      cursor_y = y;
      cursor_x = found_pos;
      return;
    }
  }
}

void VimNetGame::find_next() { search_forward(last_search_query); }

void VimNetGame::find_previous() { search_backward(last_search_query); }

void VimNetGame::find_char_forward(char c) {
  if (buffer.empty())
    return;
  size_t found_pos = buffer[cursor_y].find(c, cursor_x + 1);
  if (found_pos != std::string::npos) {
    cursor_x = found_pos;
  }
}

void VimNetGame::find_char_forward_before(char c) {
  if (buffer.empty())
    return;
  size_t found_pos = buffer[cursor_y].find(c, cursor_x + 1);
  if (found_pos != std::string::npos) {
    cursor_x = found_pos - 1;
  }
}

void VimNetGame::replace_char(char c) {
  if (!buffer.empty() && !buffer[cursor_y].empty() &&
      cursor_x < buffer[cursor_y].length()) {
    buffer[cursor_y][cursor_x] = c;
  }
}

void VimNetGame::change_word() {
  delete_word();
  set_mode(GameMode::INSERT);
}

void VimNetGame::set_cursor_position(int y, int x) {
  cursor_y = y;
  cursor_x = x;
  clamp_cursor_position();
}

void VimNetGame::delete_word() {
  if (buffer.empty())
    return;

  int start_y = cursor_y;
  int start_x = cursor_x;

  move_word_forward();

  int end_y = cursor_y;
  int end_x = cursor_x;

  // Reset cursor to start position for deletion
  cursor_y = start_y;
  cursor_x = start_x;

  if (start_y == end_y) {
    buffer[start_y].erase(start_x, end_x - start_x);
  } else {
    // Deletion spans multiple lines
    buffer[start_y].erase(start_x);
    for (int i = start_y + 1; i < end_y; ++i) {
      buffer[i].clear();
    }
    buffer[start_y] += buffer[end_y].substr(end_x);
    buffer.erase(buffer.begin() + start_y + 1, buffer.begin() + end_y + 1);
  }
  clamp_cursor_position();
}

void VimNetGame::undo() {
  if (!history.empty()) {
    buffer = history.back();
    history.pop_back();
    clamp_cursor_position(); //
  }
}

void VimNetGame::save_history() {
  history.push_back(buffer);
  if (history.size() > 50) { // Limit undo history
    history.erase(history.begin());
  }
}

void VimNetGame::handle_visual_mode_input(int ch) {
  switch (ch) {
  case 'h':
    move_left();
    break;
  case 'l':
    move_right();
    break;
  case 'k':
    move_up();
    break;
  case 'j':
    move_down();
    break;
  case 'w':
    move_word_forward();
    break;
  case 'b':
    move_word_backward();
    break;
  case 'e':
    move_to_end_of_word();
    break;
  case 'y':
    yank();
    break;
  }
  // Update the end of the selection to the current cursor position
  visual_end_y = cursor_y;

  visual_end_x = cursor_x;
}

void VimNetGame::yank() {
  if (current_mode != GameMode::VISUAL)
    return;

  yank_register.clear();

  int start_y = std::min(visual_start_y, visual_end_y);
  int end_y = std::max(visual_start_y, visual_end_y);
  int start_x =
      (visual_start_y < visual_end_y ||
       (visual_start_y == visual_end_y && visual_start_x < visual_end_x))
          ? visual_start_x
          : visual_end_x;
  int end_x =
      (visual_start_y > visual_end_y ||
       (visual_start_y == visual_end_y && visual_start_x > visual_end_x))
          ? visual_start_x
          : visual_end_x;

  if (start_y == end_y) {
    yank_register.push_back(buffer[start_y].substr(start_x, end_x - start_x));
  } else {
    yank_register.push_back(buffer[start_y].substr(start_x));
    for (int i = start_y + 1; i < end_y; ++i) {
      yank_register.push_back(buffer[i]);
    }
    yank_register.push_back(buffer[end_y].substr(0, end_x));
  }

  set_mode(GameMode::NORMAL);
}

void VimNetGame::paste() {
  if (yank_register.empty())
    return;

  save_history();

  if (yank_register.size() == 1) {
    buffer[cursor_y].insert(cursor_x, yank_register[0]);
  } else {
    std::string first_line_end = buffer[cursor_y].substr(cursor_x);
    buffer[cursor_y].erase(cursor_x);
    buffer[cursor_y] += yank_register[0];

    for (size_t i = 1; i < yank_register.size(); ++i) {
      buffer.insert(buffer.begin() + cursor_y + i, yank_register[i]);
    }

    buffer[cursor_y + yank_register.size() - 1] += first_line_end;
  }

  clamp_cursor_position();
}

void VimNetGame::delete_line() {
  if (buffer.empty())
    return;

  save_history();
  yank_register.clear();
  yank_register.push_back(buffer[cursor_y]);
  buffer.erase(buffer.begin() + cursor_y);

  if (buffer.empty()) {
    buffer.push_back("");
  }

  clamp_cursor_position();
}

void VimNetGame::set_mode(GameMode new_mode) {
  if (new_mode == GameMode::VISUAL && current_mode != GameMode::VISUAL) {
    visual_start_y = cursor_y;
    visual_start_x = cursor_x;
    visual_end_y = cursor_y;
    visual_end_x = cursor_x;
  }

  // Set cursor visibility based on mode
  if (new_mode == GameMode::INSERT || new_mode == GameMode::REPLACE) {
    set_cursor_visibility(true);
  } else {
    set_cursor_visibility(false);
  }

  current_mode = new_mode;

  // When entering NORMAL mode, cursor should be on a character, not past the
  // end of line
  if (current_mode == GameMode::NORMAL && !buffer.empty()) {
    cursor_x = std::min(cursor_x, (int)buffer[cursor_y].length() > 0
                                      ? (int)buffer[cursor_y].length() - 1
                                      : 0);
  }
  clamp_cursor_position();
}

void VimNetGame::load_level(const Level &level) {
  buffer = level.initial_buffer;
  target_buffer = level.target_buffer;
  cursor_x = 0;
  cursor_y = 0;
  current_mode = GameMode::NORMAL; // Always start a level in normal mode
  clamp_cursor_position();
}

bool VimNetGame::is_level_complete() const { return buffer == target_buffer; }
