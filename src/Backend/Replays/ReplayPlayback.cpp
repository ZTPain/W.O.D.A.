#include "ReplayPlayback.h"
#include "Replay.h"
#include <cstddef>

ReplayPlayback::ReplayPlayback(const struct Replay& replay) : replay(replay) {}

const Replay& ReplayPlayback::Replay() const { return replay; }

int ReplayPlayback::CurrentStep() const { return currentStep; }

bool ReplayPlayback::StepForward() {
  if (currentStep < 0 || static_cast<size_t>(currentStep) >= replay.history.size())
    return false;
  return replay.history[currentStep++]->Execute();
}

bool ReplayPlayback::StepBackward() {
  if (currentStep < 0 || static_cast<size_t>(currentStep) >= replay.history.size())
    return false;
  replay.history[currentStep++]->Undo();
  return true;
}
