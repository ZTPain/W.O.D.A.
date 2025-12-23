// Battleships.h

#pragma once

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Replays/ReplayManager.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include <vector>

class Battleships {
  static Battleships instance;
  static GameMode standardGameMode;
  static GameMode salvoGameMode;
  static GameMode extendedGameMode;

public:
  UserManager userManager;
  ReplayManager replayManager;

private:
  void ReadSave();
  Battleships();

public:
  static Battleships& GetInstance();
  ~Battleships();
  GameManager NewGame(GameMode mode, std::vector<UserProfile&> profiles);
  void WriteToSave();
};
