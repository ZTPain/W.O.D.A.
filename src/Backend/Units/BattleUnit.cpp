#include "BattleUnit.h"

#include "BattleUnitType.h"

BattleUnitCategory BattleUnit::GetCategory() const { return category; }

BattleUnitType BattleUnit::GetType() const { return type; }

void BattleUnit::DestroySegment() {
  if (destroyedSegments < segments) {
    destroyedSegments++;
  }
}

void BattleUnit::FixSegment() {
  if (destroyedSegments != 0) {
    destroyedSegments--;
  }
}

bool BattleUnit::IsDestroyed() const { return destroyedSegments == segments; }
