#include "InGameView.h"

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/Player.h"
#include "Backend/Units/BattleUnit.h"
#include "Backend/Units/BattleUnitHelper.h"
#include "Backend/Units/BattleUnitType.h"
#include "Backend/Users/UserProfile.h"
#include "Frontend/Helpers//PromptHelper.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/AppState.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/Grid.h"
#include "Frontend/Helpers/TextHelper.h"
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
#include <cstdlib>
#include <map>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

void InGameView::SetGridOffsets() {
  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  if (invertGridPositions) {
    enemyGrid.SetOffset(2 + 2, 8);
    enemyGrid.SetInvertOnXAxis(false);

    currentGrid.SetOffset(width - enemyGrid.GetTotalWidth() - enemyGrid.GetXOffset() - 1, 8);
    currentGrid.SetInvertOnXAxis(true);
  } else {
    currentGrid.SetOffset(2 + 2, 8);
    currentGrid.SetInvertOnXAxis(false);

    enemyGrid.SetOffset(width - currentGrid.GetTotalWidth() - currentGrid.GetXOffset() - 1, 8);
    enemyGrid.SetInvertOnXAxis(true);
  }
}

void InGameView::OnEnter() {
  const auto& mode = GetGameMode();

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  currentPlayerIndex = 0;
  enemyPlayerIndex = 1;
  fastForwardEnabled = false;
  invertGridPositions = false;

  actionHistory.clear();
  lastPositionsPerPlayer.clear();
  const auto& players = GetPlayers();
  for (size_t i = 0; i < players.size(); i++) {
    lastPositionsPerPlayer[i] = {
        (i + 1) % players.size(), Coordinates{0, 0}
    };
  }

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
      width - currentGrid.GetTotalWidth() - currentGrid.GetXOffset() - 1,
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

  currentGrid.SetInvertOnXAxis(false);
  enemyGrid.SetInvertOnXAxis(true);

  OnChangeTurn();

  InputManager::GetTerminalSize(width, height);
  WindowManager::GetInstance().OnTerminalResize(width, height);
}

void InGameView::OnExit() { IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset(); }

void InGameView::HandleEscape() {
  // Prompt to confirm exit to main menu
  if (PromptHelper::ShowYesNoPrompt("Are you sure you want to exit to the main menu?")) {
    AppState::Reset();
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
  } else {
    ForceRender();
  }
}

void InGameView::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool InGameView::IsCorrectSize(int width, int height) const {
  const auto& mode = GetGameMode();
  const auto requiredWidth = (((mode.boardWidth + 1) * (5) * 2) + 6);
  const auto requiredHeight = currentGrid.GetTotalHeight() + 10;
  return static_cast<size_t>(width) >= requiredWidth &&
         static_cast<size_t>(height) >= requiredHeight;
}

void InGameView::ForceRender() {
  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();

  BoxDrawing::DrawWindowFrame(true, "In-Game View");

  if (ultraFastForwardEnabled) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Yellow);
    TextHelper::DrawCenteredText(3, "Ultra Fast Forward Enabled (Press Shift + '.' to toggle)");
    IO::cout << AnsiHelper::Reset();
  } else if (fastForwardEnabled) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Yellow);
    TextHelper::DrawCenteredText(3, "Fast Forward Enabled (Press '.' to toggle)");
    IO::cout << AnsiHelper::Reset();
  }

  const auto& mode = GetGameMode();

  const auto& currentPlayer = GetPlayerAtIndex(currentPlayerIndex);
  const auto& enemyPlayer = GetPlayerAtIndex(enemyPlayerIndex);

  const auto alivePlayersCount =
      std::count_if(GetPlayers().begin(), GetPlayers().end(), [](const auto& player) {
        return !player.board->IsGameOver();
      });

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
      currentGrid.GetXOffset() - 1 + (invertGridPositions ? 1 : 0),
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
      enemyGrid.GetXOffset() - 1 + (!invertGridPositions ? 1 : 0),
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
  const auto& players = GetPlayers();
  const auto& currentPlayer = GetCurrentPlayer();

  const auto pred = [&currentPlayer](const auto& player) {
    return player.profile.UserId() == currentPlayer.profile.UserId();
  };

  currentPlayerIndex = std::find_if(players.begin(), players.end(), pred) - players.begin();

  enemyPlayerIndex = lastPositionsPerPlayer[currentPlayerIndex].first;
  bool firstLoop = true;
  while (players[enemyPlayerIndex].board->IsGameOver() || currentPlayerIndex == enemyPlayerIndex) {
    if (currentPlayerIndex == enemyPlayerIndex) {
      if (!firstLoop)
        throw std::runtime_error("No valid enemy player found!");
      firstLoop = false;
    }

    enemyPlayerIndex = (enemyPlayerIndex + 1) % players.size();
  }

  const auto& lastCoord = lastPositionsPerPlayer[currentPlayerIndex].second;
  enemyGrid.SetCursorPosition(lastCoord.x, lastCoord.y);

  ForceRender();

  HandleNextTurn();
}

void InGameView::RenderCell(
    size_t x, size_t y, size_t posX, size_t posY, bool isCursor, size_t playerIndex
) const {
  const auto& player = GetPlayerAtIndex(playerIndex);
  const auto& board = player.board;
  const auto& segmentBoard = board->GetSegmentBoard();
  const auto& unitsPlacement = board->Units();

  const Coordinates coord(static_cast<int>(x), static_cast<int>(y));
  bool isHit = segmentBoard.Segments()[y][x];
  bool hasUnit = unitsPlacement[y][x] != nullptr;

  UpdateRenderCellState(x, y, posX, posY, isCursor, playerIndex, isHit, hasUnit);

  IO::cout << AnsiHelper::MoveCursor(posX, posY);

  bool updateColor = true;
  const auto* const symbol =
      GetCellSymbol(x, y, playerIndex, isHit, hasUnit, isCursor, unitsPlacement, updateColor);

  const auto& mode = GetGameMode();
  if (mode.isExtended) {
    const auto& landSegments = segmentBoard.LandSegments();

    if (landSegments[y][x]) {
      IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Green);
    } else {
      IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Blue);
    }
  }

  if (updateColor) {
    if (isHit && hasUnit) {
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red);
    } else if (isHit && !hasUnit) {
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Blue);
    }

    IO::cout << symbol;
  }

  IO::cout << AnsiHelper::Reset();
  IO::cout.flush();
}

const char* InGameView::GetCellSymbol(
    size_t x,
    size_t y,
    size_t playerIndex,
    bool isHit,
    bool hasUnit,
    bool isCursor,
    std::vector<std::vector<std::shared_ptr<BattleUnit>>> unitsPlacement,
    bool& updateColor
) const {
  const auto& player = GetPlayerAtIndex(playerIndex);

  updateColor = true;
  if (!salvoSelectionCoordinates.empty()) {
    const auto isSelected = std::any_of(
        salvoSelectionCoordinates.begin(),
        salvoSelectionCoordinates.end(),
        [x, y](const Coordinates& coord) { return coord.x == x && coord.y == y; }
    );

    if (isSelected) {
      if (inAnimation) {
        updateColor = false;
        IO::cout << AnsiHelper::SetTextColor(AnsiColor::White);
      }

      return "{ }";
    }
  }

  const char* symbolChar = " ";

  if (isHit && hasUnit) {
    if (unitsPlacement[y][x]->IsDestroyed()) {
      switch (player.profile.settings.unitPattern) {
        case UnitPattern::Default:
          symbolChar = "D";
          break;

        case UnitPattern::FlowerShipIcon:
          symbolChar = "✿";
          break;

        case UnitPattern::CrosshairShipIcon:
          symbolChar = "⌖";
          break;

        case UnitPattern::StarShipIcon:
          symbolChar = "★";
          break;

        case UnitPattern::StoneShipIcon:
          symbolChar = "■";
          break;

        default:
          symbolChar = "@";
          break;
      }
    } else {
      symbolChar = "X";
    }
  } else if (isHit && !hasUnit) {
    symbolChar = "o";
  } else if (!isCursor) {
    symbolChar = ".";
  }

  static std::array<char, 10> buffer{};
  if (isCursor) {
    snprintf(buffer.data(), buffer.size(), "[%s]", symbolChar);
  } else {
    snprintf(buffer.data(), buffer.size(), " %s ", symbolChar);
  }

  return buffer.data();
}

void InGameView::UpdateRenderCellState(
    size_t /*x*/,
    size_t /*y*/,
    size_t /*posX*/,
    size_t /*posY*/,
    bool /*isCursor*/,
    size_t /*playerIndex*/,
    bool& /*isHit*/,
    bool& /*hasUnit*/
) const {}

size_t InGameView::CalculateSegmentsLeftForPlayer(size_t playerIndex, size_t* outMax) const {
  const auto& mode = GetGameMode();
  const auto& player = GetPlayerAtIndex(playerIndex);
  const auto& units = player.board->GetAllUnits();

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

void InGameView::RenderUnitsLeftForPlayer(size_t playerIndex, size_t startX, size_t startY) const {
  const auto& mode = GetGameMode();
  const auto& player = GetPlayerAtIndex(playerIndex);
  const auto& units = player.board->GetAllUnits();

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

void InGameView::HandleAfterFireAtCoordinate() {
  const auto& enemyPlayer = GetPlayerAtIndex(enemyPlayerIndex);
  const auto& enemyUnits = enemyPlayer.board->Units();
  for (const auto& coord : salvoSelectionCoordinates) {
    uint8_t result = 0;
    if (enemyUnits[coord.y][coord.x] != nullptr) {
      if (enemyUnits[coord.y][coord.x]->IsDestroyed()) {
        result = 2; // Destroyed
      } else {
        result = 1; // Hit
      }
    } else {
      result = 0; // Miss
    }
    actionHistory.emplace_back(currentPlayerIndex, enemyPlayerIndex, coord, result);
  }

  if (!ultraFastForwardEnabled)
    ShowPlayerFireAnimation();

  CheckForPeriod();

  salvoSelectionCoordinates.clear();
  AppState::IncrementTurnCounter(currentPlayerIndex);

  if (GetGameState() == GameState::Over) {
    // Show game over prompt
    HandleGameOver();
    return;
  }

  OnChangeTurn();
}

void InGameView::CalculateBulletPath(
    size_t startX,
    size_t startY,
    size_t targetX,
    std::vector<std::pair<Coordinates, bool>>& outPath,
    const Grid& enemyGrid
) {
  outPath.clear();

  size_t bulletX = startX;
  size_t const bulletY = startY;

  const auto invertGridPositions = bulletX > targetX;

  while ((invertGridPositions ? bulletX > targetX : bulletX < targetX)) {
    const auto insideEnemyGrid = bulletX >= enemyGrid.GetXOffset() &&
                                 bulletX < (enemyGrid.GetXOffset() + enemyGrid.GetTotalWidth());
    outPath.push_back({
        {bulletX, bulletY},
        insideEnemyGrid
    });

    if (!invertGridPositions) {
      bulletX += 2;
    } else {
      bulletX -= 2;
    }
  }
}

void InGameView::CalculateAnimationCoordinates(
    const Coordinates& coord,
    const Grid& enemyGrid,
    bool invertGridPositions,
    size_t& outTargetX,
    size_t& outTargetY
) {
  outTargetY = enemyGrid.GetYOffset() + (coord.y * enemyGrid.GetCellHeightWithBorders()) +
               (enemyGrid.GetCellHeightWithBorders() / 2) + 1;

  auto coordX = coord.x;
  if (!invertGridPositions) {
    coordX = (enemyGrid.GetWidth() - 1) - coord.x;
  }

  outTargetX = enemyGrid.GetXOffset() + (coordX * enemyGrid.GetCellWidthWithBorders()) +
               (enemyGrid.GetCellWidthWithBorders() / 2) + 2;

  if (invertGridPositions) {
    outTargetX = enemyGrid.GetXOffset() + (coordX * enemyGrid.GetCellWidthWithBorders()) +
                 (enemyGrid.GetCellWidthWithBorders() / 2) + 2 - 1;
  }
}

std::vector<std::vector<std::pair<Coordinates, bool>>> InGameView::CalculateBulletPaths(
    const std::vector<Coordinates>& targetCoordinates,
    const Grid& enemyGrid,
    bool invertGridPositions,
    size_t& maxDistance,
    size_t bulletX
) {
  std::vector<std::vector<std::pair<Coordinates, bool>>> out;

  maxDistance = 1;
  std::vector<std::pair<Coordinates, bool>> emptyPath{};
  for (const auto& coord : targetCoordinates) {
    size_t outTargetX = 0;
    size_t outTargetY = 0;
    CalculateAnimationCoordinates(coord, enemyGrid, invertGridPositions, outTargetX, outTargetY);

    CalculateBulletPath(bulletX, outTargetY, outTargetX, emptyPath, enemyGrid);
    out.push_back(emptyPath);

    const auto distance =
        static_cast<size_t>(std::abs(static_cast<int>(bulletX) - static_cast<int>(outTargetX)));
    maxDistance = std::max(distance, maxDistance);
  }

  return out;
}

void InGameView::CheckForPeriod() {
  ConsoleKeyDetails keyDetails{};
  if (InputManager::TryGetKeyPress(keyDetails)) {
    if (keyDetails.key == ConsoleKey::OemPeriod) {
      if (keyDetails.modifiers == ConsoleModifiers::Shift) {
        ultraFastForwardEnabled = !ultraFastForwardEnabled;
      } else {
        fastForwardEnabled = !fastForwardEnabled;
        ultraFastForwardEnabled = false;
      }
      ForceRender();
    } else if (keyDetails.key == ConsoleKey::Escape) {
      HandleEscape();
    }
  }
}

void InGameView::ShowPlayerFireAnimation() {
  inAnimation = true;

  assert(!salvoSelectionCoordinates.empty());

  auto bulletX = currentGrid.GetTotalWidth() + currentGrid.GetXOffset() + 3;
  if (invertGridPositions)
    bulletX = currentGrid.GetXOffset() - 1;

  size_t maxDistance = 0;
  const auto bulletPaths = CalculateBulletPaths(
      salvoSelectionCoordinates, enemyGrid, invertGridPositions, maxDistance, bulletX
  );

  enemyGrid.Render();

  const auto stepDelay = std::max(5, static_cast<int>(1500 / maxDistance));

  const size_t totalFrames =
      std::max_element(bulletPaths.begin(), bulletPaths.end(), [](const auto& a, const auto& b) {
        return a.size() < b.size();
      })->size();

  for (size_t frameId = 0; frameId < totalFrames; frameId++) {
    CheckForPeriod();

    for (const auto& path : bulletPaths) {
      if (frameId >= path.size())
        continue;

      const auto& [coord, _] = path.at(frameId);

      IO::cout << AnsiHelper::MoveCursor(coord.x, coord.y);
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Yellow) << (invertGridPositions ? "←" : "→")
               << AnsiHelper::Reset();
      IO::cout.flush();
    }

    if (!fastForwardEnabled)
      std::this_thread::sleep_for(std::chrono::milliseconds(stepDelay));

    bool insideEnemyGrid = false;
    for (const auto& path : bulletPaths) {
      if (frameId >= path.size())
        continue;

      const auto& [coord, localInsideEnemyGrid] = path.at(frameId);

      // Clear previous bullet
      IO::cout << AnsiHelper::MoveCursor(coord.x + (invertGridPositions ? 2 : -2), coord.y);
      IO::cout << " ";

      insideEnemyGrid |= localInsideEnemyGrid;
    }

    if (insideEnemyGrid)
      enemyGrid.Render();
  }

  for (const auto& path : bulletPaths) {
    const auto& [coord, _] = path.back();

    // Final position
    IO::cout << AnsiHelper::MoveCursor(coord.x, coord.y);
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red) << "*" << AnsiHelper::Reset();
    IO::cout.flush();
  }

  inAnimation = false;

  if (!fastForwardEnabled)
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  InputManager::DiscardPendingKeyPresses();

  ForceRender();
}

void InGameView::RenderTurnQueue() const {
  const size_t startX = 4;
  const size_t startY = currentGrid.GetYOffset() + currentGrid.GetTotalHeight() + 2;

  IO::cout << AnsiHelper::MoveCursor(startX, startY) << "Turn Queue: ";
  size_t queueIndex = currentPlayerIndex;

  IO::cout << AnsiHelper::MoveCursor(startX, startY + 1);
  IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
  IO::cout << " -  " << GetPlayerAtIndex(queueIndex).profile.name << " ";
  IO::cout << AnsiHelper::Reset();
  queueIndex = (queueIndex + 1) % GetPlayers().size();

  constexpr size_t LIMIT = 6;

  size_t i = 1;
  while (queueIndex != currentPlayerIndex) {
    const auto& player = GetPlayerAtIndex(queueIndex);
    if (player.board->IsGameOver()) {
      queueIndex = (queueIndex + 1) % GetPlayers().size();
      continue;
    }

    IO::cout << AnsiHelper::MoveCursor(startX, startY + 1 + i);
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Cyan);
    IO::cout << "[" << i << "] " << player.profile.name << " ";
    IO::cout << AnsiHelper::Reset();
    queueIndex = (queueIndex + 1) % GetPlayers().size();
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

  std::vector<uint16_t> segmentsLeft(GetPlayers().size(), 0);
  for (size_t i = 0; i < GetPlayers().size(); ++i) {
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
    const auto& player = GetPlayerAtIndex(playerIndex);

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
  const size_t startX = 4 + ((enemyGrid.GetTotalWidth() + 4) * 2) - 10;
  const size_t startY = currentGrid.GetYOffset() + currentGrid.GetTotalHeight() + 2;

  IO::cout << AnsiHelper::MoveCursor(startX, startY) << "Action History:";
  constexpr size_t LIMIT = 6;
  size_t i = 1;
  size_t j = actionHistory.size();
  for (auto it = actionHistory.rbegin(); it != actionHistory.rend(); ++it) {
    const auto& [attackerIndex, defenderIndex, coord, result] = *it;
    const auto& attacker = GetPlayerAtIndex(attackerIndex);
    const auto& defender = GetPlayerAtIndex(defenderIndex);

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

void InGameView::HandleGameOver() {
  WindowManager::GetInstance().SwitchToWindow(WindowType::PostGameSummary);
}
