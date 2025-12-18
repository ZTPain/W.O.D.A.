#include "POCGameView.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <array>
#include <iostream>

POCGameView::POCGameView() : Window(WindowType::InGame) {}

POCGameView::~POCGameView() = default;

static constexpr int GRID_SIZE = 10;
static std::array<std::array<bool, GRID_SIZE>, GRID_SIZE> gameGrid;
static int cursorX = 0;
static int cursorY = 0;

static void Render() {
  for (int y = 0; y < GRID_SIZE; ++y) {
    for (int x = 0; x < GRID_SIZE; ++x) {
      if (x == cursorX && y == cursorY) {
        std::cout << (gameGrid.at(y).at(x) ? "[X]" : "[ ]");
      } else {
        std::cout << (gameGrid.at(y).at(x) ? " X " : " . ");
      }
    }
    std::cout << "\n";
  }
  std::cout << MoveCursor(0, 0);
  std::cout.flush();
}

void POCGameView::OnEnter() {
  // Initialize game grid
  for (auto& row : gameGrid) {
    row.fill(false);
  }
}

void POCGameView::OnExit() {}

bool POCGameView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  switch (keyDetails.key) {
    case ConsoleKey::UpArrow:
      cursorY = (cursorY > 0) ? cursorY - 1 : GRID_SIZE - 1;
      Render();
      return true;

    case ConsoleKey::DownArrow:
      cursorY = (cursorY + 1) % GRID_SIZE;
      Render();
      return true;

    case ConsoleKey::LeftArrow:
      cursorX = (cursorX > 0) ? cursorX - 1 : GRID_SIZE - 1;
      Render();
      return true;

    case ConsoleKey::RightArrow:
      cursorX = (cursorX + 1) % GRID_SIZE;
      Render();
      return true;

    default:
      break;
  }
  return false;
}
