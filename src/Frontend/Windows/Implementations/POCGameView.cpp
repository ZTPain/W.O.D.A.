#include "POCGameView.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/InteractiveGrid.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <array>
#include <cstddef>
#include <iostream>

POCGameView::POCGameView() : Window(WindowType::InGame) {}

POCGameView::~POCGameView() = default;

static constexpr int GRID_WIDTH = 22;
static constexpr int GRID_HEIGHT = 14;
static std::array<std::array<bool, GRID_WIDTH>, GRID_HEIGHT> gameGrid;

static void RenderCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) {
  std::cout << MoveCursor(posX, posY);
  if (isCursor) {
    std::cout << (gameGrid.at(y).at(x) ? "[X]" : "[ ]");
  } else {
    std::cout << (gameGrid.at(y).at(x) ? " X " : " . ");
  }
  std::cout.flush();
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

  grid.Subscribe();

  Render();
}

void POCGameView::OnExit() {}

bool POCGameView::OnKeyPressed(ConsoleKeyDetails /*keyDetails*/) { return false; }

void POCGameView::OnResize(int /*width*/, int /*height*/) { Render(); }

void POCGameView::Render() {

  std::cout << ANSI_CLEAR_SCREEN << ANSI_RESET;
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
