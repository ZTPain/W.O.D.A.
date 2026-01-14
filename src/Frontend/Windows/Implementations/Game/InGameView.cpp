#include "InGameView.h"

#include "Backend/Boards/GameBoard.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/FireCommand.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/BattleUnitHelper.h"
#include "Backend/Units/BattleUnitType.h"
#include "Frontend/Helpers//PromptHelper.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/AppState.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/Grid.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <map>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

void InGameView::SetGridOffsets() {
  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  if (invertGridPositions) {
    enemyGrid.SetOffset(2 + 2, 8);
    currentGrid.SetOffset(width - enemyGrid.GetTotalWidth() - enemyGrid.GetXOffset(), 8);
  } else {
    currentGrid.SetOffset(2 + 2, 8);
    enemyGrid.SetOffset(width - currentGrid.GetTotalWidth() - currentGrid.GetXOffset(), 8);
  }
}

void InGameView::OnEnter() {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  currentPlayerIndex = 0;
  enemyPlayerIndex = 1;

  currentGrid = Grid(
      2 + 2,
      8,
      mode.boardWidth,
      mode.boardHeight,
      3,
      1,
      [this](size_t x, size_t y, size_t posX, size_t posY, bool /*isCursor*/) {
        RenderCell(x, y, posX, posY, false, currentPlayerIndex);
      },
      nullptr
  );

  enemyGrid = Grid(
      width - currentGrid.GetTotalWidth() - currentGrid.GetXOffset(),
      8,
      mode.boardWidth,
      mode.boardHeight,
      3,
      1,
      [this](size_t x, size_t y, size_t posX, size_t posY, bool isCursor) {
        RenderCell(x, y, posX, posY, isCursor, enemyPlayerIndex);
      },
      [this](size_t x, size_t y, size_t posX, size_t posY) { OnToggleEnemyCell(x, y, posX, posY); }
  );

  actionHistory.clear();
  invertGridPositions = false;

  OnChangeTurn();
}

void InGameView::OnExit() { IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset(); }

bool InGameView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    // Prompt to confirm exit to main menu
    PromptHelper::ShowYesNoPrompt(
        "Are you sure you want to exit to the main menu?", [](bool result) {
          if (result) {
            AppState::Reset();
            WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
          } else {
            WindowManager::GetInstance().GetCurrentWindow()->ForceRender();
          }
        }
    );
    return true;
  }

  const auto& gameManager = AppState::GetCurrentGameManager();

  if (gameManager->State() != GameState::Playing) {
    return false;
  }

  if (keyDetails.key == ConsoleKey::R) {
    const auto& currentPlayer = gameManager->Players().at(currentPlayerIndex);
    const auto& enemyPlayer = gameManager->Players().at(enemyPlayerIndex);
    assert(currentPlayer.profile.AI() == nullptr);
    const auto coords =
        GameManager::GetComputerByType(ComputerType::Medium)->GetFireCoordinates(enemyPlayer.board);

    HandleFireAtCoordinate(coords);
    return true;
  }

  enemyGrid.OnKeyPressed(keyDetails);

  return false;
}

void InGameView::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool InGameView::IsCorrectSize(int /*width*/, int /*height*/) const { return true; }

void InGameView::ForceRender() {
  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();

  BoxDrawing::DrawWindowFrame(true, "In-Game View");

  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();

  const auto& currentPlayer = gameManager->Players().at(currentPlayerIndex);
  const auto& enemyPlayer = gameManager->Players().at(enemyPlayerIndex);

  const auto alivePlayersCount = std::count_if(
      gameManager->Players().begin(), gameManager->Players().end(), [](const auto& player) {
        return !player.board.IsGameOver();
      }
  );

  bool shouldInvert = false;
  if (alivePlayersCount == 2) {
    shouldInvert = currentPlayerIndex != 0;
  } else if (alivePlayersCount > 2) {
    shouldInvert = currentPlayer.profile.AI() != nullptr && enemyPlayer.profile.AI() == nullptr;
  }

  if (!invertGridPositions && shouldInvert) {
    invertGridPositions = true;
    SetGridOffsets();
  } else if (invertGridPositions && !shouldInvert) {
    invertGridPositions = false;
    SetGridOffsets();
  }

  std::array<char, 100> titleBuffer{};
  std::snprintf(
      titleBuffer.data(),
      titleBuffer.size(),
      "%s's board%s",
      currentPlayer.profile.name.c_str(),
      (currentPlayer.profile.AI() != nullptr ? " (AI)" : "")
  );

  BoxDrawing::DrawBox(
      currentGrid.GetXOffset() - 1,
      currentGrid.GetYOffset() - 1,
      ((mode.boardWidth + 1) * currentGrid.GetCellWidthWithBorders()) - 1,
      ((mode.boardHeight + 1) * currentGrid.GetCellHeightWithBorders()) + 1,
      BoxStyle::Single,
      true,
      titleBuffer.data()
  );

  currentGrid.Render();

  std::snprintf(
      titleBuffer.data(),
      titleBuffer.size(),
      "%s's board%s",
      enemyPlayer.profile.name.c_str(),
      (enemyPlayer.profile.AI() != nullptr ? " (AI)" : "")
  );

  BoxDrawing::DrawBox(
      enemyGrid.GetXOffset() - 1,
      enemyGrid.GetYOffset() - 1,
      ((mode.boardWidth + 1) * enemyGrid.GetCellWidthWithBorders()) - 1,
      ((mode.boardHeight + 1) * enemyGrid.GetCellHeightWithBorders()) + 1,
      BoxStyle::Single,
      true,
      titleBuffer.data()
  );

  enemyGrid.Render();

  RenderUnitsLeft();
  RenderTurnQueue();
  RenderLeaderboard();
  RenderHistory();

  IO::cout.flush();
}

// Justification: This is recursive only if the next player is AI
// In theory this could lead to stack overflow if there are only AI players
// Will probably need to be reworked in the future to avoid that case
// NOLINTNEXTLINE(misc-no-recursion)
void InGameView::OnChangeTurn() {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& players = gameManager->Players();
  const auto& currentPlayer = gameManager->GetCurrentPlayer();

  const auto pred = [&currentPlayer](const auto& player) {
    return player.profile.UserId() == currentPlayer.profile.UserId();
  };

  currentPlayerIndex = std::find_if(players.begin(), players.end(), pred) - players.begin();

  enemyPlayerIndex = (currentPlayerIndex + 1) % players.size();
  while (players[enemyPlayerIndex].board.IsGameOver()) {
    enemyPlayerIndex = (enemyPlayerIndex + 1) % players.size();
  }

  if (currentPlayerIndex == enemyPlayerIndex) {
    throw std::runtime_error("No valid enemy player found!");
  }

  enemyGrid.SetCursorPosition(0, 0);

  ForceRender();

  if (currentPlayer.profile.AI() != nullptr) {
    HandleAITurn();
  }
}

void InGameView::RenderCell(
    size_t x, size_t y, size_t posX, size_t posY, bool isCursor, size_t playerIndex
) const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& player = gameManager->Players().at(playerIndex);
  const auto& board = player.board;
  const auto& segmentBoard = board.GetSegmentBoard();
  const auto& unitsPlacement = board.Units();

  const Coordinates coord(static_cast<int>(x), static_cast<int>(y));
  const bool isHit = segmentBoard.Segments()[y][x];
  const bool hasUnit = unitsPlacement[y][x] != nullptr;

  IO::cout << AnsiHelper::MoveCursor(posX, posY);

  std::array<char, 4> symbol = {' ', '.', ' ', '\0'};

  if (isCursor) {
    symbol[0] = '[';
    symbol[1] = ' ';
    symbol[2] = ']';
  }

  if (isHit && hasUnit) {
    if (unitsPlacement[y][x]->IsDestroyed()) {
      symbol[1] = 'D';
    } else {
      symbol[1] = 'X';
    }
  } else if (isHit && !hasUnit) {
    symbol[1] = 'o';
  }

  IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Default);

  if (inAnimation && isCursor) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::White);
    symbol[0] = '[';
    symbol[1] = ' ';
    symbol[2] = ']';
  }

  if (isHit && hasUnit) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red) << symbol.data() << AnsiHelper::Reset();
  } else if (isHit && !hasUnit) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Blue) << symbol.data() << AnsiHelper::Reset();
  } else {
    IO::cout << symbol.data();
  }

  IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Default);
  IO::cout.flush();
}

void InGameView::OnToggleEnemyCell(size_t x, size_t y, size_t /*posX*/, size_t /*posY*/) {
  const auto coord = Coordinates(static_cast<int>(x), static_cast<int>(y));
  HandleFireAtCoordinate(coord);
}

size_t InGameView::CalculateSegmentsLeftForPlayer(size_t playerIndex, size_t* outMax) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& player = gameManager->Players().at(playerIndex);
  const auto& units = player.board.GetAllUnits();

  const auto maxCount =
      std::accumulate(mode.unitPool.begin(), mode.unitPool.end(), 0, [](int sum, const auto& pair) {
        return sum + (BattleUnitHelper::GetSizeForUnitType(pair.first) * pair.second);
      });
  const auto totalSegmentsLeft =
      maxCount - std::accumulate(units.begin(), units.end(), 0, [](int sum, const auto& pair) {
        return sum + pair->GetDestroyedSegments();
      });

  if (outMax != nullptr) {
    *outMax = maxCount;
  }

  return totalSegmentsLeft;
}

void InGameView::RenderUnitsLeftForPlayer(size_t playerIndex, size_t startX, size_t startY) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& player = gameManager->Players().at(playerIndex);
  const auto& units = player.board.GetAllUnits();

  IO::cout << AnsiHelper::MoveCursor(startX, startY);
  IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);

  IO::cout << "Units Left: ";
  std::map<BattleUnitType, int> unitCount{};
  for (const auto& [unitType, _] : mode.unitPool) {
    unitCount[unitType] = 0;
  }
  for (const auto& unit : units) {
    if (unit->IsDestroyed())
      continue;

    unitCount[unit->GetType()]++;
  }

  const auto halfCount = (mode.unitPool.size() + 1) / 2;

  size_t maxCount = 0;
  const auto segmentsLeft = CalculateSegmentsLeftForPlayer(playerIndex, &maxCount);

  size_t i = 0;
  size_t j = 0;
  IO::cout << AnsiHelper::MoveCursor(startX + 20, startY);
  IO::cout << "Total segments: " << segmentsLeft << "/" << maxCount << " ";
  for (const auto& [unitType, count] : unitCount) {
    const auto maxCount = mode.unitPool.at(unitType);
    IO::cout << AnsiHelper::MoveCursor(startX + (j * 20), startY + i + 1);
    if (count == 0) {
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::BrightBlack);
    } else {
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
    }
    IO::cout << BattleUnitHelper::GetNameForUnitType(unitType) << ": " << count << "/" << maxCount
             << " ";

    i++;
    if (i >= halfCount) {
      i = 0;
      j++;
    }
  }

  IO::cout << AnsiHelper::Reset();
}

void InGameView::RenderUnitsLeft() const {
  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  RenderUnitsLeftForPlayer(currentPlayerIndex, currentGrid.GetXOffset(), 2);
  RenderUnitsLeftForPlayer(enemyPlayerIndex, enemyGrid.GetXOffset(), 2);
}

// Justification: This is recursive only if the next player is AI
// In theory this could lead to stack overflow if there are only AI players
// Will probably need to be reworked in the future to avoid that case
// NOLINTNEXTLINE(misc-no-recursion)
void InGameView::HandleAITurn() {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& currentPlayer = gameManager->GetCurrentPlayer();

  assert(currentPlayer.profile.AI() != nullptr);

  // Select enemy player
  // For now, just the next player who is not defeated, so default to enemyPlayerIndex
  // If in the future we have more complex logic (like teams), this will need to be updated and we
  // will need to redraw the enemy grid accordingly

  const auto& enemyPlayer = gameManager->Players().at(enemyPlayerIndex);
  const auto& enemyBoard = enemyPlayer.board;

  size_t attempts = 0;
  while (true) {
    attempts++;
    if (attempts > 1000) {
      throw std::runtime_error("AI failed to make a valid move after 1000 attempts!");
    }

    switch (mode.commandType) {
      case FireCommandType::FireCommand: {
        const auto coord = currentPlayer.profile.AI()->GetFireCoordinates(enemyBoard);
        if (!HandleFireAtCoordinate(coord))
          continue;
        break;
      }

      case FireCommandType::SalvoFireCommand: {
        throw std::runtime_error("SalvoFireCommand not implemented in InGameView! (Yet)");
      }
    }

    break;
  }
}

// Justification: This is recursive only if the next player is AI
// In theory this could lead to stack overflow if there are only AI players
// Will probably need to be reworked in the future to avoid that case
// NOLINTNEXTLINE(misc-no-recursion)
bool InGameView::HandleFireAtCoordinate(const Coordinates& coord) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& enemyPlayer = gameManager->Players().at(enemyPlayerIndex);
  const auto& enemyBoard = enemyPlayer.board;

  assert(enemyPlayerIndex != currentPlayerIndex);

  switch (mode.commandType) {
    case FireCommandType::FireCommand: {
      // NOLINTNEXTLINE
      auto command = std::make_unique<FireCommand>(const_cast<GameBoard&>(enemyBoard), coord);
      if (!gameManager->ExecuteCommand(std::move(command)))
        return false;

      break;
    }

    case FireCommandType::SalvoFireCommand: {
      throw std::runtime_error("SalvoFireCommand not implemented in InGameView! (Yet)");
    }
  }

  uint8_t result = 0;
  if (enemyBoard.Units()[coord.y][coord.x] != nullptr) {
    if (enemyBoard.Units()[coord.y][coord.x]->IsDestroyed()) {
      result = 2; // Destroyed
    } else {
      result = 1; // Hit
    }
  } else {
    result = 0; // Miss
  }
  actionHistory.emplace_back(currentPlayerIndex, enemyPlayerIndex, coord, result);

  enemyGrid.SetCursorPosition(coord.x, coord.y);

  ShowPlayerFireAnimation(coord);

  if (gameManager->State() == GameState::Over) {
    // Show game over prompt
    HandleGameOver();
    return true;
  }

  OnChangeTurn();
  return true;
}

void InGameView::ShowPlayerFireAnimation(const Coordinates& coord) {
  inAnimation = true;

  auto bulletY = enemyGrid.GetYOffset() + (coord.y * enemyGrid.GetCellHeightWithBorders()) +
                 (enemyGrid.GetCellHeightWithBorders() / 2) + 1;
  auto bulletX = currentGrid.GetTotalWidth() + currentGrid.GetXOffset() + 1;

  auto targetX = enemyGrid.GetXOffset() + (coord.x * enemyGrid.GetCellWidthWithBorders()) +
                 (enemyGrid.GetCellWidthWithBorders() / 2) + 2;

  if (invertGridPositions) {
    bulletX = currentGrid.GetXOffset() - 2;
    targetX = enemyGrid.GetXOffset() + (coord.x * enemyGrid.GetCellWidthWithBorders()) +
              (enemyGrid.GetCellWidthWithBorders() / 2) + 2;
  }

  enemyGrid.Render();

  const auto distance = invertGridPositions ? (bulletX - targetX) : (targetX - bulletX);
  const auto stepDelay = std::max(5, static_cast<int>(1500 / distance));

  while ((invertGridPositions ? bulletX > targetX : bulletX < targetX)) {
    IO::cout << AnsiHelper::MoveCursor(bulletX, bulletY);
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Yellow) << (invertGridPositions ? "←" : "→")
             << AnsiHelper::Reset();
    IO::cout.flush();

    std::this_thread::sleep_for(std::chrono::milliseconds(stepDelay));
    if (invertGridPositions)
      bulletX -= 2; // Move left by 2 to account for character width
    else
      bulletX += 2; // Move right by 2 to account for character width

    // Clear previous bullet
    IO::cout << AnsiHelper::MoveCursor(bulletX + (invertGridPositions ? 2 : -2), bulletY);
    IO::cout << " ";

    const auto insideEnemyGrid = bulletX >= enemyGrid.GetXOffset() &&
                                 bulletX < (enemyGrid.GetXOffset() + enemyGrid.GetTotalWidth());

    if (insideEnemyGrid)
      enemyGrid.Render();
  }

  // Final position
  IO::cout << AnsiHelper::MoveCursor(targetX, bulletY);
  IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red) << "*" << AnsiHelper::Reset();
  IO::cout.flush();

  inAnimation = false;

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  InputManager::DiscardPendingKeyPresses();

  ForceRender();
}

void InGameView::RenderTurnQueue() const {
  const size_t startX = 4;
  const size_t startY = currentGrid.GetYOffset() + currentGrid.GetTotalHeight() + 2;

  IO::cout << AnsiHelper::MoveCursor(startX, startY) << "Turn Queue: ";
  const auto& gameManager = AppState::GetCurrentGameManager();
  size_t queueIndex = currentPlayerIndex;

  IO::cout << AnsiHelper::MoveCursor(startX, startY + 1);
  IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
  IO::cout << " -  " << gameManager->Players().at(queueIndex).profile.name << " ";
  IO::cout << AnsiHelper::Reset();
  queueIndex = (queueIndex + 1) % gameManager->Players().size();

  constexpr size_t LIMIT = 6;

  size_t i = 1;
  while (queueIndex != currentPlayerIndex) {
    const auto& player = gameManager->Players().at(queueIndex);
    if (player.board.IsGameOver()) {
      queueIndex = (queueIndex + 1) % gameManager->Players().size();
      continue;
    }

    IO::cout << AnsiHelper::MoveCursor(startX, startY + 1 + i);
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
    IO::cout << "[" << i << "] " << player.profile.name << " ";
    IO::cout << AnsiHelper::Reset();
    queueIndex = (queueIndex + 1) % gameManager->Players().size();
    ++i;

    if (LIMIT == i) {
      IO::cout << AnsiHelper::MoveCursor(startX, startY + 1 + i);
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
      IO::cout << "...";
      IO::cout << AnsiHelper::Reset();
      break;
    }
  }
}

void InGameView::RenderLeaderboard() const {
  const size_t startX = 4 + enemyGrid.GetTotalWidth() + 4;
  const size_t startY = currentGrid.GetYOffset() + currentGrid.GetTotalHeight() + 2;

  IO::cout << AnsiHelper::MoveCursor(startX, startY) << "Leaderboard:";
  const auto& gameManager = AppState::GetCurrentGameManager();

  std::vector<uint16_t> segmentsLeft(gameManager->Players().size(), 0);
  for (size_t i = 0; i < gameManager->Players().size(); ++i) {
    segmentsLeft[i] = CalculateSegmentsLeftForPlayer(i);
  }

  std::vector<std::pair<size_t, uint16_t>> leaderboard;
  leaderboard.reserve(segmentsLeft.size());
  for (size_t i = 0; i < segmentsLeft.size(); ++i) {
    leaderboard.emplace_back(i, segmentsLeft[i]);
  }

  std::sort(leaderboard.begin(), leaderboard.end(), [](const auto& a, const auto& b) {
    return a.second > b.second;
  });

  constexpr size_t LIMIT = 6;

  size_t i = 1;
  for (const auto& [playerIndex, score] : leaderboard) {
    const auto& player = gameManager->Players().at(playerIndex);

    IO::cout << AnsiHelper::MoveCursor(startX, startY + i);
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
    IO::cout << "[" << i << "] " << player.profile.name << " - " << score << " ";
    IO::cout << AnsiHelper::Reset();

    i++;
    if (i == LIMIT) {
      IO::cout << AnsiHelper::MoveCursor(startX, startY + i);
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
      IO::cout << "...";
      IO::cout << AnsiHelper::Reset();
      break;
    }
  }
}

void InGameView::RenderHistory() const {
  const size_t startX = 4 + ((enemyGrid.GetTotalWidth() + 4) * 2);
  const size_t startY = currentGrid.GetYOffset() + currentGrid.GetTotalHeight() + 2;

  IO::cout << AnsiHelper::MoveCursor(startX, startY) << "Action History:";
  const auto& gameManager = AppState::GetCurrentGameManager();
  constexpr size_t LIMIT = 6;
  size_t i = 1;
  size_t j = actionHistory.size();
  for (auto it = actionHistory.rbegin(); it != actionHistory.rend(); ++it) {
    const auto& [attackerIndex, defenderIndex, coord, result] = *it;
    const auto& attacker = gameManager->Players().at(attackerIndex);
    const auto& defender = gameManager->Players().at(defenderIndex);

    IO::cout << AnsiHelper::MoveCursor(startX, startY + i);
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
    IO::cout << "[" << j << "] " << attacker.profile.name << " -> " << defender.profile.name << " ("
             << static_cast<char>('A' + coord.x) << (coord.y + 1) << ") ";

    switch (result) {
      case 0:
        IO::cout << "Miss";
        break;
      case 1:
        IO::cout << "Hit";
        break;
      case 2:
        IO::cout << "Destroyed";
        break;
      default:
        IO::cout << "Unknown";
        break;
    }

    IO::cout << AnsiHelper::Reset();

    j--;
    i++;
    if (i == LIMIT) {
      IO::cout << AnsiHelper::MoveCursor(startX, startY + i);
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
      IO::cout << "...";
      IO::cout << AnsiHelper::Reset();
      break;
    }
  }
}

void InGameView::HandleGameOver() {}
