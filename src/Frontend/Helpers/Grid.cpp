#include "Grid.h"
#include "AnsiHelper.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include <cstddef>
#include <functional>
#include <iostream>
#include <utility>

Grid::Grid(
    size_t xOffset,
    size_t yOffset,
    size_t width,
    size_t height,
    size_t cellWidth,
    size_t cellHeight,
    std::function<void(size_t, size_t, size_t, size_t, bool)> renderCellCallback,
    std::function<void(size_t, size_t, size_t, size_t)> onToggleCellCallback
)
    : xOffset(xOffset), yOffset(yOffset), width(width), height(height), cellWidth(cellWidth),
      cellHeight(cellHeight), cellWidthWithBorders(cellWidth + 2),
      cellHeightWithBorders(cellHeight + 1), renderCellCallback(std::move(renderCellCallback)),
      onToggleCellCallback(std::move(onToggleCellCallback)), cursorX(0), cursorY(0) {}

Grid::~Grid() = default;

void Grid::Render() {
  RenderBorders();
  for (size_t y = 0; y < height + 1; ++y) {
    if (y == 0) {
      DrawLettersLegend(width);
      continue;
    }
    for (size_t x = 0; x < width + 1; ++x) {
      if (x == 0) {
        DrawNumbersLegend(y);
        continue;
      }

      InvokeOnRenderCell(x - 1, y - 1, cursorX == x - 1 && cursorY == y - 1);
    }
  }
}

void Grid::MoveCursorUp() { MoveCursorTo(cursorX, (cursorY > 0) ? cursorY - 1 : cursorY); }

void Grid::MoveCursorDown() {
  MoveCursorTo(cursorX, (cursorY < height - 1) ? cursorY + 1 : cursorY);
}

void Grid::MoveCursorLeft() { MoveCursorTo((cursorX > 0) ? cursorX - 1 : cursorX, cursorY); }
void Grid::MoveCursorRight() {
  MoveCursorTo((cursorX < width - 1) ? cursorX + 1 : cursorX, cursorY);
}

void Grid::ToggleCellAtCursor() {
  onToggleCellCallback(
      cursorX, cursorY, cursorX * cellWidthWithBorders, cursorY * cellHeightWithBorders
  );

  InvokeOnRenderCell(cursorX, cursorY, true);
}

void Grid::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  static int lastNumberIndex = -1;
  auto isNumber = (keyDetails.key >= ConsoleKey::D0 && keyDetails.key <= ConsoleKey::D9);
  if (!isNumber)
    lastNumberIndex = -1;

  switch (keyDetails.key) {
    case ConsoleKey::UpArrow:
      MoveCursorUp();
      return;

    case ConsoleKey::DownArrow:
      MoveCursorDown();
      return;

    case ConsoleKey::LeftArrow:
      MoveCursorLeft();
      return;

    case ConsoleKey::RightArrow:
      MoveCursorRight();
      return;

    case ConsoleKey::Spacebar:
    case ConsoleKey::Enter:
      ToggleCellAtCursor();
      return;

    default: {
      auto isAlpha = (keyDetails.key >= ConsoleKey::A && keyDetails.key <= ConsoleKey::Z);

      if (isAlpha) {
        size_t const x = static_cast<size_t>(keyDetails.key) - static_cast<size_t>(ConsoleKey::A);
        if (x < width) {
          MoveCursorTo(x, cursorY);
        }
      } else if (isNumber) {
        size_t y = static_cast<size_t>(keyDetails.key) - static_cast<size_t>(ConsoleKey::D0);
        if (y == 0)
          y = 10;
        if (lastNumberIndex != -1) {
          y += static_cast<size_t>(lastNumberIndex) * 10;
          lastNumberIndex = -1;
        } else {
          lastNumberIndex = static_cast<int>(y);
        }

        y--;

        if (y < height) {
          MoveCursorTo(cursorX, y);
        }
      }
      return;
    }
  }
}

void Grid::MoveCursorTo(size_t x, size_t y) {
  InvokeOnRenderCell(cursorX, cursorY, false);

  cursorX = x % width;
  cursorY = y % height;

  InvokeOnRenderCell(cursorX, cursorY, true);
}

void Grid::InvokeOnRenderCell(size_t x, size_t y, bool isCursor) {
  renderCellCallback(
      x,
      y,
      ((x)*cellWidthWithBorders) + (cellWidthWithBorders / 2) + xOffset + 1,
      ((y)*cellHeightWithBorders) + (cellHeightWithBorders / 2) + yOffset + 1,
      isCursor
  );
}

void Grid::DrawNumbersLegend(size_t row) const {
  if (row == 0)
    return;

  IO::cout << AnsiHelper::MoveCursor(xOffset, (row * cellHeightWithBorders) + yOffset) << row;
}

void Grid::DrawLettersLegend(size_t cols) const {
  for (size_t col = 0; col < cols; ++col) {
    char const letter = static_cast<char>(static_cast<int>('A') + col);
    IO::cout << AnsiHelper::MoveCursor(((col + 1) * cellWidthWithBorders) + xOffset - 1, yOffset)
             << letter;
  }
}

void Grid::RenderBorders() const {
  for (size_t y = 0; y < height + 1; y++) {
    for (size_t x = 0; x < (width * cellWidthWithBorders) + (cellWidthWithBorders / 2); x++) {
      IO::cout << AnsiHelper::MoveCursor(
          x + xOffset, (y * cellHeightWithBorders) + (cellHeightWithBorders / 2) + yOffset
      );
      IO::cout << "─";
    }
  }

  for (size_t x = 0; x < width + 1; x++) {
    for (size_t y = 0; y < (height + 1) * cellHeightWithBorders; y++) {
      IO::cout << AnsiHelper::MoveCursor(
          (x * cellWidthWithBorders) + (cellWidthWithBorders / 2) + xOffset, y + yOffset
      );
      if (y % cellHeightWithBorders == cellHeightWithBorders / 2)
        if (y == 0)
          IO::cout << "┬";
        else if (y == (height * cellHeightWithBorders) + (cellHeightWithBorders / 2)) {
          if (x == width)
            IO::cout << "┘";
          else
            IO::cout << "┴";
        } else if (x == width)
          IO::cout << "┤";
        else
          IO::cout << "┼";
      else
        IO::cout << "│";
    }
  }
}
