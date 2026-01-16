// ReplayPlayback.h

#pragma once

#include "Replay.h"

class ReplayPlayback {
  Replay replay;
  int currentStep = -1;

public:
  ReplayPlayback(const Replay& replay);
  [[nodiscard]] const Replay& Replay() const;
  [[nodiscard]] int CurrentStep() const;
  bool StepForward();
  bool StepBackward();
};
