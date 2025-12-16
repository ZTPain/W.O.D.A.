// GameMode.h

#pragma once

#include "../Units/BattleUnitType.h"
#include <cstddef>
#include <string>
#include <unordered_map>

enum class FireCommandType {
  FireCommand,
  SalvoFireCommand,
};

struct GameMode {
  std::string name;
  std::string description;
  size_t boardWidth;
  size_t boardHeight;
  bool isExtended;
  FireCommandType commandType;
  std::unordered_map<BattleUnitType, size_t> unitPool;
};
