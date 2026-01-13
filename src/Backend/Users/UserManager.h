// UserManager.h

#pragma once

#include "AchievementPool.h"
#include "Backend/Computers/Computer.h"
#include "UserProfile.h"
#include <string>
#include <vector>

class UserManager {
  static UserManager instance;
  static unsigned int nextUserId;
  std::vector<UserProfile> users;
  unsigned int currentUserId;
  AchievementPool initialAchievementPool;

  UserManager();

public:
  [[nodiscard]] static UserManager& GetInstance();
  ~UserManager();
  [[nodiscard]] std::vector<const UserProfile*> Users() const;
  [[nodiscard]] const std::vector<UserProfile>& UsersAndComputers() const;
  void CreateUser(const std::string& name);
  [[nodiscard]] UserProfile& GetCurrentUser();
  bool ChangeCurrentUser(unsigned int userId);
  [[nodiscard]] UserProfile& GetUserById(unsigned int userId);
  [[nodiscard]] UserProfile& CreateComputer(const std::string& name, ComputerType computerType);

  void AddUserProfile(const UserProfile& userProfile);
};
