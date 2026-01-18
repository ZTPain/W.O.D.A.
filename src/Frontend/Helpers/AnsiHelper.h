#pragma once

#include "Backend/Users/UserProfile.h"
#include <cstddef>
#include <cstdint>
#include <string>

#define USE_ANSI_CODES
// #define USE_WODA_CODES

enum class AnsiColor : uint8_t {
  Black = 0,
  Red = 1,
  Green = 2,
  Yellow = 3,
  Blue = 4,
  Magenta = 5,
  Cyan = 6,
  White = 7,
  Default = 9,

  BrightBlack = 60,
  BrightRed = 61,
  BrightGreen = 62,
  BrightYellow = 63,
  BrightBlue = 64,
  BrightMagenta = 65,
  BrightCyan = 66,
  BrightWhite = 67,
};

class AnsiHelper {
public:
  static std::string MoveCursor(size_t x, size_t y);

  static std::string SetTextColor(uint8_t r, uint8_t g, uint8_t b);
  static std::string SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b);

  static std::string SetTextColor(AnsiColor color);
  static std::string SetBackgroundColor(AnsiColor color);

  static std::string SetTextColor(Color color);
  static std::string SetBackgroundColor(Color color);

  constexpr static const char* Reset();
  constexpr static const char* Bold();
  constexpr static const char* Underline();
  constexpr static const char* Reversed();
  constexpr static const char* ClearScreen();
  constexpr static const char* ClearLine();
  constexpr static const char* SaveCursorPosition();
  constexpr static const char* RestoreCursorPosition();
  constexpr static const char* HideCursor();
  constexpr static const char* ShowCursor();
  constexpr static const char* SaveScreen();
  constexpr static const char* RestoreScreen();
  constexpr static const char* EnableAltScreenBuffer();
  constexpr static const char* DisableAltScreenBuffer();
};

#ifdef USE_ANSI_CODES

#define ANSI_ESCAPE_CODE "\x1B[" // NOLINT(cppcoreguidelines-macro-usage)

#define ANSI_RESET ANSI_ESCAPE_CODE "0m"
#define ANSI_BOLD ANSI_ESCAPE_CODE "1m"
#define ANSI_UNDERLINE ANSI_ESCAPE_CODE "4m"
#define ANSI_REVERSED ANSI_ESCAPE_CODE "7m"
#define ANSI_CLEAR_SCREEN ANSI_ESCAPE_CODE "2J"
#define ANSI_CLEAR_LINE ANSI_ESCAPE_CODE "2K"

#define ANSI_SAVE_CURSOR_POSITION ANSI_ESCAPE_CODE "s"
#define ANSI_RESTORE_CURSOR_POSITION ANSI_ESCAPE_CODE "u"

#define ANSI_HIDE_CURSOR ANSI_ESCAPE_CODE "?25l"
#define ANSI_SHOW_CURSOR ANSI_ESCAPE_CODE "?25h"

#define ANSI_SAVE_SCREEN ANSI_ESCAPE_CODE "?47h"
#define ANSI_RESTORE_SCREEN ANSI_ESCAPE_CODE "?47l"

#define ANSI_ENABLE_ALT_SCREEN_BUFFER ANSI_ESCAPE_CODE "?1049h"
#define ANSI_DISABLE_ALT_SCREEN_BUFFER ANSI_ESCAPE_CODE "?1049l"

inline std::string AnsiHelper::MoveCursor(size_t x, size_t y) {
  return ANSI_ESCAPE_CODE + std::to_string(y) + ";" + std::to_string(x) + "H";
}

inline std::string AnsiHelper::SetTextColor(uint8_t r, uint8_t g, uint8_t b) {
  return ANSI_ESCAPE_CODE "38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" +
         std::to_string(b) + "m";
}

inline std::string AnsiHelper::SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
  return ANSI_ESCAPE_CODE "48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" +
         std::to_string(b) + "m";
}

inline std::string AnsiHelper::SetTextColor(AnsiColor color) {
  return ANSI_ESCAPE_CODE + std::to_string(static_cast<uint8_t>(color) + 30) + "m";
}

inline std::string AnsiHelper::SetBackgroundColor(AnsiColor color) {
  return ANSI_ESCAPE_CODE + std::to_string(static_cast<uint8_t>(color) + 40) + "m";
}

inline std::string AnsiHelper::SetTextColor(Color color) {
  switch (color) {
    case Color::Black:
      return SetTextColor(AnsiColor::Black);
    case Color::Red:
      return SetTextColor(AnsiColor::Red);
    case Color::Green:
      return SetTextColor(AnsiColor::Green);
    case Color::Blue:
      return SetTextColor(AnsiColor::Blue);
    case Color::Gold:
      return SetTextColor(AnsiColor::Yellow);
    case Color::Default:
    default:
      return SetTextColor(AnsiColor::Default);
  }
}

inline std::string AnsiHelper::SetBackgroundColor(Color color) {
  switch (color) {
    case Color::Black:
      return SetBackgroundColor(AnsiColor::Black);
    case Color::Red:
      return SetBackgroundColor(AnsiColor::Red);
    case Color::Green:
      return SetBackgroundColor(AnsiColor::Green);
    case Color::Blue:
      return SetBackgroundColor(AnsiColor::Blue);
    case Color::Gold:
      return SetBackgroundColor(AnsiColor::Yellow);
    case Color::Default:
    default:
      return SetBackgroundColor(AnsiColor::Default);
  }
}

constexpr const char* AnsiHelper::Reset() { return ANSI_RESET; }

constexpr const char* AnsiHelper::Bold() { return ANSI_BOLD; }

constexpr const char* AnsiHelper::Underline() { return ANSI_UNDERLINE; }

constexpr const char* AnsiHelper::Reversed() { return ANSI_REVERSED; }

constexpr const char* AnsiHelper::ClearScreen() { return ANSI_CLEAR_SCREEN; }

constexpr const char* AnsiHelper::ClearLine() { return ANSI_CLEAR_LINE; }

constexpr const char* AnsiHelper::SaveCursorPosition() { return ANSI_SAVE_CURSOR_POSITION; }

constexpr const char* AnsiHelper::RestoreCursorPosition() { return ANSI_RESTORE_CURSOR_POSITION; }

constexpr const char* AnsiHelper::HideCursor() { return ANSI_HIDE_CURSOR; }

constexpr const char* AnsiHelper::ShowCursor() { return ANSI_SHOW_CURSOR; }

constexpr const char* AnsiHelper::SaveScreen() { return ANSI_SAVE_SCREEN; }

constexpr const char* AnsiHelper::RestoreScreen() { return ANSI_RESTORE_SCREEN; }

constexpr const char* AnsiHelper::EnableAltScreenBuffer() { return ANSI_ENABLE_ALT_SCREEN_BUFFER; }

constexpr const char* AnsiHelper::DisableAltScreenBuffer() {
  return ANSI_DISABLE_ALT_SCREEN_BUFFER;
}

#elif defined(USE_WODA_CODES)

// W.O.D.A specific console codes
#define WODA_ESCAPE_CODE "\x1B" // NOLINT(cppcoreguidelines-macro-usage)

enum class WodaAnsiCode : uint8_t {
  None,

  MoveCursor,

  ChangeForegroundColor,
  ChangeBackgroundColor,

  ResetColors,

  ChangeForegroundRgbColor,
  ChangeBackgroundRgbColor,

  ClearScreen,
  ClearLine,

  ShowCursor,
  HideCursor,

  BoldText,
  UnderlineText,
  InverseText,

  SaveCursorPosition,
  RestoreCursorPosition,
};

inline std::string AnsiHelper::MoveCursor(size_t x, size_t y) {
  std::array<char, 5> buffer{};
  buffer[0] = '\x1B';
  buffer[1] = static_cast<char>(WodaAnsiCode::MoveCursor);
  buffer[2] = static_cast<char>(x);
  buffer[3] = static_cast<char>(y);
  buffer[4] = ';';
  return std::string(buffer.data(), 5);
}

inline std::string AnsiHelper::SetTextColor(uint8_t r, uint8_t g, uint8_t b) {
  std::array<char, 6> buffer{};
  buffer[0] = '\x1B';
  buffer[1] = static_cast<char>(WodaAnsiCode::ChangeForegroundRgbColor);
  buffer[2] = static_cast<char>(r);
  buffer[3] = static_cast<char>(g);
  buffer[4] = static_cast<char>(b);
  buffer[5] = ';';
  return std::string(buffer.data(), 6);
}

inline std::string AnsiHelper::SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
  std::array<char, 6> buffer{};
  buffer[0] = '\x1B';
  buffer[1] = static_cast<char>(WodaAnsiCode::ChangeBackgroundRgbColor);
  buffer[2] = static_cast<char>(r);
  buffer[3] = static_cast<char>(g);
  buffer[4] = static_cast<char>(b);
  buffer[5] = ';';
  return std::string(buffer.data(), 6);
}

inline std::string AnsiHelper::SetTextColor(AnsiColor color) {
  return std::string{WODA_ESCAPE_CODE, static_cast<char>(WodaAnsiCode::ChangeForegroundColor)} +
         static_cast<char>(color) + ';';
}

inline std::string AnsiHelper::SetBackgroundColor(AnsiColor color) {
  return std::string{WODA_ESCAPE_CODE, static_cast<char>(WodaAnsiCode::ChangeBackgroundColor)} +
         static_cast<char>(color) + ';';
}

constexpr const char* AnsiHelper::Reset() {
  return WODA_ESCAPE_CODE "\x04"
                          ";";
}

constexpr const char* AnsiHelper::Bold() {
  return WODA_ESCAPE_CODE "\x0B"
                          ";";
}

constexpr const char* AnsiHelper::Underline() {
  return WODA_ESCAPE_CODE "\x0C"
                          ";";
}

constexpr const char* AnsiHelper::Reversed() {
  return WODA_ESCAPE_CODE "\x0D"
                          ";";
}

constexpr const char* AnsiHelper::ClearScreen() {
  return WODA_ESCAPE_CODE "\x07"
                          ";";
}

constexpr const char* AnsiHelper::ClearLine() {
  return WODA_ESCAPE_CODE "\x08"
                          ";";
}

constexpr const char* AnsiHelper::SaveCursorPosition() {
  return WODA_ESCAPE_CODE "\x0E"
                          ";";
}

constexpr const char* AnsiHelper::RestoreCursorPosition() {
  return WODA_ESCAPE_CODE "\x0F"
                          ";";
}

constexpr const char* AnsiHelper::HideCursor() {
  return WODA_ESCAPE_CODE "\x0A"
                          ";";
}

constexpr const char* AnsiHelper::ShowCursor() {
  return WODA_ESCAPE_CODE "\x09"
                          ";";
}

#else
#error "No console code system defined. Define either USE_ANSI_CODES or USE_WODA_CODES."
#endif
