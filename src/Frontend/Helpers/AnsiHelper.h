#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

constexpr auto ANSI_ESCAPE_CODE = "\x1B[";

#define ANSI_RESET ANSI_ESCAPE_CODE "0m"
#define ANSI_BOLD ANSI_ESCAPE_CODE "1m"
#define ANSI_UNDERLINE ANSI_ESCAPE_CODE "4m"
#define ANSI_REVERSED ANSI_ESCAPE_CODE "7m"
#define ANSI_CLEAR_SCREEN ANSI_ESCAPE_CODE "2J"
#define ANSI_CLEAR_LINE ANSI_ESCAPE_CODE "2K"

// #define ANSI_MOVE_CURSOR(x, y) ANSI_ESCAPE_CODE #x ";" #y "H"
// #define ANSI_MOVE_CURSOR_UP(n) ANSI_ESCAPE_CODE #n "A"
// #define ANSI_MOVE_CURSOR_DOWN(n) ANSI_ESCAPE_CODE #n "B"
// #define ANSI_MOVE_CURSOR_RIGHT(n) ANSI_ESCAPE_CODE #n "C"
// #define ANSI_MOVE_CURSOR_LEFT(n) ANSI_ESCAPE_CODE #n "D"
#define ANSI_SAVE_CURSOR_POSITION ANSI_ESCAPE_CODE "s"
#define ANSI_RESTORE_CURSOR_POSITION ANSI_ESCAPE_CODE "u"

#define ANSI_GET_CURSOR_POSITION ANSI_ESCAPE_CODE "6n"
#define ANSI_HIDE_CURSOR ANSI_ESCAPE_CODE "?25l"
#define ANSI_SHOW_CURSOR ANSI_ESCAPE_CODE "?25h"

// #define ANSI_SET_TEXT_COLOR(color) ANSI_ESCAPE_CODE "38;2;" #color "m"
// #define ANSI_SET_BACKGROUND_COLOR(color) ANSI_ESCAPE_CODE "48;2;" #color "m"

inline std::string MoveCursor(size_t x, size_t y) {
  return ANSI_ESCAPE_CODE + std::to_string(y) + ";" + std::to_string(x) + "H";
}

enum class AnsiColor : uint8_t {
  Black = 0,
  Red = 1,
  Green = 2,
  Yellow = 3,
  Blue = 4,
  Magenta = 5,
  Cyan = 6,
  White = 7,
  Default = 9
};

#define ANSI_SET_RGB_TEXT_COLOR(r, g, b) ANSI_ESCAPE_CODE "38;2;" #r ";" #g ";" #b "m"

#define ANSI_SET_RGB_BACKGROUND_COLOR(r, g, b) ANSI_ESCAPE_CODE "48;2;" #r ";" #g ";" #b "m"
