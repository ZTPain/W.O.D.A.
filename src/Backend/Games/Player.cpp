#include "Player.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Users/UserProfile.h"

Player::Player(UserProfile& profile, const GameMode& mode)
    : profile(profile), board(GameBoard(mode)) {}
