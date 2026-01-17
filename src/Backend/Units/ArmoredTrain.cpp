#include "ArmoredTrain.h"

#include "Backend/Games/Coordinates.h"
#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"
#include <algorithm>
#include <cstddef>
#include <vector>

ArmoredTrain::ArmoredTrain() {
  type = BattleUnitType::ArmoredTrain;
  category = BattleUnitCategory::Land;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}

bool ArmoredTrain::IsValidUnitShape(const std::vector<Coordinates>& segments) {
  const size_t targetX = segments[0].x;
  const size_t targetY = segments[0].y;

  if (std::all_of(segments.begin(), segments.end(), [targetX](const Coordinates& cord) {
        return cord.x == targetX;
      }))
    return true;

  if (std::all_of(segments.begin(), segments.end(), [targetY](const Coordinates& cord) {
        return cord.y == targetY;
      }))
    return true;

  return false;
}
