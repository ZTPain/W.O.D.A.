#include "UserProfileWindow.h"

#include "Backend/Users/AchievementPool.h"
#include "Backend/Users/UserManager.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/ColorHelper.h"
#include "Frontend/Helpers/TextHelper.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <cassert>
#include <cstddef>
#include <string>

void UserProfileWindow::OnEnter() { ForceRender(); }

void UserProfileWindow::OnExit() { IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset(); }

void UserProfileWindow::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool UserProfileWindow::IsCorrectSize(int width, int height) const {
  return width >= 80 && height >= 24;
}

bool UserProfileWindow::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }

  const size_t optionsCount =
      STATISTICS_COUNT + UNLOCKED_CONTENT_COUNT +
      UserManager::GetInstance().GetCurrentUser().achievements->NameToAchievementMap().size();

  switch (keyDetails.key) {
    case ConsoleKey::UpArrow:
    case ConsoleKey::W:
      if (selectedIndex > 0) {
        selectedIndex--;
        ForceRender();
      }
      return true;

    case ConsoleKey::DownArrow:
    case ConsoleKey::S:
      if (selectedIndex + 1 < optionsCount) {
        selectedIndex++;
        ForceRender();
      }
      return true;

    default:
      break;
  }

  return false;
}

void UserProfileWindow::ForceRender() {
  IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset();

  BoxDrawing::DrawWindowFrame(true, "User Profile");

  const auto& currentUser = UserManager::GetInstance().GetCurrentUser();

  IO::cout << AnsiHelper::MoveCursor(3, 3);
  IO::cout << "UserId: " << currentUser.UserId();

  IO::cout << AnsiHelper::MoveCursor(3, 4);
  IO::cout << "Username: " << currentUser.name;

  RenderStatisticsSection();

  RenderUnlockedContentSection();

  RenderAchievementsSection();

  IO::cout.flush();
}

void UserProfileWindow::RenderStatisticsSection() const {
  if (selectedIndex >= STATISTICS_COUNT) {
    IO::cout << AnsiHelper::MoveCursor(3, 6);
    IO::cout << "Statistics";
    return;
  }

  const auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  IO::cout << AnsiHelper::MoveCursor(5, 6);
  IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::BrightBlue)
           << AnsiHelper::SetTextColor(AnsiColor::White);
  IO::cout << "Statistics:";
  IO::cout << AnsiHelper::Reset();
  RenderStatistic(0, std::to_string(currentUser.statistics.gamesPlayed));
  RenderStatistic(1, std::to_string(currentUser.statistics.gamesWon));
  RenderStatistic(2, std::to_string(currentUser.statistics.gamesLost));
  RenderStatistic(3, std::to_string(currentUser.statistics.highestScore));
  RenderStatistic(4, std::to_string(currentUser.statistics.totalShotsFired));
  RenderStatistic(5, std::to_string(currentUser.statistics.totalShotsHit));
  RenderStatistic(
      6,
      std::to_string(
          currentUser.statistics.totalShotsHit * 100.0 / currentUser.statistics.totalShotsFired
      ) + "%"
  );
  RenderStatistic(7, std::to_string(currentUser.statistics.totalUnitsDestroyed));
  RenderStatistic(8, std::to_string(currentUser.statistics.totalPlaytime.count()) + " seconds");
  RenderStatistic(9, std::to_string(currentUser.statistics.fastestWonGame.count()) + " seconds");
}

void UserProfileWindow::RenderStatistic(size_t index, const std::string& value) const {
  assert(index < STATISTICS_COUNT);

  IO::cout << AnsiHelper::MoveCursor(7, 7 + index);

  if (selectedIndex == index) {
    IO::cout << AnsiHelper::SetColor(SELECTED_COLOR);
  }

  IO::cout << STATISTIC_LABELS.at(index) << ": " << value;

  if (selectedIndex == index) {
    IO::cout << AnsiHelper::Reset();

    RenderDetails("Statistic Details", STATISTIC_DESCRIPTIONS.at(index));
  }
}

void UserProfileWindow::RenderUnlockedContentSection() const {
  const size_t startIndex = STATISTICS_COUNT;
  if (selectedIndex < startIndex) {
    IO::cout << AnsiHelper::MoveCursor(3, 7 + STATISTICS_COUNT);
    IO::cout << "Unlocked Content";
    return;
  }

  if (selectedIndex >= startIndex + UNLOCKED_CONTENT_COUNT) {
    IO::cout << AnsiHelper::MoveCursor(3, 7);
    IO::cout << "Unlocked Content";
    return;
  }

  IO::cout << AnsiHelper::MoveCursor(5, 7);
  IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::BrightBlue)
           << AnsiHelper::SetTextColor(AnsiColor::White);
  IO::cout << "Unlocked Content:";
  IO::cout << AnsiHelper::Reset();

  const auto& currentUser = UserManager::GetInstance().GetCurrentUser();

  for (size_t i = 0; i < UNLOCKED_CONTENT_COUNT; ++i) {
    RenderUnlockedContent(i, currentUser.unlockedContent & static_cast<UnlockableContent>(1 << i));
  }
}

void UserProfileWindow::RenderUnlockedContent(size_t index, bool isUnlocked) const {
  IO::cout << AnsiHelper::MoveCursor(7, 8 + index);

  if (selectedIndex == STATISTICS_COUNT + index) {
    IO::cout << AnsiHelper::SetColor(SELECTED_COLOR);
  }

  const std::string contentStatus =
      (isUnlocked ? AnsiHelper::SetTextColor(AnsiColor::Green) + "Unlocked"
                  : AnsiHelper::SetTextColor(AnsiColor::Red) + "Locked") +
      AnsiHelper::SetTextColor(AnsiColor::Default);
  IO::cout << UNLOCKED_CONTENT_LABELS.at(index) << " [" << contentStatus << "]";

  if (selectedIndex == STATISTICS_COUNT + index) {
    IO::cout << AnsiHelper::Reset();
  }
}

void UserProfileWindow::RenderAchievementsSection() const {
  const auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  const auto& achievements = currentUser.achievements->NameToAchievementMap();

  const size_t startIndex = STATISTICS_COUNT + UNLOCKED_CONTENT_COUNT;
  if (selectedIndex < startIndex - UNLOCKED_CONTENT_COUNT) {
    IO::cout << AnsiHelper::MoveCursor(3, 8 + STATISTICS_COUNT);
    IO::cout << "Achievements";
    return;
  }

  if (selectedIndex < startIndex) {
    IO::cout << AnsiHelper::MoveCursor(3, 8 + UNLOCKED_CONTENT_COUNT);
    IO::cout << "Achievements";
    return;
  }

  if (selectedIndex >= startIndex + achievements.size()) {
    IO::cout << AnsiHelper::MoveCursor(3, 8);
    IO::cout << "Achievements";
    return;
  }

  IO::cout << AnsiHelper::MoveCursor(5, 8);
  IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::BrightBlue)
           << AnsiHelper::SetTextColor(AnsiColor::White);
  IO::cout << "Achievements:";
  IO::cout << AnsiHelper::Reset();

  size_t i = 0;
  for (const auto& [name, achievement] : achievements) {
    RenderAchievement(i, achievement);
    i++;
  }
}

void UserProfileWindow::RenderAchievement(size_t index, const Achievement& achievement) const {
  IO::cout << AnsiHelper::MoveCursor(7, 9 + index);

  if (selectedIndex == STATISTICS_COUNT + UNLOCKED_CONTENT_COUNT + index) {
    IO::cout << AnsiHelper::SetColor(SELECTED_COLOR);
  }

  const std::string achievementStatus =
      (achievement.unlocked ? AnsiHelper::SetTextColor(AnsiColor::Green) + "Unlocked"
                            : AnsiHelper::SetTextColor(AnsiColor::Red) + "Locked") +
      AnsiHelper::SetTextColor(AnsiColor::Default);
  const std::string achievementTitle = achievement.name + " [" + achievementStatus + "]";

  IO::cout << "- " << achievementTitle;

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  if (selectedIndex == STATISTICS_COUNT + UNLOCKED_CONTENT_COUNT + index) {
    IO::cout << AnsiHelper::Reset();

    RenderDetails("Achievement Description", achievement.description);
  }
}

void UserProfileWindow::RenderDetails(const std::string& title, const std::string& content) {
  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  size_t tmp = 0;
  const auto contentLines =
      TextHelper::CalculateWrappedText(0, 0, width - 60 - 8, content.c_str(), tmp);

  BoxDrawing::DrawBox(
      60, 9, width - 60 - 4, contentLines + 2, BoxStyle::Rounded, true, title.c_str()
  );

  TextHelper::DrawWrappedText(62, 10, width - 60 - 8, content.c_str());
}
