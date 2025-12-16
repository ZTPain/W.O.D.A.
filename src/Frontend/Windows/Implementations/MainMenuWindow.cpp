#include "MainMenuWindow.h"

#include "../../Helpers/BoxDrawing.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/InputManager.h"
#include <cstdio>
#include <iostream>

void MainMenuWindow::OnEnter() {
  BoxDrawing::DrawBox(10, 5, 50, 15, BoxStyle::Rounded, false, "Main Menu");

  Draw();
}

void MainMenuWindow::OnExit() { printf("Exited Main Menu Window\n"); }

bool MainMenuWindow::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  switch (keyDetails.key) {
    case ConsoleKey::UpArrow:
      if (selectedIndex > 0) {
        --selectedIndex;
        DrawOptions();
      }
      return true;
    case ConsoleKey::DownArrow:
      if (selectedIndex + 1 < OPTIONS.size()) {
        ++selectedIndex;
        DrawOptions();
      }
      return true;
    case ConsoleKey::Enter:
      HandleSelection();
      return true;
    default:
      return false;
  }
}

void MainMenuWindow::Draw() const {
  BoxDrawing::DrawBox(10, 5, 50, 15, BoxStyle::Rounded, false, "Main Menu");

  DrawOptions();
}

void MainMenuWindow::DrawOptions() const {
  size_t i = 0;
  for (const auto var : OPTIONS) {
    DrawOption(i, var.data(), selectedIndex == i);
    ++i;
  }
}

void MainMenuWindow::DrawOption(size_t index, const char* text, bool selected) {
  auto x = 12;
  auto y = 7 + (index * 2);
  std::cout << MoveCursor(x, y);
  if (selected) {
    std::cout << "> " << text;
  } else {
    std::cout << "  " << text;
  }
}

void MainMenuWindow::HandleSelection() const {
  switch (selectedIndex) {
    case 0:
      std::cout << MoveCursor(0, 22) << "Starting game..." << '\n';
      break;
    case 1:
      std::cout << MoveCursor(0, 22) << "Opening settings..." << '\n';
      break;
    case 2:
      std::cout << MoveCursor(0, 22) << "Quitting..." << '\n';
      break;
    default:
      break;
  }
}
