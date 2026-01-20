#pragma once

#include "AnsiHelper.h"
#include <utility>

constexpr auto DEFAULT_COLOR_TEXT = AnsiColor::Default;
constexpr auto DEFAULT_COLOR_BACKGROUND = AnsiColor::Default;
constexpr auto DEFAULT_COLOR = std::make_pair(DEFAULT_COLOR_TEXT, DEFAULT_COLOR_BACKGROUND);

constexpr auto SELECTED_COLOR_TEXT = AnsiColor::Black;
constexpr auto SELECTED_COLOR_BACKGROUND = AnsiColor::White;
constexpr auto SELECTED_COLOR = std::make_pair(SELECTED_COLOR_TEXT, SELECTED_COLOR_BACKGROUND);

constexpr auto ERROR_COLOR_TEXT = AnsiColor::BrightWhite;
constexpr auto ERROR_COLOR_BACKGROUND = AnsiColor::Red;
constexpr auto ERROR_COLOR = std::make_pair(ERROR_COLOR_TEXT, ERROR_COLOR_BACKGROUND);

constexpr auto WATER_COLOR_BACKGROUND = AnsiColor::Blue;
constexpr auto LAND_COLOR_BACKGROUND = AnsiColor::Green;

constexpr auto MISS_COLOR_TEXT = AnsiColor::Cyan;
constexpr auto HIT_COLOR_TEXT = AnsiColor::Red;
constexpr auto DESTROYED_COLOR_TEXT = AnsiColor::Magenta;
