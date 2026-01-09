#include "POCGameView.h"
#include "Backend/Boards/SegmentBoard.h"
#include "Backend/Boards/SegmentBoardValidator.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/BattleUnitHelper.h"
#include "Backend/Units/BattleUnitType.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/InteractiveGrid.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <vector>

POCGameView::POCGameView() : Window(WindowType::InGame) {}

POCGameView::~POCGameView() = default;

static int redX = -1;
static int redY = -1;

static constexpr int GRID_WIDTH = 10;
static constexpr int GRID_HEIGHT = 10;
static SegmentBoard segmentBoard = SegmentBoard(GRID_WIDTH, GRID_HEIGHT);
static GameMode demoMode = GameMode{
    "Demo Mode",
    "A simple demo mode for testing.",
    static_cast<size_t>(GRID_WIDTH),
    static_cast<size_t>(GRID_HEIGHT),
    false,
    FireCommandType::FireCommand,
    {
                   {BattleUnitType::PatrolBoat, 4},
                   {BattleUnitType::Interceptor, 3},
                   {BattleUnitType::Cruiser, 2},
                   {BattleUnitType::Dreadnought, 1},
                   }
};
static SegmentBoardValidator segmentBoardValidator = SegmentBoardValidator(segmentBoard, demoMode);

static void RenderEmptyCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) {
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

static AnsiColor GetColorForUnitType(BattleUnitType type) {
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

static BattleUnitType GetUnitTypeOfCoordinate(const Coordinates& coord) {
  const auto& units = segmentBoardValidator.GetUnits();

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

static void RenderFilledCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) {
  IO::cout << AnsiHelper::MoveCursor(posX, posY);

  const auto currentCoord = Coordinates(x, y);

  const auto unitType = GetUnitTypeOfCoordinate(currentCoord);
  const auto color = GetColorForUnitType(unitType);

  IO::cout << AnsiHelper::SetBackgroundColor(color);

  if (redX == static_cast<int>(x) && redY == static_cast<int>(y)) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red);
  }

  const char* symbol = "?";

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

static void RenderCell(size_t x, size_t y, size_t posX, size_t posY, bool isCursor) {
  if (segmentBoardValidator.Segments()[y][x]) {
    RenderFilledCell(x, y, posX, posY, isCursor);
  } else {
    RenderEmptyCell(x, y, posX, posY, isCursor);
  }
}

static void OnToggleCell(size_t x, size_t y, size_t /*posX*/, size_t /*posY*/);
static Grid grid(2, 5, GRID_WIDTH, GRID_HEIGHT, 3, 1, RenderCell, OnToggleCell);

static void RenderUnitsLeft(const std::unordered_map<BattleUnitType, size_t>& unitPool) {
  IO::cout << AnsiHelper::MoveCursor(grid.GetTotalWidth() + 8, 5) << "Units left: ";
  auto i = 1;
  for (const auto& [unitType, count] : unitPool) {

    const auto color = GetColorForUnitType(unitType);
    const auto* const name = BattleUnitHelper::GetNameForUnitType(unitType);

    const auto unitsPlaced = segmentBoardValidator.GetUnits().at(unitType);

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

static void OnToggleCell(size_t x, size_t y, size_t /*posX*/, size_t /*posY*/) {
  if (segmentBoardValidator.ToggleSegment(x, y)) {
    grid.Render();

    IO::cout << AnsiHelper::MoveCursor(1, 3) << ANSI_CLEAR_LINE;
    RenderUnitsLeft(demoMode.unitPool);
  } else {
    // Invalid toggle
    IO::cout << AnsiHelper::MoveCursor(1, 3) << AnsiHelper::SetTextColor(AnsiColor::Red)
             << "Invalid segment toggle!" << AnsiHelper::Reset();
    redX = static_cast<int>(x);
    redY = static_cast<int>(y);
  }
}

void POCGameView::OnEnter() {
  // Initialize game grid
  segmentBoardValidator.Clear();

  ForceRender();
}

void POCGameView::OnExit() {}

bool POCGameView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  redX = -1;
  redY = -1;
  grid.OnKeyPressed(keyDetails);
  if (keyDetails.key == ConsoleKey::Escape) {
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }
  return false;
}

void POCGameView::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

void POCGameView::ForceRender() {

  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();
  BoxDrawing::DrawBox(
      1,
      4,
      ((GRID_WIDTH + 1) * 5) - 1,
      ((GRID_HEIGHT + 1) * 2) + 1,
      BoxStyle::Single,
      true,
      "Game View"
  );
  grid.Render();
  RenderUnitsLeft(demoMode.unitPool);
}

bool POCGameView::IsCorrectSize(int width, int height) const {
  const int requiredWidth = ((GRID_WIDTH + 1) * 5) + 4;
  const int requiredHeight = ((GRID_HEIGHT + 1) * 2) + 4;
  return width >= requiredWidth && height >= requiredHeight;
}
