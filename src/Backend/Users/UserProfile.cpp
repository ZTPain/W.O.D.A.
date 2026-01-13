#include "UserProfile.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Users/AchievementPool.h"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

UserProfile::UserProfile(const UserProfile& other)
    : UserProfile(
          other.userId, other.name, other.achievements->Clone(), other.ai, other.unlockedContent
      ) {}

UserProfile::UserProfile(
    PlayerId userId, std::string name, std::unique_ptr<AchievementPool> achievements
)
    : UserProfile(userId, std::move(name), std::move(achievements), nullptr, 0) {}

UserProfile::UserProfile(
    PlayerId userId,
    std::string name,
    std::unique_ptr<AchievementPool> achievements,
    class Computer* ai
)
    : UserProfile(userId, std::move(name), std::move(achievements), ai, 0) {}

UserProfile::UserProfile(
    PlayerId userId,
    std::string name,
    std::unique_ptr<AchievementPool> achievements,
    class Computer* ai,
    uint64_t unlockedContent
)
    : userId(userId), ai(ai), name(std::move(name)), achievements(std::move(achievements)),
      unlockedContent(unlockedContent) {}

UserProfile::~UserProfile() = default;

unsigned int UserProfile::UserId() const { return userId; }

Computer* UserProfile::AI() const { return ai; }
