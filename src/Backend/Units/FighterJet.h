// FighterJet.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "BattleUnit.h"
#include <array>
#include <vector>

class FighterJet : public BattleUnit {
public:
  FighterJet();
  static bool IsValidUnitShape(const std::vector<Coordinates>& segments);

  static inline const std::array<Coordinates, 5> EXPECTED_COORDINATES_ROTATION_RIGHT = {
      Coordinates(0, 0),
      Coordinates(0, 1),
      Coordinates(1, 1),
      Coordinates(2, 1),
      Coordinates(0, 2),
  };

  static inline const std::array<Coordinates, 5> EXPECTED_COORDINATES_ROTATION_DOWN = {
      Coordinates(0, 0),
      Coordinates(1, 0),
      Coordinates(2, 0),
      Coordinates(1, 1),
      Coordinates(1, 2),
  };

  static inline const std::array<Coordinates, 5> EXPECTED_COORDINATES_ROTATION_LEFT = {
      Coordinates(0, 0),
      Coordinates(1, 0),
      Coordinates(2, 0),
      Coordinates(2, -1),
      Coordinates(2, 1),
  };

  static inline const std::array<Coordinates, 5> EXPECTED_COORDINATES_ROTATION_UP = {
      Coordinates(0, 0),
      Coordinates(1, 0),
      Coordinates(2, 0),
      Coordinates(1, -1),
      Coordinates(1, -2),
  };
};
