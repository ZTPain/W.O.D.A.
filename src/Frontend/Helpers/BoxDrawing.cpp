#include "BoxDrawing.h"
#include "AnsiHelper.h"
#include "Frontend/Input/IO.h"
#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>

std::unordered_map<BoxStyle, BoxStyleDefinition> BoxDrawing::boxStyles = {
    { BoxStyle::Single, {"┌", "┐", "└", "┘", "─", "│", "├", "┤"}},
    { BoxStyle::Double, {"╔", "╗", "╚", "╝", "═", "║", "╠", "╣"}},
    {   BoxStyle::Bold, {"█", "█", "█", "█", "█", "█", "█", "█"}},
    {BoxStyle::Rounded, {"╭", "╮", "╰", "╯", "─", "│", "├", "┤"}},
};

void BoxDrawing::DrawBox(
    size_t x,
    size_t y,
    size_t width,
    size_t height,
    BoxStyle style,
    bool filled,
    const char* title,
    bool flush
) {
  const auto styleDef = boxStyles[style];

  DrawHorizontalLine(x + 1, y, width - 2, style, false);              // Top edge
  DrawHorizontalLine(x + 1, y + height - 1, width - 2, style, false); // Bottom edge

  DrawVerticalLine(x, y + 1, height - 2, style, false);             // Left edge
  DrawVerticalLine(x + width - 1, y + 1, height - 2, style, false); // Right edge

  // Draw corners
  IO::cout << AnsiHelper::MoveCursor(x, y) << styleDef.topLeft;
  IO::cout << AnsiHelper::MoveCursor(x + width - 1, y) << styleDef.topRight;
  IO::cout << AnsiHelper::MoveCursor(x, y + height - 1) << styleDef.bottomLeft;
  IO::cout << AnsiHelper::MoveCursor(x + width - 1, y + height - 1) << styleDef.bottomRight;

  // Draw title if provided
  if (title != nullptr) {
    auto titleWithBrackets = styleDef.breakLeft + std::string(title) + styleDef.breakRight;
    const size_t titleLength = titleWithBrackets.length();
    if (titleLength < width - 4) { // Ensure title fits within the box
      const size_t titleX = x + 1 + ((width / 2 - titleLength / 2));
      IO::cout << AnsiHelper::MoveCursor(titleX, y)
               << titleWithBrackets; // Position title at the top center
    }
  }

  if (filled) {
    // Fill the box interior
    for (size_t row = y + 1; row < y + height - 1; ++row) {
      IO::cout << AnsiHelper::MoveCursor(x + 1, row);
      for (size_t col = x + 1; col < x + width - 1; ++col) {
        IO::cout << ' ';
      }
    }
  }

  if (flush)
    IO::cout.flush();
}

void BoxDrawing::ClearBox(
    size_t x, size_t y, size_t width, size_t height, bool filled, bool flush
) {
  if (filled) {
    IO::cout << AnsiHelper::MoveCursor(x + 1, y + 1);
    for (size_t row = y + 1; row < y + height - 1; ++row) {
      for (size_t col = x + 1; col < x + width - 1; ++col) {
        IO::cout << ' ';
      }
      IO::cout << '\n';
    }
  } else {
    // Clear edges
    for (size_t col = x; col < x + width; ++col) {
      IO::cout << AnsiHelper::MoveCursor(col, y) << ' ';              // Top edge
      IO::cout << AnsiHelper::MoveCursor(col, y + height - 1) << ' '; // Bottom edge
    }
    for (size_t row = y; row < y + height; ++row) {
      IO::cout << AnsiHelper::MoveCursor(x, row) << ' ';             // Left edge
      IO::cout << AnsiHelper::MoveCursor(x + width - 1, row) << ' '; // Right edge
    }
  }

  if (flush)
    IO::cout.flush();
}

void BoxDrawing::DrawHorizontalLine(size_t x, size_t y, size_t length, BoxStyle style, bool flush) {
  for (size_t col = x; col < x + length; ++col) {
    IO::cout << AnsiHelper::MoveCursor(col, y) << boxStyles[style].horizontal;
  }

  if (flush)
    IO::cout.flush();
}

void BoxDrawing::DrawVerticalLine(size_t x, size_t y, size_t length, BoxStyle style, bool flush) {
  for (size_t row = y; row < y + length; ++row) {
    IO::cout << AnsiHelper::MoveCursor(x, row) << boxStyles[style].vertical;
  }

  if (flush)
    IO::cout.flush();
}
