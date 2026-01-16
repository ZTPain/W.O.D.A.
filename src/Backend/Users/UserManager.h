// UserManager.h

#pragma once

#include "AchievementPool.h"
#include "Backend/Computers/Computer.h"
#include "UserProfile.h"
#include <map>
#include <string>

class UserManager {
  unsigned int nextUserId;
  unsigned int nextComputerUserId;
  std::map<PlayerId, UserProfile> users;
  std::map<PlayerId, UserProfile> computers;
  PlayerId currentUserId;
  AchievementPool initialAchievementPool;

  UserManager();

public:
  [[nodiscard]] static UserManager& GetInstance();
  ~UserManager();
  [[nodiscard]] const std::map<PlayerId, UserProfile>& Users() const;
  [[nodiscard]] std::map<PlayerId, const UserProfile*> UsersAndComputers() const;
  void CreateUser(const std::string& name);
  [[nodiscard]] UserProfile& GetCurrentUser();
  bool ChangeCurrentUser(PlayerId userId);
  [[nodiscard]] UserProfile& GetUserById(PlayerId userId);
  [[nodiscard]] UserProfile& CreateComputer(const std::string& name, ComputerType computerType);

  void AddUserProfile(const UserProfile& userProfile);
  void DestroyComputer(PlayerId computerId);
};
