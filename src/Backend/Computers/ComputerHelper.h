#pragma once

#include "Computer.h"

class ComputerHelper {
public:
  static const char* GetComputerTypeString(ComputerType type) {
    switch (type) {
      case ComputerType::None:
        return "None";
      case ComputerType::Easy:
        return "Easy";
      case ComputerType::Medium:
        return "Medium";
      case ComputerType::Hard:
        return "Hard";
      default:
        return "Unknown";
    }
  }
};
