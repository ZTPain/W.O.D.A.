#include "OperationsHeadquarter.h"

#include "Backend/Games/Coordinates.h"
#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"
#include <algorithm>
#include <vector>

OperationsHeadquarter::OperationsHeadquarter() {
  type = BattleUnitType::OperationsHeadquarter;
  category = BattleUnitCategory::Land;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}

bool OperationsHeadquarter::IsValidUnitShape(const std::vector<Coordinates>& segments) {
  const auto targetXY = std::min_element(
      segments.begin(), segments.end(), [](const Coordinates& a, const Coordinates& b) {
        if (a.x != b.x)
          return a.x < b.x;
        return a.y < b.y;
      }
  );

  const Coordinates origin = *targetXY;

  return BattleUnitHelper::IsValidShape(EXPECTED_COORDINATES, segments, origin);
}
