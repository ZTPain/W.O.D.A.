// PatrolBoat.h

#pragma once

#include "BattleUnit.h"
#include "BattleUnitType.h"

class PatrolBoat : public BattleUnit {
public:
  PatrolBoat();
  [[nodiscard]] BattleUnitType GetType() const override;
};
