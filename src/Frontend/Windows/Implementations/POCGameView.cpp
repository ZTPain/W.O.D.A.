#include "POCGameView.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/InteractiveGrid.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <array>
#include <cstddef>
#include <iostream>

POCGameView::POCGameView() : Window(WindowType::InGame) {}

POCGameView::~POCGameView() = default;

static constexpr int GRID_WIDTH = 22;
static constexpr int GRID_HEIGHT = 14;
static std::array<std::array<bool, GRID_WIDTH>, GRID_HEIGHT> gameGrid;

static void RenderCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) {
  IO::cout << AnsiHelper::MoveCursor(posX, posY);
  if (isCursor) {
    IO::cout << (gameGrid.at(y).at(x) ? "[X]" : "[ ]");
  } else {
    IO::cout << (gameGrid.at(y).at(x) ? " X " : " . ");
  }
  IO::cout.flush();
}

static void OnToggleCell(size_t x, size_t y, size_t /*posX*/, size_t /*posY*/) {
  gameGrid.at(y).at(x) = !gameGrid.at(y).at(x);
}

static Grid grid(2, 2, GRID_WIDTH, GRID_HEIGHT, 3, 1, RenderCell, OnToggleCell);

void POCGameView::OnEnter() {
  // Initialize game grid
  for (auto& row : gameGrid) {
    row.fill(false);
  }

  ForceRender();
}

void POCGameView::OnExit() {}

bool POCGameView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  grid.OnKeyPressed(keyDetails);
  if (keyDetails.key == ConsoleKey::Escape) {
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }
  return false;
}

void POCGameView::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

void POCGameView::ForceRender() {

  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();
  BoxDrawing::DrawBox(
      1,
      1,
      ((GRID_WIDTH + 1) * 5) + 2,
      ((GRID_HEIGHT + 1) * 2) + 2,
      BoxStyle::Single,
      true,
      "Game View"
  );
  grid.Render();
}

bool POCGameView::IsCorrectSize(int width, int height) const {
  const int requiredWidth = ((GRID_WIDTH + 1) * 5) + 4;
  const int requiredHeight = ((GRID_HEIGHT + 1) * 2) + 4;
  return width >= requiredWidth && height >= requiredHeight;
}
