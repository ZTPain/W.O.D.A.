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

  if (BattleUnitHelper::IsValidShape(EXPECTED_COORDINATES_ROTATION_RIGHT, segments, origin))
    return true;

  if (BattleUnitHelper::IsValidShape(EXPECTED_COORDINATES_ROTATION_DOWN, segments, origin))
    return true;

  if (BattleUnitHelper::IsValidShape(EXPECTED_COORDINATES_ROTATION_LEFT, segments, origin))
    return true;

  if (BattleUnitHelper::IsValidShape(EXPECTED_COORDINATES_ROTATION_UP, segments, origin))
    return true;

  return false;
}
