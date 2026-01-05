// BattleUnit.h

#pragma once

#include "BattleUnitType.h"

enum class BattleUnitCategory {
  None,
  Land,
  Marine,
  Aerial,
};

class BattleUnit {
  unsigned int destroyedSegments = 0;

protected:
  unsigned int segments = 0;
  BattleUnitCategory category = BattleUnitCategory::None;

public:
  virtual ~BattleUnit() = default;
  [[nodiscard]] virtual BattleUnitType GetType() const = 0;
  [[nodiscard]] BattleUnitCategory GetCategory() const;
  void DestroySegment();
  void FixSegment();
  [[nodiscard]] bool IsDestroyed() const;
};
