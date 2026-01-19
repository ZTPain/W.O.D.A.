#include "GameSetupView.h"

#include "Backend/Boards/ISegment.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Computers/ComputerStrategyHelper.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/BattleUnit.h"
#include "Backend/Units/BattleUnitHelper.h"
#include "Backend/Units/BattleUnitType.h"
#include "Backend/Units/FighterJet.h"
#include "Backend/Units/OperationsHeadquarter.h"
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
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

GameSetupView::GameSetupView() : Window(WindowType::GameSetup), currentPlayerIndex(0) {}

static int redX = -1;
static int redY = -1;

void GameSetupView::RenderEmptyCell(
    size_t x, size_t y, size_t posX, size_t posY, bool isCursor
) const {
  IO::cout << AnsiHelper::MoveCursor(posX, posY);

  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = AppState::GetCurrentGameMode();
  if (mode.isExtended) {
    const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);
    const auto& board = currentPlayer.board;
    const auto& segmentBoard = board->GetSegmentBoard();
    const auto& landSegments = segmentBoard.LandSegments();

    if (landSegments[y][x]) {
      IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Green);
    } else {
      IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Blue);
    }
  }

  if (redX == static_cast<int>(x) && redY == static_cast<int>(y)) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red);
  }

  if (isCursor) {
    IO::cout << "[  ]";
  } else {
    IO::cout << " .  ";
  }

  IO::cout << AnsiHelper::Reset();
  if (!grid.IsInGridRender())
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

    case BattleUnitType::InfantrySquadron:
      return AnsiColor::Blue;

    case BattleUnitType::GrenadeLauncher:
      return AnsiColor::BrightGreen;

    case BattleUnitType::MobileArtillery:
      return AnsiColor::BrightYellow;

    case BattleUnitType::ArmoredTrain:
      return AnsiColor::BrightMagenta;

    case BattleUnitType::OperationsHeadquarter:
      return AnsiColor::BrightCyan;

    case BattleUnitType::FighterJet:
      return AnsiColor::BrightRed;

    default:
      return AnsiColor::White;
  }
}

BattleUnitType GameSetupView::GetUnitTypeOfCoordinate(const Coordinates& coord) const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  const auto& segmentBoard = board->GetSegmentBoard();
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
      symbol = "!";
      break;

    default:
      symbol = "■";
      break;
  }

  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = AppState::GetCurrentGameMode();
  if (mode.isExtended) {
    const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);
    const auto& board = currentPlayer.board;
    const auto& segmentBoard = board->GetSegmentBoard();
    const auto& landSegments = segmentBoard.LandSegments();

    if (landSegments[y][x]) {
      IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Green);
    } else {
      IO::cout << AnsiHelper::SetBackgroundColor(AnsiColor::Blue);
    }
  }

  const bool red = redX == static_cast<int>(x) && redY == static_cast<int>(y);
  if (red) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red);
  }

  if (isCursor)
    IO::cout << "[";
  else
    IO::cout << " ";
  IO::cout << AnsiHelper::SetTextColor(color) << symbol
           << AnsiHelper::SetTextColor(red ? AnsiColor::Red : AnsiColor::Default);
  if (isCursor)
    IO::cout << "]";
  else
    IO::cout << " ";

  IO::cout << AnsiHelper::Reset();
  IO::cout << " ";
  if (!grid.IsInGridRender())
    IO::cout.flush();
}

void GameSetupView::RenderCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) const {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  const auto& segmentBoard = board->GetSegmentBoard();

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
  const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  const auto& segmentBoard = board->GetSegmentBoard();

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
  const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  auto& segmentBoard = board->GetSegmentBoard();

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

  HandleAI();

  ForceRender();

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);
  WindowManager::GetInstance().OnTerminalResize(width, height);
}

void GameSetupView::OnExit() { IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset(); }

bool GameSetupView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    AppState::Reset();
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
    const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);

    while (!AllUnitsPlaced()) {
      GenerateRandomSetup(
          &currentPlayer.board->GetSegmentBoard(),
          currentPlayer.profile.AI() != nullptr ? currentPlayer.profile.AI()->GetComputerType()
                                                : ComputerType::Easy
      );
    }

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
  const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);

  if (gameManager->State() != GameState::Setting) {
    return;
  }

  const auto aiVsAi = std::all_of(
      gameManager->Players().begin(), gameManager->Players().end(), [](const auto& player) {
        return player.profile.AI() != nullptr;
      }
  );

  if (aiVsAi)
    return;

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
  const auto requiredWidth = grid.GetTotalWidth() + 5;
  const auto requiredHeight = grid.GetTotalHeight() + 5;
  return static_cast<size_t>(width) >= requiredWidth &&
         static_cast<size_t>(height) >= requiredHeight;
}

// Justification: This is recursive only if the next player is AI
// In theory this could lead to stack overflow if there are only AI players
// Will probably need to be reworked in the future to avoid that case
// NOLINTNEXTLINE(misc-no-recursion)
void GameSetupView::HandleAI() {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);

  if (currentPlayer.profile.AI() == nullptr)
    return;

  GenerateRandomSetup(
      &currentPlayer.board->GetSegmentBoard(), currentPlayer.profile.AI()->GetComputerType()
  );

  while (!ConfirmGridSetup()) {
    GenerateRandomSetup(
        &currentPlayer.board->GetSegmentBoard(), currentPlayer.profile.AI()->GetComputerType()
    );
  }
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

  bool placed = false;

  size_t attempts = 0;
  const size_t maxAttempts = mode.isExtended ? 10000 : 1000;

  while (!placed) {
    // Prevent infinite loop
    if (attempts++ >= maxAttempts) {
      ShowErrorMessage("Failed to place unit after multiple attempts.");
      InputManager::WaitUntillKeyPressed(true);
      WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
      break;
    }

    const auto orientation = ComputerStrategyHelper::GetRandomFromRange(0, 3);
    const auto startX = ComputerStrategyHelper::GetRandomFromRange(0, mode.boardWidth);
    const auto startY = ComputerStrategyHelper::GetRandomFromRange(0, mode.boardHeight);

    // Check if unit can be placed
    bool const canPlace = CanPlaceUnitAt(segmentBoard, unitType, startX, startY, orientation);

    if (!canPlace) {
      continue;
    }

    // Place unit
    PlaceUnitAt(segmentBoard, unitType, startX, startY, orientation);
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
  const auto& unitSize = BattleUnitHelper::GetSizeForUnitType(unitType);
  const auto category = BattleUnitHelper::GetCategoryForUnitType(unitType);
  const auto& mode = AppState::GetCurrentGameMode();
  const auto& landSegments =
      mode.isExtended ? segmentBoard->LandSegments() : std::vector<std::vector<bool>>();

  if (unitType == BattleUnitType::OperationsHeadquarter || unitType == BattleUnitType::FighterJet) {
    return CanPlaceUnitAtSpecialCase(segmentBoard, unitType, startX, startY, orientation);
  }

  orientation = orientation % 2; // 0 = horizontal, 1 = vertical

  for (size_t j = 0; j < unitSize; ++j) {
    size_t x = startX;
    size_t y = startY;

    if (orientation == 0) {
      x += j;
    } else {
      y += j;
    }

    if (!CheckSegmentAvailability(segmentBoard, x, y)) {
      return false;
    }

    if (mode.isExtended) {
      if (category == BattleUnitCategory::Land && !landSegments[y][x])
        return false;

      if (category == BattleUnitCategory::Marine && landSegments[y][x])
        return false;
    }
  }

  return true;
}

bool GameSetupView::CanPlaceUnitAtSpecialCase(
    ISegment* segmentBoard,
    BattleUnitType unitType,
    size_t startX,
    size_t startY,
    size_t orientation
) {

  const auto& mode = AppState::GetCurrentGameMode();
  const auto& landSegments =
      mode.isExtended ? segmentBoard->LandSegments() : std::vector<std::vector<bool>>();

  if (unitType == BattleUnitType::OperationsHeadquarter) {
    return std::all_of(
        OperationsHeadquarter::EXPECTED_COORDINATES.begin(),
        OperationsHeadquarter::EXPECTED_COORDINATES.end(),
        [&](const Coordinates& offset) {
          const auto x = startX + offset.x;
          const auto y = startY + offset.y;

          if (!CheckSegmentAvailability(segmentBoard, x, y))
            return false;

          if (mode.isExtended && !landSegments[y][x])
            return false;

          return true;
        }
    );
  }

  if (unitType == BattleUnitType::FighterJet) {
    const auto predicate = [&](const SCoordinates& offset) {
      const auto x = startX + offset.first;
      const auto y = startY + offset.second;

      return CheckSegmentAvailability(segmentBoard, x, y);
    };

    switch (orientation) {
      case 0:
        // Rotation Right
        return std::all_of(
            FighterJet::EXPECTED_COORDINATES_ROTATION_RIGHT.begin(),
            FighterJet::EXPECTED_COORDINATES_ROTATION_RIGHT.end(),
            predicate
        );

      case 1:
        // Rotation Down
        return std::all_of(
            FighterJet::EXPECTED_COORDINATES_ROTATION_DOWN.begin(),
            FighterJet::EXPECTED_COORDINATES_ROTATION_DOWN.end(),
            predicate
        );

      case 2:
        // Rotation Left
        return std::all_of(
            FighterJet::EXPECTED_COORDINATES_ROTATION_LEFT.begin(),
            FighterJet::EXPECTED_COORDINATES_ROTATION_LEFT.end(),
            predicate
        );

      case 3:
        // Rotation Up
        return std::all_of(
            FighterJet::EXPECTED_COORDINATES_ROTATION_UP.begin(),
            FighterJet::EXPECTED_COORDINATES_ROTATION_UP.end(),
            predicate
        );

      default:
        throw std::runtime_error("Invalid orientation for Fighter Jet");
    }
  }

  throw std::runtime_error("CanPlaceUnitAtSpecialCase called for unsupported unit type");
}

bool GameSetupView::CheckSegmentAvailability(ISegment* segmentBoard, size_t x, size_t y) {
  const auto& gameManager = AppState::GetCurrentGameManager();
  const auto& mode = gameManager->Mode();
  const auto& segments = segmentBoard->Segments();

  if (x >= mode.boardWidth || y >= mode.boardHeight || segments[y][x]) {
    return false;
  }

  // Check adjacent cells
  if (!CheckAdjacentCells(segmentBoard, x, y)) {
    return false;
  }

  return true;
}

void GameSetupView::PlaceUnitAt(
    ISegment* segmentBoard,
    BattleUnitType unitType,
    size_t startX,
    size_t startY,
    size_t orientation
) {
  const auto& unitSize = BattleUnitHelper::GetSizeForUnitType(unitType);

  if (unitType == BattleUnitType::OperationsHeadquarter) {
    for (const auto& offset : OperationsHeadquarter::EXPECTED_COORDINATES) {
      const auto x = startX + offset.x;
      const auto y = startY + offset.y;

      segmentBoard->ToggleSegment(x, y);
    }
    return;
  }

  if (unitType == BattleUnitType::FighterJet) {
    const std::array<SCoordinates, 5>* shape = nullptr;

    switch (orientation) {
      case 0:
        shape = &FighterJet::EXPECTED_COORDINATES_ROTATION_RIGHT;
        break;

      case 1:
        shape = &FighterJet::EXPECTED_COORDINATES_ROTATION_DOWN;
        break;

      case 2:
        shape = &FighterJet::EXPECTED_COORDINATES_ROTATION_LEFT;
        break;

      case 3:
        shape = &FighterJet::EXPECTED_COORDINATES_ROTATION_UP;
        break;

      default:
        throw std::runtime_error("Invalid orientation for Fighter Jet");
    }

    for (const auto& offset : *shape) {
      const auto x = startX + offset.first;
      const auto y = startY + offset.second;

      segmentBoard->ToggleSegment(x, y);
    }

    return;
  }

  orientation = orientation % 2; // 0 = horizontal, 1 = vertical

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

// Justification: This is recursive only if the next player is AI
// In theory this could lead to stack overflow if there are only AI players
// Will probably need to be reworked in the future to avoid that case
// NOLINTNEXTLINE(misc-no-recursion)
bool GameSetupView::ConfirmGridSetup() {
  const auto& gameManager = AppState::GetCurrentGameManager();

  if (!AllUnitsPlaced()) {
    ShowErrorMessage("Not all units have been placed!");
    return false;
  }

  // Show confirmation prompt
  // TODO: Implement confirmation prompt

  const auto& players = gameManager->Players();

  // Move to next player or finish setup
  if (currentPlayerIndex + 1 < players.size()) {
    currentPlayerIndex++;

    ForceRender();

    HandleAI();
  } else {
    // All players have set up their boards
    gameManager->StartGame();
    WindowManager::GetInstance().SwitchToWindow(WindowType::InGame);
  }

  return true;
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
  const auto& currentPlayer = gameManager->GetPlayerAtIndex(currentPlayerIndex);
  const auto& board = currentPlayer.board;
  const auto& segmentBoard = board->GetSegmentBoard();

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
