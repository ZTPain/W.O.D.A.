// ReplayManager.h

#pragma once

#include "Replay.h"
#include "ReplayPlayback.h"
#include <vector>

class ReplayManager {
  static ReplayManager instance;
  std::vector<Replay> replays;

  ReplayManager();

public:
  static ReplayManager& GetInstance();
  ~ReplayManager();
  [[nodiscard]] const std::vector<Replay>& Replays() const;
  void SaveReplay(Replay replay);
  ReplayPlayback Play(unsigned int replayId);
};
