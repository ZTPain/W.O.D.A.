#pragma once

#include "../Api/Window.h"
#include "Frontend/Input/ConsoleKey.h"

class MainMenuWindow : public Window {
public:
  MainMenuWindow() : Window(WindowType::MainMenu) {}

protected:
  void OnEnter() override;
  void OnExit() override;
  void OnKeyPressed(ConsoleKey key, ConsoleModifiers modifiers) override;
};
