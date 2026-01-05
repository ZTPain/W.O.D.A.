// GameManager.cpp

#include "Backend/Games/GameManager.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/ICommand.h"
#include "Backend/Games/Player.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Users/UserProfile.h"
#include <memory>
#include <utility>
#include <vector>

GameManager::GameManager(GameMode mode, std::vector<UserProfile*>& profiles)
    : gameId(nextGameId++), mode(std::move(mode)), currentTurn(0), winnerId(0), playtime(0) {
  for (auto* profile : profiles)
    players.push_back({*profile, GameBoard(mode.boardWidth, mode.boardHeight)});
}

GameManager::~GameManager() {}

GameMode GameManager::Mode() const { return mode; }

const std::vector<Player>& GameManager::Players() { return players; }

unsigned int GameManager::CurrentTurn() const { return currentTurn; }

void GameManager::StartGame() {}

bool GameManager::ExecuteCommand(std::unique_ptr<ICommand> /*command*/) { return history.empty(); }

void GameManager::HandleGameOver() {}

Replay GameManager::GetReplay() {
  return {gameId, initialPlayerState, history, winnerId, playtime, 0};
}
