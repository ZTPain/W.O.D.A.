#include "FighterJet.h"

#include "Backend/Games/Coordinates.h"
#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"
#include <algorithm>
#include <array>
#include <vector>

FighterJet::FighterJet() {
  type = BattleUnitType::FighterJet;
  category = BattleUnitCategory::Aerial;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}

bool FighterJet::IsValidUnitShape(const std::vector<Coordinates>& segments) {
  const auto targetXY = std::min_element(
      segments.begin(), segments.end(), [](const Coordinates& a, const Coordinates& b) {
        if (a.x != b.x)
          return a.x < b.x;
        return a.y < b.y;
      }
  );

  const Coordinates origin = *targetXY;

  static const std::array<Coordinates, 5> EXPECTED_COORDINATES_ROTATION_RIGHT = {
      Coordinates(0, 0),
      Coordinates(0, 1),
      Coordinates(1, 1),
      Coordinates(2, 1),
      Coordinates(0, 2),
  };

  static const std::array<Coordinates, 5> EXPECTED_COORDINATES_ROTATION_DOWN = {
      Coordinates(0, 0),
      Coordinates(1, 0),
      Coordinates(2, 0),
      Coordinates(1, 1),
      Coordinates(1, 2),
  };

  static const std::array<Coordinates, 5> EXPECTED_COORDINATES_ROTATION_LEFT = {
      Coordinates(0, 0),
      Coordinates(1, 0),
      Coordinates(2, 0),
      Coordinates(2, -1),
      Coordinates(2, 1),
  };

  static const std::array<Coordinates, 5> EXPECTED_COORDINATES_ROTATION_UP = {
      Coordinates(0, 0),
      Coordinates(1, 0),
      Coordinates(2, 0),
      Coordinates(1, -1),
      Coordinates(1, -2),
  };

  bool allSegmentsGood = true;

  for (const auto& seg : segments) {
    const auto isInExpectedCoordinates = std::any_of(
        EXPECTED_COORDINATES_ROTATION_RIGHT.begin(),
        EXPECTED_COORDINATES_ROTATION_RIGHT.end(),
        [seg, origin](const Coordinates& cord) {
          return cord.x == seg.x + origin.x && cord.y == seg.y + origin.y;
        }
    );

    if (!isInExpectedCoordinates) {
      allSegmentsGood = false;
      break;
    }
  }

  if (allSegmentsGood)
    return true;

  allSegmentsGood = true;
  for (const auto& seg : segments) {
    const auto isInExpectedCoordinates = std::any_of(
        EXPECTED_COORDINATES_ROTATION_DOWN.begin(),
        EXPECTED_COORDINATES_ROTATION_DOWN.end(),
        [seg, origin](const Coordinates& cord) {
          return cord.x == seg.x + origin.x && cord.y == seg.y + origin.y;
        }
    );

    if (!isInExpectedCoordinates) {
      allSegmentsGood = false;
      break;
    }
  }

  if (allSegmentsGood)
    return true;

  allSegmentsGood = true;
  for (const auto& seg : segments) {
    const auto isInExpectedCoordinates = std::any_of(
        EXPECTED_COORDINATES_ROTATION_LEFT.begin(),
        EXPECTED_COORDINATES_ROTATION_LEFT.end(),
        [seg, origin](const Coordinates& cord) {
          return cord.x == seg.x + origin.x && cord.y == seg.y + origin.y;
        }
    );

    if (!isInExpectedCoordinates) {
      allSegmentsGood = false;
      break;
    }
  }

  if (allSegmentsGood)
    return true;

  allSegmentsGood = true;
  for (const auto& seg : segments) {
    const auto isInExpectedCoordinates = std::any_of(
        EXPECTED_COORDINATES_ROTATION_UP.begin(),
        EXPECTED_COORDINATES_ROTATION_UP.end(),
        [seg, origin](const Coordinates& cord) {
          return cord.x == seg.x + origin.x && cord.y == seg.y + origin.y;
        }
    );

    if (!isInExpectedCoordinates) {
      allSegmentsGood = false;
      break;
    }
  }
  return allSegmentsGood;
}
