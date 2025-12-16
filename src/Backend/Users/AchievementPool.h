// AchievementPool.h

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

enum class UnlockableContent : uint64_t {
  None = 0,
  MediumComputer = 1,
  HardComputer = 2,
  SalvoMode = 4,
  ExtendedMode = 8,
};

struct Achievement {
  std::string name;
  std::string description;
  UnlockableContent content;
  bool unlocked;
};

class AchievementPool {
  std::unordered_map<std::string, Achievement> nameToAchievementMap;

public:
  AchievementPool();
  [[nodiscard]] const std::unordered_map<std::string, Achievement>& NameToAchievementMap() const;
  bool Unlock(std::string name);
};
