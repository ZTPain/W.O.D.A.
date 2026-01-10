#pragma once

#include "../Api/Window.h"
#include "Frontend/Input/InputManager.h"
#include <array>
#include <cstddef>
#include <string_view>

class MainMenuWindow : public Window {
public:
  MainMenuWindow() : Window(WindowType::MainMenu) {}

protected:
  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;

private:
  void Draw() const;
  void DrawOptions() const;
  static void DrawOption(size_t index, const char* text, bool selected);
  void HandleSelection() const;

  static constexpr std::array<std::string_view, 4> OPTIONS = {
      "Start Game", "Settings", "User Select", "Quit"
  };
  size_t selectedIndex = 0;
};
