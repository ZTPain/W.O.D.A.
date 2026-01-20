#include "ReplayInGameView.h"

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/Player.h"
#include "Frontend/Helpers/AppState.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/InputManager.h"

#include <cassert>
#include <cstddef>
#include <vector>

bool ReplayInGameView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    HandleEscape();
    return true;
  }

  if (keyDetails.key == ConsoleKey::OemPeriod) {
    if (keyDetails.modifiers == ConsoleModifiers::Shift) {
      ultraFastForwardEnabled = !ultraFastForwardEnabled;
    } else {
      fastForwardEnabled = !fastForwardEnabled;
      ultraFastForwardEnabled = false;
    }
    ForceRender();
    return true;
  }
  auto& playback = AppState::GetCurrentReplayPlayback();

  switch (keyDetails.key) {
    case ConsoleKey::A:
    case ConsoleKey::LeftArrow:
      playback.StepBackward();
      ForceRender();
      return true;

    case ConsoleKey::D:
    case ConsoleKey::RightArrow:
      playback.StepForward();
      ForceRender();
      return true;

    default:
      break;
  }

  return false;
}

void ReplayInGameView::HandleNextTurn() {}

void ReplayInGameView::OnToggleEnemyCell(
    size_t /*x*/, size_t /*y*/, size_t /*posX*/, size_t /*posY*/
) {
  // No-op in replay view
}

const GameMode& ReplayInGameView::GetGameMode() const {
  return AppState::GetCurrentReplayPlayback().Replay().mode;
}

const std::vector<Player>& ReplayInGameView::GetPlayers() const {
  return AppState::GetCurrentReplayPlayback().Replay().players;
}

const Player& ReplayInGameView::GetPlayerAtIndex(size_t index) const {
  const auto& playback = AppState::GetCurrentReplayPlayback();
  assert(index < playback.Replay().players.size());
  return playback.Replay().players[index];
}

const Player& ReplayInGameView::GetCurrentPlayer() const {
  const auto& playback = AppState::GetCurrentReplayPlayback();
  const auto currentPlayerIndex = playback.CurrentPlayerIndex();
  assert(
      currentPlayerIndex >= 0 &&
      static_cast<size_t>(currentPlayerIndex) < playback.Replay().players.size()
  );
  return playback.Replay().players.at(static_cast<size_t>(currentPlayerIndex));
}

GameState ReplayInGameView::GetGameState() const {
  const auto& playback = AppState::GetCurrentReplayPlayback();
  if (playback.CurrentStep() >=
      static_cast<int>(AppState::GetCurrentReplayPlayback().Replay().history.size())) {
    return GameState::Over;
  }

  return GameState::Playing;
}
