#pragma once

#include "Backend/Units/BattleUnitType.h"
#include "BattleUnit.h"
#include "BattleUnitType.h"
#include "Cruiser.h"
#include "Dreadnought.h"
#include "Interceptor.h"
#include "PatrolBoat.h"
#include <cstddef>
#include <memory>

class BattleUnitHelper {
public:
  static constexpr size_t BIGGEST_UNIT_SIZE = 4;
  static constexpr size_t GetSizeForUnitType(BattleUnitType type) {
    switch (type) {
      case BattleUnitType::PatrolBoat:
        return 1;

      case BattleUnitType::Interceptor:
        return 2;

      case BattleUnitType::Cruiser:
        return 3;

      case BattleUnitType::Dreadnought:
        return 4;

      default:
        return 0;
    }
  }

  static std::shared_ptr<BattleUnit> CreateBattleUnit(BattleUnitType type) {
    switch (type) {
      case BattleUnitType::PatrolBoat:
        return std::make_shared<PatrolBoat>();

      case BattleUnitType::Interceptor:
        return std::make_shared<Interceptor>();

      case BattleUnitType::Cruiser:
        return std::make_shared<Cruiser>();

      case BattleUnitType::Dreadnought:
        return std::make_shared<Dreadnought>();

      default:
        return nullptr;
    }
  }
};
