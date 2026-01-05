// Player.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Backend/Users/UserProfile.h"

struct Player {
  UserProfile& profile;
  GameBoard board;
};
