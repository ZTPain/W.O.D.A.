// AchievementPool.h

#pragma once

#include "IClone.h"
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
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

  Achievement(
      std::string_view name, std::string_view description, UnlockableContent content, bool unlocked
  );
};

class AchievementPool : IClone<AchievementPool> {
  std::unordered_map<std::string, Achievement> nameToAchievementMap;
  void InitMapElement(
      std::string_view name, std::string_view description, UnlockableContent content
  );

public:
  AchievementPool();
  [[nodiscard]] const std::unordered_map<std::string, Achievement>& NameToAchievementMap() const;
  void Unlock(const std::string& name);
  std::unique_ptr<AchievementPool> Clone() override;
};
