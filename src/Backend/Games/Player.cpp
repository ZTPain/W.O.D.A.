#include "Player.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Users/UserProfile.h"

Player::Player(UserProfile& profile, const GameMode& mode)
    : profile(profile), board(GameBoard(mode)) {}

Player& Player::operator=(const Player& other) {
  if (this == &other)
    return *this;

  profile = other.profile;
  board = other.board;
  shotsFired = other.shotsFired;
  shotsHit = other.shotsHit;
  score = other.score;
  unitsDestroyed = other.unitsDestroyed;

  return *this;
}
