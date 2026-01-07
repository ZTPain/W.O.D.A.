#pragma once

#include "Frontend/Input/InputManager.h"
#include <cstddef>
#include <functional>

class Grid {
public:
  Grid(
      size_t xOffset,
      size_t yOffset,
      size_t width,
      size_t height,
      size_t cellWidth,
      size_t cellHeight,
      std::function<void(size_t, size_t, size_t, size_t, bool)> renderCellCallback,
      std::function<void(size_t, size_t, size_t, size_t)> onToggleCellCallback
  );
  ~Grid();

  void Render();

  void MoveCursorUp();
  void MoveCursorDown();
  void MoveCursorLeft();
  void MoveCursorRight();

  void ToggleCellAtCursor();

  void OnKeyPressed(ConsoleKeyDetails keyDetails);

private:
  size_t xOffset;
  size_t yOffset;
  size_t width;
  size_t height;
  size_t cellWidth;
  size_t cellHeight;
  size_t cellWidthWithBorders;
  size_t cellHeightWithBorders;
  std::function<void(size_t, size_t, size_t, size_t, bool)> renderCellCallback;
  std::function<void(size_t, size_t, size_t, size_t)> onToggleCellCallback;
  size_t cursorX;
  size_t cursorY;

  void MoveCursorTo(size_t x, size_t y);

  void DrawNumbersLegend(size_t row) const;
  void DrawLettersLegend(size_t cols) const;
  void RenderBorders() const;
  void InvokeOnRenderCell(size_t x, size_t y, bool isCursor);
};
