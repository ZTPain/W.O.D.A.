#include "WindowManager.h"

#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <iostream>
#include <memory>

#include "Implementations/MainMenuWindow.h"
#include "Implementations/POCGameView.h"
#include "Implementations/UserSelectWindow.h"

void WindowManager::Initialize() {
  windows[WindowType::MainMenu] = std::make_unique<MainMenuWindow>();
  windows[WindowType::InGame] = std::make_unique<POCGameView>();
  windows[WindowType::UserSelect] = std::make_unique<UserSelectWindow>();

  InputManager::onKeyPressedProvider.Subscribe([this](ConsoleKeyDetails keyDetails) {
    OnKeyPressed(keyDetails);
    return false;
  });

  InputManager::onTerminalResizeProvider.Subscribe([this](int width, int height) {
    OnTerminalResize(width, height);
    return false;
  });

  SwitchToWindow(WindowType::UserSelect);
}

void WindowManager::SwitchToWindow(WindowType type) {
  if (currentWindowType != WindowType::None) {
    windows[currentWindowType]->Exit();
  }

  currentWindowType = type;
  windows[currentWindowType]->Enter();

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);
  if (!windows[currentWindowType]->IsCorrectSize(width, height)) {
    minSizeShown = true;
    ShowMinimumSizeMessage();
  } else {
    minSizeShown = false;
  }
}

void WindowManager::ShowMinimumSizeMessage() {
  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();
  BoxDrawing::DrawBox(1, 1, 50, 5, BoxStyle::Single, true, "Window Size Too Small");
  IO::cout << AnsiHelper::MoveCursor(3, 3) << "Please resize the terminal to a larger size."
           << '\n';
  IO::cout.flush();
}
