#include "GameBoard.h"
#include "Backend/Boards/SegmentBoard.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/BattleUnit.h"
#include "Backend/Units/BattleUnitHelper.h"
#include "ISegment.h"
#include "SegmentBoardValidator.h"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <vector>

GameBoard::GameBoard(const GameMode& mode)
    : mode(mode), segmentBoard(std::make_unique<SegmentBoard>(mode.boardWidth, mode.boardHeight)),
      segmentValidator(std::make_unique<SegmentBoardValidator>(*segmentBoard, mode)),
      units(mode.boardHeight, std::vector<std::shared_ptr<BattleUnit>>(mode.boardWidth, nullptr)) {}

GameBoard::GameBoard(const GameBoard& other)
    : mode(other.mode), segmentBoard(other.segmentBoard->Clone()),
      segmentValidator(std::make_unique<SegmentBoardValidator>(*segmentBoard, mode)),
      units(other.units), allUnits(other.allUnits) {}

void GameBoard::ParseSegments() {
  allUnits.clear();

  const UnitsMap& unitsMap = segmentValidator->GetUnits();
  for (const auto& [unitType, groups] : unitsMap) {
    for (const auto& group : groups) {
      const auto battleUnit = BattleUnitHelper::CreateBattleUnit(unitType);

      allUnits.push_back(battleUnit);

      for (const auto& coord : group) {
        units[coord.y][coord.x] = battleUnit;
      }
    }
  }

  segmentValidator->Clear();
}

bool GameBoard::FireAt(size_t x, size_t y) {
  if (x >= mode.boardWidth || y >= mode.boardHeight)
    return false;

  if (segmentBoard->Segments()[y][x])
    return false;

  segmentBoard->ToggleSegment(x, y);

  if (units[y][x] != nullptr) {
    units[y][x]->DestroySegment();
  }

  return true;
}

void GameBoard::FixSegment(size_t x, size_t y) {
  if (x >= mode.boardWidth || y >= mode.boardHeight)
    return;

  if (!segmentBoard->Segments()[y][x])
    return;

  segmentBoard->ToggleSegment(x, y);

  if (units[y][x] != nullptr) {
    units[y][x]->FixSegment();
  }
}

bool GameBoard::IsGameOver() const {
  return std::all_of(allUnits.begin(), allUnits.end(), [](const std::shared_ptr<BattleUnit>& item) {
    return item == nullptr || item->IsDestroyed();
  });
}

size_t GameBoard::Width() const { return mode.boardWidth; }
size_t GameBoard::Height() const { return mode.boardHeight; }

ISegment& GameBoard::GetSegmentBoard() const {
  // If no units have been placed yet, this means we are still in the setup phase
  if (allUnits.empty()) {
    return *segmentValidator;
  }

  return *segmentBoard;
}

const std::vector<std::shared_ptr<BattleUnit>>& GameBoard::GetAllUnits() const { return allUnits; }
