// Player.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Backend/Users/UserProfile.h"

struct Player {
  UserProfile& profile;
  GameBoard board;
  unsigned int score = 0;
  unsigned int shotsFired = 0;
  unsigned int shotsHit = 0;
  unsigned int unitsDestroyed = 0;
};
