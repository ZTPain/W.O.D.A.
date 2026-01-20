#include "UserProfile.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Users/AchievementPool.h"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

UserProfile::UserProfile() : UserProfile(0, "", nullptr) {}

UserProfile::UserProfile(const UserProfile& other)
    : UserProfile(
          other.userId,
          other.name,
          other.statistics,
          other.achievements->Clone(other.userId),
          other.unlockedContent,
          other.settings,
          other.ai
      ) {}

UserProfile::UserProfile(
    PlayerId userId, const std::string& name, std::unique_ptr<AchievementPool> achievements
)
    : UserProfile(userId, name, {}, std::move(achievements), 0, UserSettings{}, nullptr) {}

UserProfile::UserProfile(
    PlayerId userId,
    const std::string& name,
    std::unique_ptr<AchievementPool> achievements,
    class Computer* ai
)
    : UserProfile(userId, name, {}, std::move(achievements), 0, UserSettings{}, ai) {}

UserProfile::UserProfile(
    PlayerId userId,
    std::string name,
    Statistics statistics,
    std::unique_ptr<AchievementPool> achievements,
    uint64_t unlockedContent,
    UserSettings settings,
    class Computer* ai
)
    : userId(userId), ai(ai), name(std::move(name)), statistics(statistics),
      achievements(std::move(achievements)), unlockedContent(unlockedContent), settings(settings) {}

UserProfile::~UserProfile() = default;

unsigned int UserProfile::UserId() const { return userId; }

Computer* UserProfile::AI() const { return ai; }

UserProfile& UserProfile::operator=(const UserProfile& other) {
  if (this != &other) {
    userId = other.userId;
    ai = other.ai;
    name = other.name;
    statistics = other.statistics;
    achievements = other.achievements->Clone(other.userId);
    unlockedContent = other.unlockedContent;
    settings = other.settings;
  }
  return *this;
}
