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

  std::cout << MoveCursor(0, 0);
  int x = -1;
  int y = -1;
  InputManager::GetCursorPosition(x, y);
  std::cout << MoveCursor(12, 7);
  printf("Welcome to the Main Menu!");
  std::cout << MoveCursor(12, 9);
  printf("Press 'Q' to quit.");
  std::cout << MoveCursor(x, y);
  printf("Entered Main Menu Window\n");
}

void MainMenuWindow::OnExit() { printf("Exited Main Menu Window\n"); }

void MainMenuWindow::OnKeyPressed(ConsoleKey key, ConsoleModifiers modifiers) {
  printf(
      "MainMenuWindow received key press: %d with modifiers: %d and char: %c\n",
      static_cast<int>(key),
      static_cast<int>(modifiers),
      static_cast<char>(key)
  );
}
