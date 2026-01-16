// GameManager.h

#pragma once

#include "Backend/Computers/Computer.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Users/UserProfile.h"
#include "GameMode.h"
#include "ICommand.h"
#include "Player.h"
#include <chrono>
#include <memory>
#include <vector>

enum class GameState {
  Setting,
  Playing,
  Over,
};

class GameManager {
  inline static unsigned int nextGameId = 1;
  inline static Computer easyComputer = Computer(ComputerType::Easy);
  inline static Computer mediumComputer = Computer(ComputerType::Medium);
  inline static Computer hardComputer = Computer(ComputerType::Hard);
  unsigned int gameId;
  GameMode mode;
  GameState state;
  std::vector<Player> players;
  unsigned int currentTurn;
  std::vector<std::unique_ptr<ICommand>> history;
  unsigned int winnerId;
  std::chrono::time_point<std::chrono::steady_clock> gameStartPoint;
  std::chrono::seconds playtime;

  void UpdatePlayerStatistics();
  void UpdatePlayerAchievements();

public:
  GameManager(const GameMode& mode, std::vector<UserProfile*>& profiles);
  [[nodiscard]] const GameMode& Mode() const;
  [[nodiscard]] const std::vector<Player>& Players();
  [[nodiscard]] unsigned int CurrentTurn() const;
  [[nodiscard]] Player& GetCurrentPlayer();
  [[nodiscard]] Player& GetPlayerAtIndex(unsigned int playerIndex);
  [[nodiscard]] GameState State() const;

  [[nodiscard]] unsigned int WinnerId() const;
  [[nodiscard]] std::chrono::seconds Playtime() const;

  void StartGame();
  bool ExecuteCommand(std::unique_ptr<ICommand> command);
  void HandleGameOver();
  Replay GetReplay();
  static Computer* GetComputerByType(ComputerType computerType);
};
