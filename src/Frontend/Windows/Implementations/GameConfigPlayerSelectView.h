#pragma once

#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"

class GameConfigPlayerSelectView : public Window {
public:
  GameConfigPlayerSelectView() : Window(WindowType::GameConfigPlayersSelect) {};
  ~GameConfigPlayerSelectView() override = default;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;
};
