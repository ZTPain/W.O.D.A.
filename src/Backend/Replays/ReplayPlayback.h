// ReplayPlayback.h

#pragma once

#include "Replay.h"

class ReplayPlayback {
  Replay replay;
  int currentStep = 0;

public:
  ReplayPlayback(const Replay& replay);
  [[nodiscard]] const Replay& Replay() const;
  [[nodiscard]] int CurrentStep() const;
  [[nodiscard]] int CurrentPlayerIndex() const;
  [[nodiscard]] int CurrentEnemyIndex() const;
  bool StepForward();
  bool StepBackward();
};
