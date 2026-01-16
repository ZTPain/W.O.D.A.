#pragma once

#include "ArmoredTrain.h"
#include "Backend/Units/BattleUnitType.h"
#include "BattleUnit.h"
#include "BattleUnitType.h"
#include "Cruiser.h"
#include "Dreadnought.h"
#include "FighterJet.h"
#include "GrenadeLauncher.h"
#include "InfantrySquadron.h"
#include "Interceptor.h"
#include "MobileArtillery.h"
#include "OperationsHeadquarter.h"
#include "PatrolBoat.h"
#include <cstddef>
#include <memory>

class BattleUnitHelper {
public:
  static constexpr size_t BIGGEST_UNIT_SIZE = 5;
  static constexpr size_t GetSizeForUnitType(BattleUnitType type) {
    switch (type) {
      case BattleUnitType::PatrolBoat:
      case BattleUnitType::InfantrySquadron:
        return 1;

      case BattleUnitType::Interceptor:
      case BattleUnitType::GrenadeLauncher:
        return 2;

      case BattleUnitType::Cruiser:
      case BattleUnitType::MobileArtillery:
        return 3;

      case BattleUnitType::Dreadnought:
      case BattleUnitType::ArmoredTrain:
      case BattleUnitType::OperationsHeadquarter:
        return 4;

      case BattleUnitType::FighterJet:
        return 5;

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

      case BattleUnitType::InfantrySquadron:
        return std::make_shared<InfantrySquadron>();

      case BattleUnitType::GrenadeLauncher:
        return std::make_shared<GrenadeLauncher>();

      case BattleUnitType::MobileArtillery:
        return std::make_shared<MobileArtillery>();

      case BattleUnitType::ArmoredTrain:
        return std::make_shared<ArmoredTrain>();

      case BattleUnitType::OperationsHeadquarter:
        return std::make_shared<OperationsHeadquarter>();

      case BattleUnitType::FighterJet:
        return std::make_shared<FighterJet>();

      default:
        return nullptr;
    }
  }

  static const char* GetNameForUnitType(BattleUnitType type) {
    switch (type) {
      case BattleUnitType::PatrolBoat:
        return "Patrol Boat";

      case BattleUnitType::Interceptor:
        return "Interceptor";

      case BattleUnitType::Cruiser:
        return "Cruiser";

      case BattleUnitType::Dreadnought:
        return "Dreadnought";

      case BattleUnitType::InfantrySquadron:
        return "Infantry Squadron";

      case BattleUnitType::GrenadeLauncher:
        return "Grenade Launcher";

      case BattleUnitType::MobileArtillery:
        return "Mobile Artillery";

      case BattleUnitType::ArmoredTrain:
        return "Armored Train";

      case BattleUnitType::OperationsHeadquarter:
        return "Operations Headquarter";

      case BattleUnitType::FighterJet:
        return "Fighter Jet";

      default:
        return "Unknown";
    }
  }
};
