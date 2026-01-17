#include "Replay.h"
#include "Backend/Games/ICommand.h"
#include "Backend/Games/Player.h"
#include <chrono>
#include <ctime>
#include <memory>
#include <utility>
#include <vector>

Replay::Replay(const Replay& other)
    : replayId(other.replayId), players(other.players), winnerId(other.winnerId),
      playtime(other.playtime), timestamp(other.timestamp) {
  for (const auto& command : other.history) {
    history.emplace_back(command->Clone());
  }
}

Replay::Replay(
    unsigned int replayId,
    std::vector<Player> players,
    std::vector<std::unique_ptr<ICommand>> history,
    unsigned int winnerId,
    std::chrono::seconds playtime,
    time_t timestamp
)
    : replayId(replayId), players(std::move(players)), history(std::move(history)),
      winnerId(winnerId), playtime(playtime), timestamp(timestamp) {}
