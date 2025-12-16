// Battleships.h

#pragma once

#include "../Games/GameManager.h"
#include "../Games/GameMode.h"
#include "../Replays/ReplayManager.h"
#include "../Users/UserManager.h"
#include "../Users/UserProfile.h"
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
