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

std::vector<const UserProfile*> UserManager::Users() const {
  std::vector<const UserProfile*> usersOnly;

  for (const auto& user : users) {
    if (user.AI() == nullptr) {
      usersOnly.push_back(&user);
    }
  }

  return usersOnly;
}

const std::vector<UserProfile>& UserManager::UsersAndComputers() const { return users; }

void UserManager::CreateUser(const std::string& name) {
  currentUserId = nextUserId++;
  users.emplace_back(currentUserId, name, initialAchievementPool.Clone());
}

UserProfile& UserManager::GetCurrentUser() { return users[currentUserId]; }

bool UserManager::ChangeCurrentUser(unsigned int userId) {
  auto iter = std::find_if(users.begin(), users.end(), [userId](const UserProfile& uprof) {
    return uprof.UserId() == userId;
  });

  if (iter == users.end())
    return false;

  currentUserId = userId;
  return true;
}

UserProfile& UserManager::GetUserById(unsigned int userId) {
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

void UserManager::AddUserProfile(const UserProfile& userProfile) {
  if (userProfile.UserId() >= nextUserId) {
    nextUserId = userProfile.UserId() + 1;
  }

  users.push_back(userProfile);
}
