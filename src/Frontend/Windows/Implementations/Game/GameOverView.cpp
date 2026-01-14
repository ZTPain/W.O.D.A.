#include "GameOverView.h"

#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/AppState.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/TextHelper.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <utility>
#include <vector>

void GameOverView::OnEnter() { ForceRender(); }

void GameOverView::OnExit() {
  IO::cout << AnsiHelper::Reset();
  AppState::Reset();
}

bool GameOverView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape || keyDetails.key == ConsoleKey::Enter ||
      keyDetails.key == ConsoleKey::Spacebar) {
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }

  switch (keyDetails.key) {
    case ConsoleKey::A:
    case ConsoleKey::LeftArrow:
      if (selectedStatIndex == 0) {
        selectedStatIndex = TOTAL_STATS - 1;
      } else {
        selectedStatIndex--;
      }
      ForceRender();
      return true;

    case ConsoleKey::D:
    case ConsoleKey::RightArrow:
      selectedStatIndex = (selectedStatIndex + 1) % TOTAL_STATS;
      ForceRender();
      return true;

    default:
      return false;
  }
}

void GameOverView::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool GameOverView::IsCorrectSize(int /*width*/, int /*height*/) const { return true; }

void GameOverView::ForceRender() {
  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();

  BoxDrawing::DrawWindowFrame(true, "Game Over");

  RenderWinnerSection();
  RenderGeneralStats();

  RenderStat(selectedStatIndex);

  IO::cout.flush();
}

void GameOverView::RenderWinnerSection() {
  const auto startY = 5;

  const auto& gameManager = AppState::GetCurrentGameManager();

  const auto& winner = gameManager->Players().at(gameManager->WinnerId());

  static std::array<char, 100> buffer{};
  std::snprintf(
      buffer.data(),
      buffer.size(),
      "\x1B[1;38;2;255;215;0m%s\x1B[0m has won the game!",
      winner.profile.name.c_str()
  );

  TextHelper::DrawCenteredText(startY, buffer.data());
}

void GameOverView::RenderStat(size_t index) {
  const auto startY = 8;

  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& players = gameManager->Players();

  const auto& stat = STATS.at(index);

  IO::cout << AnsiHelper::MoveCursor(10, startY);
  IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
  IO::cout << stat.first << ": ";
  IO::cout << AnsiHelper::Reset();

  std::vector<uint64_t> statValues(players.size(), 0);
  for (size_t i = 0; i < players.size(); ++i) {
    statValues[i] = GetStatValue(index, i);
  }

  std::vector<std::pair<size_t, uint64_t>> sortedStatValues(players.size());
  for (size_t i = 0; i < statValues.size(); ++i) {
    sortedStatValues[i] = {i, statValues[i]};
  }

  std::sort(sortedStatValues.begin(), sortedStatValues.end(), [&](const auto& a, const auto& b) {
    if (stat.second) {
      return a.second < b.second;
    }

    return a.second > b.second;
  });

  size_t i = 1;
  for (const auto& [playerIndex, value] : sortedStatValues) {
    const auto& player = players.at(playerIndex);

    IO::cout << AnsiHelper::MoveCursor(10, startY + 2 + i);
    if (playerIndex == gameManager->WinnerId()) {
      IO::cout << AnsiHelper::SetTextColor(255, 215, 0);
    } else {
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
    }

    IO::cout << "[" << i << "] " << player.profile.name << ": " << value;
    IO::cout << AnsiHelper::Reset();
    i++;
  }
}

uint64_t GameOverView::GetStatValue(size_t index, size_t playerIndex) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& player = gameManager->Players().at(playerIndex);

  switch (index) {
    case 0:
      return player.score;

    case 1:
      return player.shotsFired;

    case 2:
      return player.shotsHit;

    case 3:
      return player.shotsFired - player.shotsHit;

    case 4: {
      const auto totalShots = player.shotsFired;
      if (totalShots == 0)
        return 0;

      const auto totalHits = player.shotsHit;
      return static_cast<uint64_t>((totalHits * 100) / totalShots);
    }

    case 5:
      return player.unitsDestroyed;

    default:
      return 0;
  }
}

void GameOverView::RenderGeneralStats() {
  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  const auto startY = 7;
  const auto startX = width - 30;

  const auto& gameManager = AppState::GetCurrentGameManager();

  IO::cout << AnsiHelper::MoveCursor(startX, startY);
  IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
  IO::cout << "Total Turns: ";
  IO::cout << AnsiHelper::Reset();
  IO::cout << AnsiHelper::MoveCursor(startX + 20, startY);
  IO::cout << GetStatValue(1, gameManager->WinnerId());

  IO::cout << AnsiHelper::MoveCursor(startX, startY + 2);
  IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
  IO::cout << "Total Playtime: ";
  IO::cout << AnsiHelper::Reset();
  IO::cout << AnsiHelper::MoveCursor(startX + 20, startY + 2);
  std::array<char, 10> timeBuffer{};
  TextHelper::FormatDuration(
      timeBuffer.data(), timeBuffer.size(), std::chrono::seconds(gameManager->Playtime())
  );
  IO::cout << timeBuffer.data();
}
