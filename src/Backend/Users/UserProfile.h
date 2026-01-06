// UserProfile.h

#pragma once

#include "AchievementPool.h"
#include "Backend/Computers/Computer.h"
#include <__chrono/duration.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

using namespace std::chrono_literals;

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

class UserProfile {
  unsigned int userId;
  Computer* computer;

public:
  std::string name;
  Statistics statistics;
  std::unique_ptr<AchievementPool> achievements;
  uint64_t unlockedContent;
  UserSettings settings;

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
