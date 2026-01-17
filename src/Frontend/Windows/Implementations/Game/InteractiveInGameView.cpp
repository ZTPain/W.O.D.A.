#include "InteractiveInGameView.h"

#include "Backend/Boards/GameBoard.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Computers/ComputerStrategyHelper.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/FireCommand.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/Player.h"
#include "Backend/Games/SalvoFireCommand.h"
#include "Frontend/Helpers/AppState.h"
#include "Frontend/Helpers/Grid.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/InputManager.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

bool InteractiveInGameView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
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

  const auto& gameManager = AppState::GetCurrentGameManager();

  if (gameManager->State() != GameState::Playing) {
    return false;
  }

  if (keyDetails.key == ConsoleKey::R) {
    const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);
    const auto& enemyPlayer = gameManager->GetPlayerAtIndex(enemyPlayerIndex);
    assert(currentPlayer.profile.AI() == nullptr);
    while (true) {
      const auto coords = GameManager::GetComputerByType(ComputerType::Medium)
                              ->GetFireCoordinates(enemyPlayer.board, salvoSelectionCoordinates);

      if (HandleFireAtCoordinate(coords))
        break;
    }
    return true;
  }

  const auto& players = gameManager->Players();

  switch (keyDetails.key) {
    case ConsoleKey::OemPeriod: // .
      fastForwardEnabled = !fastForwardEnabled;
      ForceRender();
      return true;

    case ConsoleKey::Oem4: // [
      enemyPlayerIndex = (enemyPlayerIndex + players.size() - 1) % players.size();
      while (enemyPlayerIndex == currentPlayerIndex ||
             gameManager->GetPlayerAtIndex(enemyPlayerIndex).board.IsGameOver()) {
        enemyPlayerIndex = (enemyPlayerIndex + players.size() - 1) % players.size();
      }
      ForceRender();
      return true;

    case ConsoleKey::Tab:
    case ConsoleKey::Oem6: // ]
      enemyPlayerIndex = (enemyPlayerIndex + 1) % players.size();
      while (enemyPlayerIndex == currentPlayerIndex ||
             gameManager->GetPlayerAtIndex(enemyPlayerIndex).board.IsGameOver()) {
        enemyPlayerIndex = (enemyPlayerIndex + 1) % players.size();
      }
      ForceRender();
      return true;

    default:
      break;
  }

  enemyGrid.OnKeyPressed(keyDetails);

  return false;
}

void InteractiveInGameView::HandleNextTurn() {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& currentPlayer = gameManager->GetCurrentPlayer();

  if (currentPlayer.profile.AI() != nullptr) {
    HandleAITurn();
    return;
  }

  // Human player's turn; wait for input
}

void InteractiveInGameView::HandleAITurn() {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& currentPlayer = gameManager->GetCurrentPlayer();
  const auto& players = gameManager->Players();

  assert(currentPlayer.profile.AI() != nullptr);

  // Select enemy player
  // For now, just the next player who is not defeated, so default to enemyPlayerIndex
  // If in the future we have more complex logic (like teams), this will need to be updated and we
  // will need to redraw the enemy grid accordingly

  enemyPlayerIndex = ComputerStrategyHelper::GetRandomFromRange(0, players.size() - 1);
  while (enemyPlayerIndex == currentPlayerIndex ||
         gameManager->GetPlayerAtIndex(enemyPlayerIndex).board.IsGameOver()) {
    enemyPlayerIndex = (enemyPlayerIndex + 1) % players.size();
  }

  const auto& enemyPlayer = gameManager->GetPlayerAtIndex(enemyPlayerIndex);

  size_t attempts = 0;
  while (true) {
    attempts++;
    if (attempts > 1000) {
      throw std::runtime_error("AI failed to make a valid move after 1000 attempts!");
    }

    const auto coord = currentPlayer.profile.AI()->GetFireCoordinates(
        enemyPlayer.board, salvoSelectionCoordinates
    );

    if (mode.commandType == FireCommandType::SalvoFireCommand) {
      // In salvo mode, ensure we don't select the same coordinate twice
      bool alreadySelected = false;
      for (const auto& selectedCoord : salvoSelectionCoordinates) {
        if (selectedCoord.x == coord.x && selectedCoord.y == coord.y) {
          alreadySelected = true;
          break;
        }
      }

      if (alreadySelected) {
        continue;
      }
    }

    if (HandleFireAtCoordinate(coord))
      break;
  }
}

void InteractiveInGameView::
    OnToggleEnemyCell(size_t x, size_t y, size_t /*posX*/, size_t /*posY*/) {
  const auto& mode = GetGameMode();

  const auto coord = Coordinates(static_cast<int>(x), static_cast<int>(y));

  lastPositionsPerPlayer[currentPlayerIndex] = {enemyPlayerIndex, coord};

  if (mode.commandType == FireCommandType::SalvoFireCommand) {
    // In salvo mode, ensure we don't select the same coordinate twice
    for (const auto& selectedCoord : salvoSelectionCoordinates) {
      if (selectedCoord.x == coord.x && selectedCoord.y == coord.y) {
        salvoSelectionCoordinates.erase(
            std::remove(
                salvoSelectionCoordinates.begin(), salvoSelectionCoordinates.end(), selectedCoord
            ),
            salvoSelectionCoordinates.end()
        );

        return;
      }
    }
  }

  HandleFireAtCoordinate(coord);
}

bool InteractiveInGameView::HandleFireAtCoordinate(const Coordinates& coord) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& currentPlayer = gameManager->GetCurrentPlayer();
  const auto& currentPlayerBoard = currentPlayer.board;
  const auto& currentPlayerUnits = currentPlayerBoard.GetAllUnits();
  const size_t currentPlayerUnitsAlive =
      std::count_if(currentPlayerUnits.begin(), currentPlayerUnits.end(), [](const auto& unit) {
        return !unit->IsDestroyed();
      });

  assert(enemyPlayerIndex != currentPlayerIndex);

  auto& enemyBoard = gameManager->GetPlayerAtIndex(enemyPlayerIndex).board;
  const auto& enemySegmentBoard = enemyBoard.GetSegmentBoard();
  const auto& enemySegments = enemySegmentBoard.Segments();
  const size_t nonShootSegmentsLeft =
      std::count_if(enemySegments.begin(), enemySegments.end(), [](const auto& row) {
        return std::count_if(row.begin(), row.end(), [](bool segment) { return !segment; });
      });

  switch (mode.commandType) {
    case FireCommandType::FireCommand: {
      auto command = std::make_unique<FireCommand>(enemyBoard, coord);
      if (!gameManager->ExecuteCommand(std::move(command), enemyPlayerIndex))
        return false;

      salvoSelectionCoordinates.push_back(coord);

      break;
    }

    case FireCommandType::SalvoFireCommand: {
      salvoSelectionCoordinates.push_back(coord);

      if (std::min(currentPlayerUnitsAlive, nonShootSegmentsLeft) ==
          salvoSelectionCoordinates.size()) {
        auto command = std::make_unique<SalvoFireCommand>(enemyBoard, salvoSelectionCoordinates);

        if (!gameManager->ExecuteCommand(std::move(command), enemyPlayerIndex)) {
          salvoSelectionCoordinates.clear();
          return false;
        }
      } else {
        enemyGrid.Render();
        return false;
      }
    }
  }

  enemyGrid.SetCursorPosition(coord.x, coord.y);
  HandleAfterFireAtCoordinate();
  return true;
}

const GameMode& InteractiveInGameView::GetGameMode() const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  return gameManager->Mode();
}

const std::vector<Player>& InteractiveInGameView::GetPlayers() const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  return gameManager->Players();
}

Player& InteractiveInGameView::GetPlayerAtIndex(size_t index) const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  return gameManager->GetPlayerAtIndex(static_cast<unsigned int>(index));
}

const Player& InteractiveInGameView::GetCurrentPlayer() const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  return gameManager->GetCurrentPlayer();
}

GameState InteractiveInGameView::GetGameState() const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  return gameManager->State();
}
