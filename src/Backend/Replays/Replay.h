// Replay.h

#pragma once

#include "Backend/Games/GameMode.h"
#include "Backend/Games/ICommand.h"
#include "Backend/Games/Player.h"
#include <chrono>
#include <ctime>
#include <memory>
#include <utility>
#include <vector>

struct ReplayAction {
  size_t playerIndex;
  size_t enemyIndex;
  std::unique_ptr<ICommand> command;

  ReplayAction(size_t playerIndex, size_t enemyIndex, std::unique_ptr<ICommand> command)
      : playerIndex(playerIndex), enemyIndex(enemyIndex), command(std::move(command)) {}

  ReplayAction(const ReplayAction& other)
      : playerIndex(other.playerIndex), enemyIndex(other.enemyIndex),
        command(other.command->Clone()) {}

  ReplayAction& operator=(const ReplayAction& other);
};

struct Replay {
  unsigned int replayId{};
  std::vector<Player> players;
  std::vector<ReplayAction> history;
  unsigned int winnerId{};
  std::chrono::seconds playtime{};
  time_t timestamp{};
  GameMode mode;

  Replay(const Replay& other);
  Replay& operator=(const Replay& other);

  Replay(
      unsigned int replayId,
      std::vector<Player> players,
      std::vector<ReplayAction> history,
      unsigned int winnerId,
      std::chrono::seconds playtime,
      time_t timestamp,
      GameMode& mode
  );
};
