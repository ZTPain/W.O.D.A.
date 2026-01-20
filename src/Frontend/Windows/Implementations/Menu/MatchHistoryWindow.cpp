#include "MatchHistoryWindow.h"

#include "Backend/Replays/Replay.h"
#include "Backend/Replays/ReplayManager.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/AppState.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/ColorHelper.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <cstddef>
#include <ctime>

MatchHistoryWindow::MatchHistoryWindow() : Window(WindowType::MatchHistory) {}
MatchHistoryWindow::~MatchHistoryWindow() = default;

void MatchHistoryWindow::OnEnter() { ForceRender(); }
void MatchHistoryWindow::OnExit() { IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset(); }

bool MatchHistoryWindow::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }

  const auto& replays = ReplayManager::GetInstance().Replays();
  switch (keyDetails.key) {
    case ConsoleKey::W:
    case ConsoleKey::UpArrow:
      if (selectedReplayIndex > 0) {
        --selectedReplayIndex;
        ForceRender();
      }
      return true;

    case ConsoleKey::S:
    case ConsoleKey::DownArrow:
      if (selectedReplayIndex + 1 < replays.size()) {
        ++selectedReplayIndex;
        ForceRender();
      }
      return true;

    case ConsoleKey::Enter:
    case ConsoleKey::Spacebar: {
      if (replays.empty()) {
        return true;
      }

      if (AppState::IsReplayPlaybackSet()) {
        AppState::ClearCurrentReplayPlayback();
      }

      const auto playback =
          ReplayManager::GetInstance().Play(replays.at(selectedReplayIndex).replayId);
      AppState::SetCurrentReplayPlayback(playback);
      WindowManager::GetInstance().SwitchToWindow(WindowType::GameReplay);
      return true;
    }

    default:
      break;
  }

  return false;
}

void MatchHistoryWindow::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool MatchHistoryWindow::IsCorrectSize(int width, int height) const {
  return width >= 80 && height >= 20;
}

void MatchHistoryWindow::ForceRender() {
  IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset();

  BoxDrawing::DrawWindowFrame(true, "Match History");
  DrawOptions();

  IO::cout.flush();
}

void MatchHistoryWindow::DrawOptions() {
  const auto& replays = ReplayManager::GetInstance().Replays();

  IO::cout << AnsiHelper::MoveCursor(2, 2);
  IO::cout << "Total Matches: " << replays.size() << "\n\n";

  if (replays.empty()) {
    IO::cout << AnsiHelper::MoveCursor(4, 4);
    IO::cout << "No replays available.";
    return;
  }

  for (size_t i = 0; i < replays.size(); ++i) {
    DrawReplay(i, replays[i]);
  }

  DrawReplayDetails(replays.at(selectedReplayIndex));

  IO::cout << AnsiHelper::Reset();
}

void MatchHistoryWindow::DrawReplay(size_t index, const Replay& replay) const {
  IO::cout << AnsiHelper::MoveCursor(4, static_cast<int>(4 + (index * 2)));
  if (selectedReplayIndex == index) {
    IO::cout << AnsiHelper::SetColor(SELECTED_COLOR);
  }

  const auto& players = replay.players;
  const auto& winner = players[replay.winnerId];

  IO::cout << "Replay #" << replay.replayId << " - Players: " << replay.players.size()
           << " - Winner: " << winner.profile.name << "\n";

  if (selectedReplayIndex == index) {
    IO::cout << AnsiHelper::Reset();
  }
}

void MatchHistoryWindow::DrawReplayDetails(const Replay& replay) {
  IO::cout << AnsiHelper::MoveCursor(60, 2);
  IO::cout << "Replay Details:";

  IO::cout << AnsiHelper::MoveCursor(60, 4);
  IO::cout << "Replay ID: " << replay.replayId;
  IO::cout << AnsiHelper::MoveCursor(60, 5);
  IO::cout << "Players:";
  size_t i = 0;
  for (const auto& player : replay.players) {
    IO::cout << AnsiHelper::MoveCursor(60, 6 + static_cast<int>(i));
    IO::cout << " - " << player.profile.name;
    if (i == replay.winnerId)
      IO::cout << "   (🥇)";
    i++;
  }
  IO::cout << AnsiHelper::MoveCursor(60, 6 + static_cast<int>(i));
  IO::cout << "Winner: " << replay.players[replay.winnerId].profile.name;
  IO::cout << AnsiHelper::MoveCursor(60, 7 + static_cast<int>(i));
  IO::cout << "Playtime: " << replay.playtime.count() << " seconds";
  IO::cout << AnsiHelper::MoveCursor(60, 8 + static_cast<int>(i));
  IO::cout << "Timestamp: " << std::ctime(&replay.timestamp);
}
