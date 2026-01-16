// Battleships.h

#pragma once

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Replays/ReplayManager.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include <memory>
#include <vector>

class Battleships {
public:
  UserManager& userManager;
  ReplayManager& replayManager;

  static const GameMode STANDARD_GAME_MODE;
  static const GameMode SALVO_GAME_MODE;
  static const GameMode EXTENDED_GAME_MODE;

private:
  void ReadSave();
  Battleships();

public:
  static Battleships& GetInstance();
  ~Battleships();
  static std::unique_ptr<GameManager> NewGame(
      const GameMode& mode, std::vector<UserProfile*>& profiles
  );
  void WriteToSave() const;
};
