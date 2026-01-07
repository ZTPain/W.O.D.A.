#include "ReplayManager.h"
#include "Replay.h"
#include "ReplayPlayback.h"
#include <vector>

ReplayManager::ReplayManager() = default;
ReplayManager& ReplayManager::GetInstance() { return instance; }
ReplayManager::~ReplayManager() = default;
const std::vector<Replay>& ReplayManager::Replays() const { return replays; }

void ReplayManager::SaveReplay(const Replay& replay) { replays.push_back(replay); }

ReplayPlayback ReplayManager::Play(unsigned int replayId) {
  return {replays[replayId]};
}
