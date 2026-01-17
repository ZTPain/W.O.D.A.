#pragma once

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
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

private:
  inline static const GameMode* currentGameMode = nullptr;
  inline static std::optional<std::unique_ptr<GameManager>> currentGameManager;
  inline static std::map<size_t, size_t> turnCounter = {};
};
