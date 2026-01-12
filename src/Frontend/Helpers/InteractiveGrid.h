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

  Grid(const Grid&) = delete;
  Grid()
      : xOffset(0), yOffset(0), width(0), height(0), cellWidth(0), cellHeight(0),
        cellWidthWithBorders(0), cellHeightWithBorders(0), renderCellCallback(nullptr),
        onToggleCellCallback(nullptr), cursorX(0), cursorY(0) {}

  void Render();

  void MoveCursorUp();
  void MoveCursorDown();
  void MoveCursorLeft();
  void MoveCursorRight();

  void ToggleCellAtCursor();

  void OnKeyPressed(ConsoleKeyDetails keyDetails);

  [[nodiscard]] size_t GetWidth() const { return width; }
  [[nodiscard]] size_t GetHeight() const { return height; }

  [[nodiscard]] size_t GetXOffset() const { return xOffset; }
  [[nodiscard]] size_t GetYOffset() const { return yOffset; }

  [[nodiscard]] size_t GetCellWidth() const { return cellWidth; }
  [[nodiscard]] size_t GetCellHeight() const { return cellHeight; }

  [[nodiscard]] size_t GetTotalWidth() const { return width * cellWidthWithBorders; }
  [[nodiscard]] size_t GetTotalHeight() const { return height * cellHeightWithBorders; }

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
