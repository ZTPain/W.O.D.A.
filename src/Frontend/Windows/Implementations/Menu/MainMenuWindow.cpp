#include "MainMenuWindow.h"

#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/AppHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <cstdio>
#include <iostream>

void MainMenuWindow::OnEnter() { ForceRender(); }

void MainMenuWindow::OnExit() { IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset(); }

bool MainMenuWindow::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    AppHelper::Exit();
    return true;
  }

  switch (keyDetails.key) {
    case ConsoleKey::W:
    case ConsoleKey::UpArrow:
      if (selectedIndex > 0) {
        --selectedIndex;
        DrawOptions();
      }
      return true;

    case ConsoleKey::S:
    case ConsoleKey::DownArrow:
      if (selectedIndex + 1 < OPTIONS.size()) {
        ++selectedIndex;
        DrawOptions();
      }
      return true;

    case ConsoleKey::Spacebar:
    case ConsoleKey::Enter:
      HandleSelection();
      return true;
    default:
      return false;
  }
}

void MainMenuWindow::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool MainMenuWindow::IsCorrectSize(int width, int height) const {
  const auto requiredWidth = 40;
  const auto requiredHeight = 12;
  return static_cast<size_t>(width) >= requiredWidth &&
         static_cast<size_t>(height) >= requiredHeight;
}

void MainMenuWindow::ForceRender() {
  BoxDrawing::DrawWindowFrame(true, "Main Menu");

  DrawOptions();
}

void MainMenuWindow::DrawOptions() const {
  size_t i = 0;
  for (const auto var : OPTIONS) {
    DrawOption(i, var.data(), selectedIndex == i);
    ++i;
  }

  IO::cout.flush();
}

void MainMenuWindow::DrawOption(size_t index, const char* text, bool selected) {
  auto x = 12;
  auto y = 3 + (index * 2);
  IO::cout << AnsiHelper::MoveCursor(x, y);
  if (selected) {
    IO::cout << AnsiHelper::SetTextColor(255, 128, 128) << "> " << text
             << AnsiHelper::SetTextColor(AnsiColor::Default);
  } else {
    IO::cout << "  " << text;
  }
}

void MainMenuWindow::HandleSelection() const {
  switch (selectedIndex) {
    case 0:
      WindowManager::GetInstance().SwitchToWindow(WindowType::GameConfigModeSelect);
      break;

    case 1:
      WindowManager::GetInstance().SwitchToWindow(WindowType::Settings);
      break;

    case 2:
      WindowManager::GetInstance().SwitchToWindow(WindowType::MatchHistory);
      break;

    case 3:
      WindowManager::GetInstance().SwitchToWindow(WindowType::UserProfile);
      break;

    case 4:
      WindowManager::GetInstance().SwitchToWindow(WindowType::UserSelect);
      break;

    case 5:
      AppHelper::Exit();
      break;

    default:
      break;
  }
}
