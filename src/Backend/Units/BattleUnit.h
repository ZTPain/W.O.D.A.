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
  BattleUnitType type = BattleUnitType::None;
  BattleUnitCategory category = BattleUnitCategory::None;

public:
  virtual ~BattleUnit() = default;
  [[nodiscard]] BattleUnitType GetType() const;
  [[nodiscard]] BattleUnitCategory GetCategory() const;
  [[nodiscard]] unsigned int GetTotalSegments() const;
  [[nodiscard]] unsigned int GetDestroyedSegments() const;
  void DestroySegment();
  void FixSegment();
  void Reset();
  [[nodiscard]] bool IsDestroyed() const;
};
