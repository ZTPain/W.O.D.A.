// PatrolBoat.h

#pragma once

#include "BattleUnit.h"

class PatrolBoat : public BattleUnit {
public:
  PatrolBoat();
  [[nodiscard]] BattleUnitType GetType() const override;
};
