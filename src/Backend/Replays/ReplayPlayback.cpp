#include "ReplayPlayback.h"
#include "Replay.h"
#include <cstddef>

ReplayPlayback::ReplayPlayback(const struct Replay& replay) : replay(replay) {}

const Replay& ReplayPlayback::Replay() const { return replay; }

int ReplayPlayback::CurrentStep() const { return currentStep; }

int ReplayPlayback::CurrentPlayerIndex() const {
  if (currentStep < 0 || static_cast<size_t>(currentStep) >= replay.history.size())
    return -1;

  return static_cast<int>(replay.history[currentStep].playerIndex);
}

int ReplayPlayback::CurrentEnemyIndex() const {
  if (currentStep < 0 || static_cast<size_t>(currentStep) >= replay.history.size())
    return -1;

  return static_cast<int>(replay.history[currentStep].enemyIndex);
}

bool ReplayPlayback::StepForward() {
  if (currentStep < 0 || static_cast<size_t>(currentStep) >= replay.history.size())
    return false;

  return replay.history[currentStep++].command->Execute();
}

bool ReplayPlayback::StepBackward() {
  if (currentStep < 0 || static_cast<size_t>(currentStep) >= replay.history.size())
    return false;

  replay.history[currentStep++].command->Undo();
  return true;
}
