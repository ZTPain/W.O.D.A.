// UserProfile.h

#pragma once

#include "AchievementPool.h"
#include "Backend/Computers/Computer.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

using std::chrono_literals::operator""s;

struct Statistics {
  std::chrono::seconds fastestWonGame = 0s;
  std::chrono::seconds totalPlaytime = 0s;
  unsigned int gamesPlayed = 0;
  unsigned int gamesWon = 0;
  unsigned int gamesLost = 0;
  unsigned int highestScore = 0;
  unsigned int totalShotsFired = 0;
  unsigned int totalShotsHit = 0;
  unsigned int totalUnitsDestroyed = 0;
};

enum class Color {
  Default,
};

enum class BorderPattern {
  Default,
};

enum class UnitPattern {
  Default,
};

struct UserSettings {
  Color unitColor = Color::Default;
  Color boardColor = Color::Default;
  Color borderColor = Color::Default;
  BorderPattern borderPattern = BorderPattern::Default;
  UnitPattern unitPattern = UnitPattern::Default;
  bool autoMarkEmptyFields = false;
};

using PlayerId = uint32_t;

class UserProfile {
  PlayerId userId;
  Computer* ai;

  UserProfile(
      PlayerId userId,
      std::string name,
      Statistics statistics,
      std::unique_ptr<AchievementPool> achievements,
      uint64_t unlockedContent,
      UserSettings settings,
      Computer* ai
  );

public:
  std::string name;
  Statistics statistics;
  std::unique_ptr<AchievementPool> achievements;
  uint64_t unlockedContent;
  UserSettings settings;
  UserProfile(const UserProfile& other);
  UserProfile(
      PlayerId userId, const std::string& name, std::unique_ptr<AchievementPool> achievements
  );
  UserProfile(
      PlayerId userId,
      const std::string& name,
      std::unique_ptr<AchievementPool> achievements,
      Computer* ai
  );
  ~UserProfile();
  [[nodiscard]] PlayerId UserId() const;
  [[nodiscard]] Computer* AI() const;

  size_t Serialize(uint8_t* buffer, size_t offset, size_t bufferSize) const;
  static UserProfile Deserialize(
      const uint8_t* buffer, size_t offset, size_t bufferSize, size_t& bytesRead
  );

private:
  void SerializeStatistics(uint8_t* buffer, size_t& offset, size_t bufferSize) const;
  void SerializeSettings(uint8_t* buffer, size_t& offset, size_t bufferSize) const;
  static void DeserializeStatistics(
      const uint8_t* buffer, size_t& offset, size_t bufferSize, Statistics& statistics
  );
  static void DeserializeSettings(
      const uint8_t* buffer, size_t& offset, size_t bufferSize, UserSettings& settings
  );
};
