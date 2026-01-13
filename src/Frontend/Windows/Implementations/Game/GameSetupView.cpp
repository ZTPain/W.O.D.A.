#include "GameSetupView.h"

#include "Backend/Boards/ISegment.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/BattleUnitHelper.h"
#include "Backend/Units/BattleUnitType.h"
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
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

GameSetupView::GameSetupView() : Window(WindowType::GameSetup), currentPlayerIndex(0) {}

static int redX = -1;
static int redY = -1;

void GameSetupView::RenderEmptyCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) {
  IO::cout << AnsiHelper::MoveCursor(posX, posY);

  if (redX == static_cast<int>(x) && redY == static_cast<int>(y)) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red);
  }

  if (isCursor) {
    IO::cout << "[  ]";
  } else {
    IO::cout << " .  ";
  }

  IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Default);
  IO::cout.flush();
}

AnsiColor GameSetupView::GetColorForUnitType(BattleUnitType type) {
  switch (type) {
    case BattleUnitType::PatrolBoat:
      return AnsiColor::Cyan;

    case BattleUnitType::Interceptor:
      return AnsiColor::Green;

    case BattleUnitType::Cruiser:
      return AnsiColor::Yellow;

    case BattleUnitType::Dreadnought:
      return AnsiColor::Magenta;

    case BattleUnitType::None:
      return AnsiColor::Red;

    default:
      return AnsiColor::White;
  }
}

BattleUnitType GameSetupView::GetUnitTypeOfCoordinate(const Coordinates& coord) const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& currentPlayer = gameManager->Players().at(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  const auto& segmentBoard = board.GetSegmentBoard();
  const auto& units = segmentBoard.GetUnits();

  const auto predicate = [&](const Coordinates& c) { return c == coord; };

  for (const auto& [unitType, groupsOfType] : units) {
    for (const auto& group : groupsOfType) {
      const auto none = std::none_of(group.begin(), group.end(), predicate);

      if (!none)
        return unitType;
    }
  }

  return BattleUnitType::None;
}

void GameSetupView::RenderFilledCell(
    size_t x, size_t y, size_t posX, size_t posY, bool isCursor
) const {
  IO::cout << AnsiHelper::MoveCursor(posX, posY);

  const auto currentCoord = Coordinates(x, y);

  const auto unitType = GetUnitTypeOfCoordinate(currentCoord);
  const auto color = GetColorForUnitType(unitType);

  IO::cout << AnsiHelper::SetBackgroundColor(color);

  if (redX == static_cast<int>(x) && redY == static_cast<int>(y)) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red);
  }

  const char* symbol = nullptr;

  switch (unitType) {
    case BattleUnitType::PatrolBoat:
      symbol = "⊛";
      break;

    case BattleUnitType::Interceptor:
      symbol = "☸";
      break;

    case BattleUnitType::Cruiser:
      symbol = "𐋺";
      break;

    case BattleUnitType::Dreadnought:
      symbol = "𐃏";
      break;

    case BattleUnitType::None:
      symbol = "❀";
      break;
  }

  if (isCursor) {
    IO::cout << "[" << symbol << " ]";
  } else {
    IO::cout << " " << symbol << "  ";
  }

  IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Default);
  IO::cout.flush();
}

void GameSetupView::RenderCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& currentPlayer = gameManager->Players().at(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  const auto& segmentBoard = board.GetSegmentBoard();

  if (segmentBoard.Segments()[y][x]) {
    RenderFilledCell(x, y, posX, posY, isCursor);
  } else {
    RenderEmptyCell(x, y, posX, posY, isCursor);
  }
}

void GameSetupView::RenderUnitsLeft(const std::unordered_map<BattleUnitType, size_t>& unitPool) {
  IO::cout << AnsiHelper::MoveCursor(grid.GetTotalWidth() + 8, 5) << "Units left: ";
  auto i = 1;

  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& currentPlayer = gameManager->Players().at(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  const auto& segmentBoard = board.GetSegmentBoard();

  for (const auto& [unitType, count] : unitPool) {
    const auto color = GetColorForUnitType(unitType);
    const auto* const name = BattleUnitHelper::GetNameForUnitType(unitType);

    const auto unitsPlaced = segmentBoard.GetUnits().at(unitType);

    const auto unitsLeft =
        count - std::count_if(
                    unitsPlaced.begin(),
                    unitsPlaced.end(),
                    [](const std::vector<Coordinates>& group) { return !group.empty(); }
                );

    IO::cout << AnsiHelper::MoveCursor(grid.GetTotalWidth() + 8, 5 + i++);
    IO::cout << AnsiHelper::SetTextColor(color);
    IO::cout << name << ": " << unitsLeft << " ";
    IO::cout << AnsiHelper::Reset();
  }
  IO::cout.flush();
}

void GameSetupView::OnToggleCell(size_t x, size_t y, size_t /*posX*/, size_t /*posY*/) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& currentPlayer = gameManager->Players().at(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  auto& segmentBoard = board.GetSegmentBoard();

  if (segmentBoard.ToggleSegment(x, y)) {
    grid.Render();

    ShowErrorMessage("");

    RenderUnitsLeft(mode.unitPool);
  } else {
    // Invalid toggle
    ShowErrorMessage("Invalid segment toggle!");
    redX = static_cast<int>(x);
    redY = static_cast<int>(y);
  }
}

void GameSetupView::OnEnter() {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();

  redX = -1;
  redY = -1;

  currentPlayerIndex = 0;

  grid = Grid(
      2 + 2,
      5,
      mode.boardWidth,
      mode.boardHeight,
      3,
      1,
      [this](size_t x, size_t y, size_t posX, size_t posY, bool isCursor) {
        RenderCell(x, y, posX, posY, isCursor);
      },
      [this](size_t x, size_t y, size_t posX, size_t posY) { OnToggleCell(x, y, posX, posY); }
  );

  ForceRender();
}

void GameSetupView::OnExit() { IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset(); }

bool GameSetupView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }

  redX = -1;
  redY = -1;

  if (keyDetails.key == ConsoleKey::H) {
    ConfirmGridSetup();
    return true;
  }

  if (keyDetails.key == ConsoleKey::R) {
    const auto& gameManager = AppState::GetCurrentGameManager();
    const auto& currentPlayer = gameManager->Players().at(currentPlayerIndex);

    GenerateRandomSetup(
        &currentPlayer.board.GetSegmentBoard(),
        currentPlayer.profile.AI() != nullptr ? currentPlayer.profile.AI()->GetComputerType()
                                              : ComputerType::Easy
    );

    ForceRender();
    return true;
  }

  grid.OnKeyPressed(keyDetails);

  return false;
}

void GameSetupView::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

void GameSetupView::ForceRender() {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& currentPlayer = gameManager->Players().at(currentPlayerIndex);
  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();

  std::array<char, 100> titleBuffer{};
  std::snprintf(
      titleBuffer.data(),
      titleBuffer.size(),
      "Game Setup - Player %zu: %s (placing units, total players: %zu)",
      currentPlayerIndex + 1,
      currentPlayer.profile.name.c_str(),
      gameManager->Players().size()
  );

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  BoxDrawing::DrawWindowFrame(true, titleBuffer.data());

  BoxDrawing::DrawBox(
      1 + 2,
      4,
      ((mode.boardWidth + 1) * grid.GetCellWidthWithBorders()) - 1,
      ((mode.boardHeight + 1) * grid.GetCellHeightWithBorders()) + 1,
      BoxStyle::Single,
      true
  );

  grid.Render();
  RenderUnitsLeft(mode.unitPool);
}

bool GameSetupView::IsCorrectSize(int width, int height) const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto requiredWidth = ((mode.boardWidth + 1) * 5) + 5;
  const auto requiredHeight = ((mode.boardHeight + 1) * 2) + 5;
  return static_cast<size_t>(width) >= requiredWidth &&
         static_cast<size_t>(height) >= requiredHeight;
}

void GameSetupView::GenerateRandomSetup(ISegment* segmentBoard, ComputerType /*computerType*/) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();

  // Clear current setup
  segmentBoard->Clear();

  // Randomly place units
  for (const auto& [unitType, count] : mode.unitPool) {
    GenerateUnitPlacement(segmentBoard, unitType, count);
  }
}

void GameSetupView::GenerateUnitPlacement(
    ISegment* segmentBoard, BattleUnitType unitType, size_t count
) {
  for (size_t i = 0; i < count; ++i) {
    PlaceUnitAtRandom(segmentBoard, unitType);
  }
}

void GameSetupView::PlaceUnitAtRandom(ISegment* segmentBoard, BattleUnitType unitType) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& unitSize = BattleUnitHelper::GetSizeForUnitType(unitType);

  bool placed = false;

  size_t attempts = 0;
  const size_t maxAttempts = 1000;

  while (!placed) {
    // Prevent infinite loop
    if (attempts++ >= maxAttempts) {
      ShowErrorMessage("Failed to place unit after multiple attempts.");
      InputManager::WaitUntillKeyPressed(true);
      WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
      break;
    }

    const auto orientation = rand() % 2;
    const auto startX = rand() % mode.boardWidth;
    const auto startY = rand() % mode.boardHeight;

    // Check if unit can be placed
    bool const canPlace = CanPlaceUnitAt(segmentBoard, unitType, startX, startY, orientation);

    if (!canPlace) {
      continue;
    }

    // Place unit
    for (size_t j = 0; j < unitSize; ++j) {
      size_t x = startX;
      size_t y = startY;

      if (orientation == 0) {
        x += j;
      } else {
        y += j;
      }

      segmentBoard->ToggleSegment(x, y);
    }

    placed = true;
  }
}

bool GameSetupView::CanPlaceUnitAt(
    ISegment* segmentBoard,
    BattleUnitType unitType,
    size_t startX,
    size_t startY,
    size_t orientation
) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& segments = segmentBoard->Segments();
  const auto& unitSize = BattleUnitHelper::GetSizeForUnitType(unitType);

  for (size_t j = 0; j < unitSize; ++j) {
    size_t x = startX;
    size_t y = startY;

    if (orientation == 0) {
      x += j;
    } else {
      y += j;
    }

    if (x >= mode.boardWidth || y >= mode.boardHeight || segments[y][x]) {
      return false;
    }

    // Check adjacent cells
    if (!CheckAdjacentCells(segmentBoard, x, y)) {
      return false;
    }
  }

  return true;
}

bool GameSetupView::CheckAdjacentCells(ISegment* segmentBoard, size_t x, size_t y) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& segments = segmentBoard->Segments();

  for (int adjY = -1; adjY <= 1; ++adjY) {
    for (int adjX = -1; adjX <= 1; ++adjX) {
      if (adjX == 0 && adjY == 0)
        continue;

      if (adjY != 0 && adjX != 0)
        continue; // Skip diagonals

      size_t const neighborX = x + adjX;
      size_t const neighborY = y + adjY;

      if (neighborX >= mode.boardWidth || neighborY >= mode.boardHeight)
        continue;

      if (segments[neighborY][neighborX])
        return false;
    }
  }

  return true;
}

void GameSetupView::ConfirmGridSetup() {
  const auto& gameManager = AppState::GetCurrentGameManager();

  if (!AllUnitsPlaced()) {
    ShowErrorMessage("Not all units have been placed!");
    return;
  }

  // Show confirmation prompt
  // TODO: Implement confirmation prompt

  const auto& players = gameManager->Players();

  // Move to next player or finish setup
  if (currentPlayerIndex + 1 < players.size()) {
    currentPlayerIndex++;

    if (players.at(currentPlayerIndex).profile.AI() != nullptr) {
      GenerateRandomSetup(
          &players.at(currentPlayerIndex).board.GetSegmentBoard(),
          players.at(currentPlayerIndex).profile.AI()->GetComputerType()
      );
    }

    ForceRender();
  } else {
    // All players have set up their boards
    WindowManager::GetInstance().SwitchToWindow(WindowType::InGame);
  }
}

void GameSetupView::ShowErrorMessage(const std::string& message) {
  static size_t lastMessageLength = 0;
  // Clear previous message
  IO::cout << AnsiHelper::MoveCursor(1, 3);
  IO::cout << std::string(lastMessageLength, ' ');

  if (message.empty())
    return;

  lastMessageLength = message.length();
  IO::cout << AnsiHelper::MoveCursor(1, 3) << AnsiHelper::SetTextColor(AnsiColor::Red) << message
           << AnsiHelper::Reset();
  IO::cout.flush();
}

bool GameSetupView::AllUnitsPlaced() const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& currentPlayer = gameManager->Players().at(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  const auto& segmentBoard = board.GetSegmentBoard();

  const auto& units = segmentBoard.GetUnits();

  if (!units.at(BattleUnitType::None).at(0).empty()) {
    return false;
  }

  for (const auto& [unitType, units] : units) {
    if (unitType == BattleUnitType::None)
      continue;

    for (const auto& group : units) {
      if (group.empty()) {
        return false;
      }
    }
  }

  return true;
}
