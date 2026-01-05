// GameManager.cpp

#include "Backend/Games/GameManager.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/ICommand.h"
#include "Backend/Games/Player.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Users/UserProfile.h"
#include <chrono>
#include <memory>
#include <vector>

GameManager::GameManager(GameMode& mode, std::vector<UserProfile*>& profiles)
    : gameId(nextGameId++), mode(mode), state(GameState::Setting), currentTurn(0), winnerId(0),
      playtime(0) {
  for (auto* profile : profiles)
    players.push_back({*profile, GameBoard(this->mode.boardWidth, this->mode.boardHeight)});
}

GameMode GameManager::Mode() const { return mode; }

const std::vector<Player>& GameManager::Players() { return players; }

unsigned int GameManager::CurrentTurn() const { return currentTurn; }

void GameManager::StartGame() {
  initialPlayerState = players;
  gameStartPoint = std::chrono::steady_clock::now();
  state = GameState::Playing;
}

bool GameManager::ExecuteCommand(std::unique_ptr<ICommand> command) {
  if (command->Execute()) {
    history.push_back(command);
    currentTurn = (currentTurn + 1) % players.size();
    return true;
  }

  return false;
}

void GameManager::HandleGameOver() {
  // TODO: Update player statistics (and achievements?)
  // for (auto& p : players) {

  // }

  auto gameEndPoint = std::chrono::steady_clock::now();
  playtime =
      std::chrono::duration_cast<std::chrono::seconds>(gameEndPoint - gameStartPoint).count();

  state = GameState::Over;
}

Replay GameManager::GetReplay() {
  return {gameId, initialPlayerState, history, winnerId, playtime, 0};
}
