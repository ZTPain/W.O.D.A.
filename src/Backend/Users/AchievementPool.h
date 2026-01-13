// AchievementPool.h

#pragma once

#include "IClone.h"
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

enum class UnlockableContent : uint64_t {
  None = 0x0,
  MediumComputer = 0x1,
  HardComputer = 0x1 << 1,
  SalvoMode = 0x1 << 2,
  ExtendedMode = 0x1 << 3,
  FlowerShipIcon = 0x1 << 4,
  CrosshairShipIcon = 0x1 << 5,
  StarShipIcon = 0x1 << 6,
  StoneShipIcon = 0x1 << 7,
  RedBorderColor = 0x1 << 8,
  BlueBorderColor = 0x1 << 9,
};

static inline uint64_t operator|(uint64_t a, UnlockableContent b) {
  return (a | static_cast<uint64_t>(b));
}

inline static bool operator&(uint64_t a, UnlockableContent b) {
  return (a & static_cast<uint64_t>(b)) != 0;
}

struct Achievement {
  std::string name;
  std::string description;
  UnlockableContent content;
  bool unlocked;

  // Not planning on runtime dynamic name constructed Achievement struct, thats why there will be no
  // std::string version
  Achievement(
      std::string_view name, std::string_view description, UnlockableContent content, bool unlocked
  );
};

class AchievementPool : IClone<AchievementPool> {
  std::unordered_map<std::string, Achievement> nameToAchievementMap;

  // Use for const initialization (hence std::string_view). It is not constexpr initialization,
  // due to compile-time unordered_maps being a C++20 feature.
  void InitMapElement(
      std::string_view name, std::string_view description, UnlockableContent content
  );

public:
  AchievementPool();
  [[nodiscard]] const std::unordered_map<std::string, Achievement>& NameToAchievementMap() const;
  void Unlock(const std::string& name);
  std::unique_ptr<AchievementPool> Clone() override;
};
