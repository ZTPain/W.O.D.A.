#include "UserManager.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Users/UserProfile.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

UserManager UserManager::instance;
unsigned int UserManager::nextUserId;

UserManager& UserManager::GetInstance() { return instance; }

const std::vector<UserProfile>& UserManager::Users() const { return users; }

void UserManager::CreateUser(const std::string& name) {
  currentUserId = nextUserId++;
  users.emplace_back(currentUserId, name, initialAchievementPool.Clone());
}

UserProfile& UserManager::GetCurrentUser() { return users[currentUserId]; }

bool UserManager::ChangeCurrentUser(PlayerId userId) {
  auto iter = std::find_if(users.begin(), users.end(), [userId](const UserProfile& uprof) {
    return uprof.UserId() == userId;
  });

  if (iter == users.end())
    return false;

  currentUserId = userId;
  return true;
}

UserProfile& UserManager::GetUserById(PlayerId userId) {
  auto iter = std::find_if(users.begin(), users.end(), [userId](const UserProfile& uprof) {
    return uprof.UserId() == userId;
  });

  if (iter == users.end())
    throw std::out_of_range("User ID not found");

  return *iter;
}

UserProfile& UserManager::CreateComputer(const std::string& name, ComputerType computerType) {
  currentUserId = nextUserId++;
  users.emplace_back(
      currentUserId,
      name,
      initialAchievementPool.Clone(),
      GameManager::GetComputerByType(computerType)
  );

  return users[currentUserId];
}

UserManager::UserManager() : currentUserId(0) {}

UserManager::~UserManager() = default;
