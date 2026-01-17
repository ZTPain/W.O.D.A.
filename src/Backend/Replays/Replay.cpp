#include "Replay.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/Player.h"
#include <chrono>
#include <ctime>
#include <memory>
#include <utility>
#include <vector>

Replay::Replay(const Replay& other) = default;

Replay& Replay::operator=(const Replay& other) {
  if (this == &other)
    return *this;

  replayId = other.replayId;
  players = other.players;
  winnerId = other.winnerId;
  playtime = other.playtime;
  timestamp = other.timestamp;
  mode = other.mode;
  history = other.history;

  return *this;
}

Replay::Replay(
    unsigned int replayId,
    std::vector<Player> players,
    std::vector<ReplayAction> history,
    unsigned int winnerId,
    std::chrono::seconds playtime,
    time_t timestamp,
    GameMode& mode
)
    : replayId(replayId), players(std::move(players)), history(std::move(history)),
      winnerId(winnerId), playtime(playtime), timestamp(timestamp), mode(mode) {}

ReplayAction& ReplayAction::operator=(const ReplayAction& other) {
  if (this == &other)
    return *this;

  playerIndex = other.playerIndex;
  enemyIndex = other.enemyIndex;
  command = other.command->Clone();
  return *this;
}
