#include "Player.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Users/UserProfile.h"

Player::Player(UserProfile& profile, GameBoard* board) : profile(profile), board(board) {}

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
