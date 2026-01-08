#include "SegmentBoardValidator.h"

#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/BattleUnitHelper.h"
#include "Backend/Units/BattleUnitType.h"
#include "ISegment.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

static void ConcentrateCoordinates(
    const ISegment& segmentBoard, std::vector<Coordinates>& outCoordinates
) {
  outCoordinates.clear();
  for (size_t y = 0; y < segmentBoard.Height(); ++y) {
    for (size_t x = 0; x < segmentBoard.Width(); ++x) {
      if (!segmentBoard.Segments()[y][x])
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

SegmentBoardValidator::SegmentBoardValidator(ISegment& segmentBoard, const GameMode& mode)
    : segmentBoard(segmentBoard), mode(mode) {}

bool SegmentBoardValidator::ToggleSegment(size_t x, size_t y) {
  if (x >= Width() || y >= Height())
    return false;

  if (segmentBoard.Segments()[y][x]) {
    // Always allow unsetting segments
    return segmentBoard.ToggleSegment(x, y);
  }

  std::vector<Coordinates> concentrated;
  ConcentrateCoordinates(segmentBoard, concentrated);
  concentrated.emplace_back(x, y);
  std::vector<std::vector<Coordinates>> groups;
  GroupCoordinates(concentrated, groups);

  for (const auto& group : groups) {
    if (group.size() > BattleUnitHelper::BIGGEST_UNIT_SIZE) {
      return false;
    }
  }

  return segmentBoard.ToggleSegment(x, y);
}

void SegmentBoardValidator::Clear() { segmentBoard.Clear(); }

void SegmentBoardValidator::GetUnits(
    std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>& outUnits
) const {
  static std::vector<Coordinates> concentrated;
  ConcentrateCoordinates(segmentBoard, concentrated);

  static std::vector<std::vector<Coordinates>> outGroups;
  GroupCoordinates(concentrated, outGroups);

  GroupsToUnits(outGroups, mode, outUnits);
}

size_t SegmentBoardValidator::Width() const { return segmentBoard.Width(); }

size_t SegmentBoardValidator::Height() const { return segmentBoard.Height(); }

const std::vector<std::vector<bool>>& SegmentBoardValidator::Segments() const {
  return segmentBoard.Segments();
}
