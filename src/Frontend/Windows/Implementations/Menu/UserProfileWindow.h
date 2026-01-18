#pragma once

#include "Backend/Users/AchievementPool.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <array>
#include <cstddef>
#include <string>

class UserProfileWindow : public Window {
public:
  UserProfileWindow() : Window(WindowType::UserProfile) {};
  ~UserProfileWindow() override = default;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;

  size_t selectedIndex = 0;

  constexpr static size_t STATISTICS_COUNT = 10;
  constexpr static std::array<const char*, STATISTICS_COUNT> STATISTIC_LABELS = {
      "Games Played",
      "Games Won",
      "Games Lost",
      "Highest Score",
      "Total Shots Fired",
      "Total Shots Hit",
      "Total Accuracy",
      "Total Units Destroyed",
      "Total Playtime",
      "Fastest Won Game"
  };

  constexpr static std::array<const char*, STATISTICS_COUNT> STATISTIC_DESCRIPTIONS = {
      "Total number of games played.",
      "Total number of games won.",
      "Total number of games lost.",
      "Highest score achieved in a single game.",
      "Total number of shots fired across all games.",
      "Total number of shots that hit a target across all games.",
      "Overall accuracy percentage across all games.",
      "Total number of enemy units destroyed across all games.",
      "Cumulative playtime across all games.",
      "Fastest time to win a game."
  };

  void RenderStatisticsSection() const;
  void RenderStatistic(size_t index, const std::string& value) const;

  constexpr static size_t UNLOCKED_CONTENT_COUNT = 10;
  constexpr static std::array<const char*, UNLOCKED_CONTENT_COUNT> UNLOCKED_CONTENT_LABELS = {
      "Medium Computer",
      "Hard Computer",

      "Salvo Mode",
      "Extended Mode",

      "Flower Ship Icon",
      "Crosshair Ship Icon",
      "Star Ship Icon",
      "Stone Ship Icon",

      "Red Border Color",
      "Blue Border Color"
  };
  void RenderUnlockedContentSection() const;
  void RenderUnlockedContent(size_t index, bool isUnlocked) const;

  void RenderAchievementsSection() const;
  void RenderAchievement(size_t index, const Achievement& achievement) const;

  static void RenderDetails(const std::string& title, const std::string& content);
};
