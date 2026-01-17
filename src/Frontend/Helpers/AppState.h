#pragma once

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Replays/ReplayPlayback.h"
#include <cstddef>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

class AppState {
public:
  static void Reset() {
    currentGameMode = nullptr;
    currentGameManager.reset();
    turnCounter.clear();
    currentReplayPlayback.reset();
  }

  static const GameMode& GetCurrentGameMode() { return *currentGameMode; }
  static void SetCurrentGameMode(const GameMode& mode) { currentGameMode = &mode; }
  static bool IsGameModeSet() { return currentGameMode != nullptr; }
  static void ClearCurrentGameMode() { currentGameMode = nullptr; }

  static std::unique_ptr<GameManager>& GetCurrentGameManager() {
    if (!currentGameManager.has_value())
      throw std::runtime_error("Current GameManager is not set");

    return currentGameManager.value();
  }
  static void SetCurrentGameManager(std::unique_ptr<GameManager> manager) {
    currentGameManager = std::move(manager);
  }
  static bool IsGameManagerSet() { return currentGameManager.has_value(); }
  static void ClearCurrentGameManager() { currentGameManager.reset(); }

  static const std::map<size_t, size_t>& GetTurnCounter() { return turnCounter; }
  static void IncrementTurnCounter(size_t key) { turnCounter[key]++; }
  static void ClearTurnCounter() { turnCounter.clear(); }

  static ReplayPlayback& GetCurrentReplayPlayback() {
    if (!currentReplayPlayback.has_value())
      throw std::runtime_error("Current ReplayPlayback is not set");

    return currentReplayPlayback.value();
  }

  static void SetCurrentReplayPlayback(const Replay& replay) {
    currentReplayPlayback = ReplayPlayback(replay);
  }

  static bool IsReplayPlaybackSet() { return currentReplayPlayback.has_value(); }
  static void ClearCurrentReplayPlayback() { currentReplayPlayback.reset(); }

private:
  inline static const GameMode* currentGameMode = nullptr;
  inline static std::optional<std::unique_ptr<GameManager>> currentGameManager;
  inline static std::map<size_t, size_t> turnCounter = {};
  inline static std::optional<ReplayPlayback> currentReplayPlayback;
};
