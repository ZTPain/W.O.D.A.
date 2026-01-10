#pragma once

#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <cstddef>

class GameConfigModeSelectView : public Window {
public:
  GameConfigModeSelectView() : Window(WindowType::GameConfigModeSelect) {};
  ~GameConfigModeSelectView() override = default;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;
  static void DrawOptions();
  static void DrawOption(size_t index);
};
