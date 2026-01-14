#pragma once

#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

class GameOverView : public Window {
public:
  GameOverView() : Window(WindowType::PostGameSummary) {}
  ~GameOverView() override = default;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;

  size_t selectedStatIndex = 0;
  constexpr static size_t TOTAL_STATS = 6;
  constexpr static std::array<std::pair<const char*, bool>, TOTAL_STATS> STATS = {
      std::pair<const char*, bool>("Score", false),
      std::pair<const char*, bool>("Total Shots", false),
      std::pair<const char*, bool>("Total Hits", false),
      std::pair<const char*, bool>("Total Misses", true),
      std::pair<const char*, bool>("Accuracy", false),
      std::pair<const char*, bool>("Units Destroyed", false),
  };

  static void RenderWinnerSection();
  static void RenderStat(size_t index);
  [[nodiscard]] static uint64_t GetStatValue(size_t index, size_t playerIndex);
  static void RenderGeneralStats();
};
