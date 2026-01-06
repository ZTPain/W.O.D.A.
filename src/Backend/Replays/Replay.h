// Replay.h

#pragma once

#include "Backend/Games/ICommand.h"
#include "Backend/Games/Player.h"
#include <chrono>
#include <ctime>
#include <memory>
#include <vector>

struct Replay {
  unsigned int replayId;
  std::vector<Player> players;
  std::vector<std::unique_ptr<ICommand>> history;
  unsigned int winnerId;
  std::chrono::seconds playtime;
  time_t timestamp;
};
