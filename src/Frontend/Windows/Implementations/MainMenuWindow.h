#pragma once

#include "../Api/Window.h"
#include "Frontend/Input/InputManager.h"

class MainMenuWindow : public Window {
public:
  MainMenuWindow() : Window(WindowType::MainMenu) {}

protected:
  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
};
