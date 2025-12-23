// GameManager.h

#pragma once

#include "Backend/Computers/Computer.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Users/UserProfile.h"
#include "GameMode.h"
#include "ICommand.h"
#include "Player.h"
#include <chrono>
#include <cstddef>
#include <memory>
#include <vector>

enum class GameState {
  Started,
  Setting,
  Playing,
  Over,
};

class GameManager {
  static unsigned int nextGameId;
  static Computer easyComputer;
  static Computer mediumComputer;
  static Computer hardComputer;
  unsigned int gameId;
  GameMode mode;
  GameState state = GameState::Started;
  std::vector<Player> players;
  unsigned int currentTurn;
  std::vector<Player> initialPlayerState;
  std::vector<std::unique_ptr<ICommand>> history;
  unsigned int winnerId;
  std::chrono::seconds playtime;

public:
  GameManager(GameMode mode, std::vector<UserProfile&> profiles);
  ~GameManager();
  [[nodiscard]] const GameMode Mode() const;
  const std::vector<Player> Players() const;
  [[nodiscard]] unsigned int CurrentTurn() const;
  void StartGame();
  bool ExecuteCommand(std::unique_ptr<ICommand> command);
  void HandleGameOver();
  Replay GetReplay();
};
