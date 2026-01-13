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
  PlayerId currentUserId;
  AchievementPool initialAchievementPool;

  UserManager();

public:
  [[nodiscard]] static UserManager& GetInstance();
  ~UserManager();
  [[nodiscard]] const std::vector<UserProfile>& Users() const;
  void CreateUser(const std::string& name);
  [[nodiscard]] UserProfile& GetCurrentUser();
  bool ChangeCurrentUser(PlayerId userId);
  [[nodiscard]] UserProfile& GetUserById(PlayerId userId);
  [[nodiscard]] UserProfile& CreateComputer(const std::string& name, ComputerType computerType);
};
