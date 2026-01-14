#include "WindowManager.h"

#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <iostream>
#include <memory>

#include "Implementations/Game/GameConfigModeSelectView.h"
#include "Implementations/Game/GameConfigPlayerSelectView.h"
#include "Implementations/Game/GameSetupView.h"
#include "Implementations/Game/InGameView.h"
#include "Implementations/Menu/MainMenuWindow.h"
#include "Implementations/Menu/UserSelectWindow.h"

void WindowManager::Initialize() {
  windows[WindowType::MainMenu] = std::make_unique<MainMenuWindow>();
  windows[WindowType::GameSetup] = std::make_unique<GameSetupView>();
  windows[WindowType::UserSelect] = std::make_unique<UserSelectWindow>();
  windows[WindowType::GameConfigModeSelect] = std::make_unique<GameConfigModeSelectView>();
  windows[WindowType::GameConfigPlayersSelect] = std::make_unique<GameConfigPlayerSelectView>();
  windows[WindowType::InGame] = std::make_unique<InGameView>();

  InputManager::onKeyPressedProvider.Subscribe([this](ConsoleKeyDetails keyDetails) {
    if (keyDetails.key == ConsoleKey::R && keyDetails.modifiers == ConsoleModifiers::Control) {
      // Ctrl+R pressed - force redraw current window
      if (currentWindowType != WindowType::None) {
        windows[currentWindowType]->ForceRender();
      }
      return true;
    }

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
