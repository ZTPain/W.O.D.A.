// Replay.h

#pragma once

#include "../Games/ICommand.h"
#include "../Games/Player.h"
#include <chrono>
#include <ctime>
#include <memory>
#include <vector>

struct Replay {
  unsigned int replayId{};
  std::vector<Player> players;
  std::vector<std::unique_ptr<ICommand>> history;
  unsigned int winnerId{};
  std::chrono::seconds playtime{};
  time_t timestamp{};
};
