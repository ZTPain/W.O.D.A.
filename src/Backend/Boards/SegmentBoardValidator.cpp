#include "SegmentBoardValidator.h"

#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/BattleUnitHelper.h"
#include "Backend/Units/BattleUnitType.h"
#include "ISegment.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

static void GetCoordinatesOfFilledSegments(
    const ISegment& segmentBoard, std::vector<Coordinates>& outCoordinates
) {
  outCoordinates.clear();
  auto segments = segmentBoard.Segments();
  for (size_t y = 0; y < segmentBoard.Height(); ++y) {
    for (size_t x = 0; x < segmentBoard.Width(); ++x) {
      if (!segments[y][x])
        continue;

      outCoordinates.emplace_back(x, y);
    }
  }
}

static void GroupCoordinates(
    const std::vector<Coordinates>& inCoordinates, std::vector<std::vector<Coordinates>>& outGroups
) {
  outGroups.clear();

  std::vector<bool> visited(inCoordinates.size(), false);
  for (size_t i = 0; i < inCoordinates.size(); ++i) {
    if (visited[i])
      continue;

    std::vector<Coordinates> group;
    std::stack<size_t> stack;
    stack.push(i);

    while (!stack.empty()) {
      size_t const index = stack.top();
      stack.pop();

      if (visited[index])
        continue;

      visited[index] = true;
      group.push_back(inCoordinates[index]);

      const auto& coord = inCoordinates[index];
      const auto coordIntX = static_cast<int>(coord.x);
      const auto coordIntY = static_cast<int>(coord.y);

      for (size_t j = 0; j < inCoordinates.size(); ++j) {
        if (visited[j])
          continue;

        const auto& neighbor = inCoordinates[j];
        const auto neighborIntX = static_cast<int>(neighbor.x);
        const auto neighborIntY = static_cast<int>(neighbor.y);

        if ((coord.x == neighbor.x && std::abs(coordIntY - neighborIntY) == 1) ||
            (coord.y == neighbor.y && std::abs(coordIntX - neighborIntX) == 1)) {
          stack.push(j);
        }
      }
    }

    outGroups.push_back(group);
  }
}

static void GroupsToUnits(
    const std::vector<std::vector<Coordinates>>& inGroups,
    const GameMode& mode,
    std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>& outUnits
) {
  outUnits.clear();

  outUnits[BattleUnitType::None] = {};
  for (const auto& [unitType, _] : mode.unitPool) {
    outUnits[unitType] = {};
  }

  for (const auto& group : inGroups) {
    BattleUnitType unitType = BattleUnitType::None;
    const auto size = group.size();
    const auto predicate = [&](const std::pair<const BattleUnitType, unsigned long long>& c) {
      return BattleUnitHelper::GetSizeForUnitType(c.first) == size;
    };

    const auto result = std::find_if(mode.unitPool.begin(), mode.unitPool.end(), predicate);
    if (result != mode.unitPool.end()) {
      unitType = result->first;
    }

    const auto it = outUnits.find(unitType);
    if (it == outUnits.end())
      throw std::logic_error("Invalid unit type found during grouping! (HOW?)");

    if (unitType != BattleUnitType::None && it->second.size() >= mode.unitPool.at(unitType)) {
      unitType = BattleUnitType::None;
    }

    outUnits.at(unitType).push_back(group);
  }

  for (const auto& [unitType, count] : mode.unitPool) {
    for (size_t i = outUnits[unitType].size(); i < count; i++) {
      outUnits[unitType].emplace_back();
    }
  }
}

static bool ValidateMaxTotalSegments(
    const std::vector<Coordinates>& coordinates, const GameMode& mode
) {
  size_t maxSegments = 0;
  for (const auto& [unitType, count] : mode.unitPool) {
    maxSegments += BattleUnitHelper::GetSizeForUnitType(unitType) * count;
  }

  const size_t currentSegments = coordinates.size();

  return currentSegments <= maxSegments;
}

static bool ValidateMaxGroupSize(const std::vector<std::vector<Coordinates>>& groups) {
  return std::all_of(groups.begin(), groups.end(), [](const std::vector<Coordinates>& group) {
    return group.size() <= BattleUnitHelper::BIGGEST_UNIT_SIZE;
  });
}

SegmentBoardValidator::SegmentBoardValidator(ISegment& segmentBoard, const GameMode& mode)
    : segmentBoard(segmentBoard), mode(mode) {

  static std::vector<Coordinates> coordinates;
  GetCoordinatesOfFilledSegments(segmentBoard, coordinates);

  static std::vector<std::vector<Coordinates>> groups;
  GroupCoordinates(coordinates, groups);

  GroupsToUnits(groups, mode, lastUnits);
}

bool SegmentBoardValidator::ToggleSegment(size_t x, size_t y) {
  if (x >= Width() || y >= Height())
    return false;

  if (segmentBoard.Segments()[y][x]) {
    // Always allow unsetting segments
    return segmentBoard.ToggleSegment(x, y);
  }

  static std::vector<Coordinates> coordinates;
  GetCoordinatesOfFilledSegments(segmentBoard, coordinates);
  coordinates.emplace_back(x, y);

  if (!ValidateMaxTotalSegments(coordinates, mode)) {
    return false;
  }

  static std::vector<std::vector<Coordinates>> groups;
  GroupCoordinates(coordinates, groups);

  if (!ValidateMaxGroupSize(groups)) {
    return false;
  }

  if (!segmentBoard.ToggleSegment(x, y))
    return false;

  GroupsToUnits(groups, mode, lastUnits);
  return true;
}

void SegmentBoardValidator::Clear() {
  segmentBoard.Clear();

  GroupsToUnits({}, mode, lastUnits);
}

const UnitsMap& SegmentBoardValidator::GetUnits() const { return lastUnits; }

size_t SegmentBoardValidator::Width() const { return segmentBoard.Width(); }

size_t SegmentBoardValidator::Height() const { return segmentBoard.Height(); }

const std::vector<std::vector<bool>>& SegmentBoardValidator::Segments() const {
  return segmentBoard.Segments();
}

std::unique_ptr<ISegment> SegmentBoardValidator::Clone() const {
  return std::make_unique<SegmentBoardValidator>(segmentBoard, mode);
}
