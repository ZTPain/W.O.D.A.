#pragma once

#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"

class UserSelectWindow : public Window {
public:
  UserSelectWindow() : Window(WindowType::UserSelect) {};
  ~UserSelectWindow() override = default;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;
};
