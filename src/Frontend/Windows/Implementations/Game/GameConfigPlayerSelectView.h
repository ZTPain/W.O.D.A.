#pragma once

#include "Backend/Computers/Computer.h"
#include "Backend/Users/UserProfile.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <cstddef>
#include <vector>

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

  void RenderPlayerOptions() const;

  void RenderSelectedPlayerOption(size_t index, const UserProfile& player) const;
  void RenderUnselectedPlayerOption(size_t index, const UserProfile& player) const;

  void RenderAIAddOption(size_t index) const;
  void RenderAIAddOption(size_t index, ComputerType type) const;

  bool HandleInputMovement(ConsoleKeyDetails keyDetails);
  bool HandleInputSelection(ConsoleKeyDetails keyDetails);
  void HandleInputSelectionLeft();
  void HandleInputSelectionRight();

  size_t highlightedOptionIndex = 0;
  std::vector<PlayerId> selectedPlayerOptions;
};
