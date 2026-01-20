#include "UserManager.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Main/SaveManager.h"
#include "Backend/Users/UserProfile.h"
#include <cassert>
#include <map>
#include <stdexcept>
#include <string>

static UserProfile defaultUserProfile(0, "Default", nullptr);

UserManager& UserManager::GetInstance() {
  static UserManager instance;
  return instance;
}

const std::map<PlayerId, UserProfile>& UserManager::Users() const { return users; }

std::map<PlayerId, const UserProfile*> UserManager::UsersAndComputers() const {
  std::map<PlayerId, const UserProfile*> result;
  for (const auto& [userId, user] : users) {
    result[userId] = &user;
  }
  for (const auto& [compId, comp] : computers) {
    result[compId] = &comp;
  }
  return result;
}

void UserManager::CreateUser(const std::string& name) {
  currentUserId = nextUserId++;
  users[currentUserId] =
      UserProfile(currentUserId, name, initialAchievementPool.Clone(currentUserId));

  SaveManager::SaveGame();
}

UserProfile& UserManager::GetCurrentUser() {
  if (currentUserId >= 1000)
    throw std::runtime_error("Current user is a computer, how?");

  if (currentUserId == 0) {
    return defaultUserProfile;
  }

  return users[currentUserId];
}

bool UserManager::ChangeCurrentUser(PlayerId userId) {
  auto user = users.find(userId);

  if (user == users.end())
    return false;

  currentUserId = userId;
  return true;
}

UserProfile& UserManager::GetUserById(PlayerId userId) {
  if (userId >= 1000)
    return computers.at(userId);

  return users.at(userId);
}

UserProfile& UserManager::CreateComputer(const std::string& name, ComputerType computerType) {
  const auto computerUserId = nextComputerUserId++;
  computers[computerUserId] = UserProfile(
      computerUserId,
      name,
      initialAchievementPool.Clone(computerUserId),
      GameManager::GetComputerByType(computerType)
  );

  return computers.at(computerUserId);
}

UserManager::UserManager() : nextUserId(1), nextComputerUserId(1001), currentUserId(0) {}

UserManager::~UserManager() = default;

void UserManager::AddUserProfile(const UserProfile& userProfile) {
  if (userProfile.UserId() >= nextUserId) {
    nextUserId = userProfile.UserId() + 1;
  }

  users[userProfile.UserId()] = userProfile;
}

void UserManager::DestroyComputer(PlayerId computerId) {
  assert(computerId >= 1000);
  computers.erase(computerId);
}
