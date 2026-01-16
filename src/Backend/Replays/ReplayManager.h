// ReplayManager.h

#pragma once

#include "Replay.h"
#include "ReplayPlayback.h"
#include <vector>

class ReplayManager {
  std::vector<Replay> replays;

  ReplayManager();

public:
  static ReplayManager& GetInstance();
  [[nodiscard]] const std::vector<Replay>& Replays() const;
  void SaveReplay(const Replay& replay);
  ReplayPlayback Play(unsigned int replayId);
};
