#include "SegmentBoardValidator.h"

#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/ArmoredTrain.h"
#include "Backend/Units/BattleUnit.h"
#include "Backend/Units/BattleUnitHelper.h"
#include "Backend/Units/BattleUnitType.h"
#include "Backend/Units/FighterJet.h"
#include "Backend/Units/OperationsHeadquarter.h"
#include "ISegment.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iterator>
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

static bool HandleSpecialShapeUnits(
    BattleUnitType unitType, const std::vector<Coordinates>& group
) {
  switch (unitType) {
    case BattleUnitType::ArmoredTrain:
      return ArmoredTrain::IsValidUnitShape(group);

    case BattleUnitType::OperationsHeadquarter:
      return OperationsHeadquarter::IsValidUnitShape(group);

    case BattleUnitType::FighterJet:
      return FighterJet::IsValidUnitShape(group);

    default:
      return true;
  }
}

static bool ValidateUnitCategory(
    BattleUnitCategory unitCategory,
    const std::vector<Coordinates>& group,
    const std::vector<std::vector<bool>>& landSegments
) {
  switch (unitCategory) {
    case BattleUnitCategory::Land:
      return std::all_of(group.begin(), group.end(), [&](const Coordinates& coord) {
        return landSegments[coord.y][coord.x];
      });

    case BattleUnitCategory::Marine:
      return std::all_of(group.begin(), group.end(), [&](const Coordinates& coord) {
        return !landSegments[coord.y][coord.x];
      });

    case BattleUnitCategory::Aerial:
      return true;

    default:
      throw std::logic_error("Unknown BattleUnitCategory encountered during validation!");
  }
}

static void GroupsToUnits(
    const std::vector<std::vector<Coordinates>>& inGroups,
    const GameMode& mode,
    const std::vector<std::vector<bool>>& landSegments,
    std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>& outUnits
) {
  outUnits.clear();

  outUnits[BattleUnitType::None] = {{}};
  for (const auto& [unitType, _] : mode.unitPool) {
    outUnits[unitType] = {};
  }

  for (const auto& group : inGroups) {
    BattleUnitType unitType = BattleUnitType::None;
    const auto size = group.size();
    const auto predicate = [&](const std::pair<const BattleUnitType, unsigned long long>& c) {
      const auto category = BattleUnitHelper::GetCategoryForUnitType(c.first);
      if (mode.isExtended && !ValidateUnitCategory(category, group, landSegments))
        return false;

      return BattleUnitHelper::GetSizeForUnitType(c.first) == size;
    };

    auto result = mode.unitPool.begin();
    while (true) {
      result = std::find_if(result, mode.unitPool.end(), predicate);

      if (result == mode.unitPool.end())
        break;

      unitType = result->first;

      if (!HandleSpecialShapeUnits(unitType, group)) {
        std::advance(result, 1);
        continue;
      }

      const auto it = outUnits.find(unitType);
      if (it == outUnits.end())
        throw std::logic_error("Invalid unit type found during grouping! (HOW?)");

      if (unitType != BattleUnitType::None && it->second.size() >= mode.unitPool.at(unitType)) {
        unitType = BattleUnitType::None;
      }

      outUnits.at(unitType).push_back(group);

      std::advance(result, 1);
    }
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

  GroupsToUnits(
      groups,
      mode,
      mode.isExtended ? segmentBoard.LandSegments() : std::vector<std::vector<bool>>(),
      lastUnits
  );
}

bool SegmentBoardValidator::ToggleSegment(size_t x, size_t y) {
  if (x >= Width() || y >= Height())
    return false;

  static std::vector<Coordinates> coordinates;
  GetCoordinatesOfFilledSegments(segmentBoard, coordinates);

  const bool isSet = segmentBoard.Segments()[y][x];
  if (isSet) {
    if (!segmentBoard.ToggleSegment(x, y))
      return false;
    coordinates.erase(
        std::remove(coordinates.begin(), coordinates.end(), Coordinates{x, y}), coordinates.end()
    );
  } else {
    coordinates.emplace_back(x, y);
  }

  if (!ValidateMaxTotalSegments(coordinates, mode)) {
    return false;
  }

  static std::vector<std::vector<Coordinates>> groups;
  GroupCoordinates(coordinates, groups);

  if (!ValidateMaxGroupSize(groups)) {
    return false;
  }

  if (!isSet && !segmentBoard.ToggleSegment(x, y))
    return false;

  GroupsToUnits(
      groups,
      mode,
      mode.isExtended ? segmentBoard.LandSegments() : std::vector<std::vector<bool>>(),
      lastUnits
  );

  return true;
}

void SegmentBoardValidator::Clear() {
  segmentBoard.Clear();

  GroupsToUnits({}, mode, std::vector<std::vector<bool>>(), lastUnits);
}

const UnitsMap& SegmentBoardValidator::GetUnits() const { return lastUnits; }

size_t SegmentBoardValidator::Width() const { return segmentBoard.Width(); }

size_t SegmentBoardValidator::Height() const { return segmentBoard.Height(); }

const std::vector<std::vector<bool>>& SegmentBoardValidator::Segments() const {
  return segmentBoard.Segments();
}

const std::vector<std::vector<bool>>& SegmentBoardValidator::LandSegments() const {
  return segmentBoard.LandSegments();
}

void SegmentBoardValidator::LoadLandSegments(const std::vector<std::vector<bool>>& landSegments) {
  segmentBoard.LoadLandSegments(landSegments);
}
