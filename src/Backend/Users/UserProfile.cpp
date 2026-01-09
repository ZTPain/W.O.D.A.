#include "UserProfile.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Users/AchievementPool.h"
#include <memory>
#include <string>
#include <utility>

UserProfile::UserProfile(
    unsigned int userId, std::string name, std::unique_ptr<AchievementPool> achievements
)
    : UserProfile(userId, std::move(name), std::move(achievements), nullptr) {}

UserProfile::UserProfile(
    unsigned int userId,
    std::string name,
    std::unique_ptr<AchievementPool> achievements,
    class Computer* computer
)
    : userId(userId), computer(computer), name(std::move(name)),
      achievements(std::move(achievements)), unlockedContent(0) {}

UserProfile::~UserProfile() = default;

unsigned int UserProfile::UserId() const { return userId; }

Computer* UserProfile::Computer() const { return computer; }
