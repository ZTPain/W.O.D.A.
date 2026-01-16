#include "ReplayManager.h"
#include "Replay.h"
#include "ReplayPlayback.h"
#include <stdexcept>
#include <vector>

ReplayManager::ReplayManager() = default;
ReplayManager& ReplayManager::GetInstance() {
  static ReplayManager instance;
  return instance;
}

const std::vector<Replay>& ReplayManager::Replays() const { return replays; }

void ReplayManager::SaveReplay(const Replay& replay) { replays.push_back(replay); }

ReplayPlayback ReplayManager::Play(unsigned int replayId) {
  for (const auto& replay : replays) {
    if (replay.replayId == replayId) {
      return {replay};
    }
  }
  throw std::runtime_error("Replay with the given ID not found.");
}
