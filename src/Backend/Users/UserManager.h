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
  static UserManager& GetInstance();
  ~UserManager();
  const std::vector<UserProfile>& Users() const;
  void CreateUser(std::string name);
  UserProfile& GetCurrentUser();
  bool ChangeCurrentUser(unsigned int userId);
  UserProfile CreateComputer(std::string name, ComputerType computerType);
};
