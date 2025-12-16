// UserProfile.h

#pragma once

#include "../Computers/Computer.h"
#include "AchievementPool.h"
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

struct Statistics {
  std::chrono::seconds fastestWonGame{0};
  std::chrono::seconds totalPlaytime{0};
  unsigned int longestHitStreak = 0;
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

class UserProfile {
  unsigned int userId;
  Computer* computer;
  std::string name;
  Statistics statistics;
  std::unique_ptr<AchievementPool> achievements;
  uint64_t unlockedContent;
  UserSettings settings;

public:
  UserProfile(unsigned int userId, std::string name, std::unique_ptr<AchievementPool> achievements);
  UserProfile(
      unsigned int userId,
      std::string name,
      std::unique_ptr<AchievementPool> achievements,
      Computer& computer
  );
  ~UserProfile();
  unsigned int UserId();
  Computer* Computer();
};
