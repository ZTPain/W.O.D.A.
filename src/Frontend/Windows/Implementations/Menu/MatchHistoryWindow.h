#pragma once

#include "Backend/Replays/Replay.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <cstddef>

class MatchHistoryWindow : public Window {
public:
  MatchHistoryWindow();
  ~MatchHistoryWindow() override;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;

  size_t selectedReplayIndex = 0;
  void DrawOptions();

  void DrawReplay(size_t index, const Replay& replay) const;
  static void DrawReplayDetails(const Replay& replay);
};
