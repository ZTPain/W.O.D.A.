// FighterJet.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "BattleUnit.h"
#include <array>
#include <corecrt.h>
#include <cstddef>
#include <utility>
#include <vector>

using SCoordinates = std::pair<ssize_t, ssize_t>;

class FighterJet : public BattleUnit {
public:
  FighterJet();
  static bool IsValidUnitShape(const std::vector<Coordinates>& segments);

  static inline const std::array<SCoordinates, 5> EXPECTED_COORDINATES_ROTATION_RIGHT = {
      SCoordinates(0, 0),
      SCoordinates(0, 1),
      SCoordinates(1, 1),
      SCoordinates(2, 1),
      SCoordinates(0, 2),
  };

  static inline const std::array<SCoordinates, 5> EXPECTED_COORDINATES_ROTATION_DOWN = {
      SCoordinates(0, 0),
      SCoordinates(1, 0),
      SCoordinates(2, 0),
      SCoordinates(1, 1),
      SCoordinates(1, 2),
  };

  static inline const std::array<SCoordinates, 5> EXPECTED_COORDINATES_ROTATION_LEFT = {
      SCoordinates(0, 0),
      SCoordinates(1, 0),
      SCoordinates(2, 0),
      SCoordinates(2, -1),
      SCoordinates(2, 1),
  };

  static inline const std::array<SCoordinates, 5> EXPECTED_COORDINATES_ROTATION_UP = {
      SCoordinates(0, 0),
      SCoordinates(1, 0),
      SCoordinates(2, 0),
      SCoordinates(1, -1),
      SCoordinates(1, -2),
  };
};
