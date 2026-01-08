// GameManager.cpp

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/ICommand.h"
#include "Backend/Games/Player.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Users/UserProfile.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

GameManager::GameManager(const GameMode& mode, std::vector<UserProfile*>& profiles)
    : gameId(nextGameId++), mode(mode), state(GameState::Setting), currentTurn(0), winnerId(0),
      playtime(0) {
  for (auto* profile : profiles)
    players.emplace_back(*profile, mode);
}

GameMode GameManager::Mode() const { return mode; }

const std::vector<Player>& GameManager::Players() { return players; }

unsigned int GameManager::CurrentTurn() const { return currentTurn; }

void GameManager::StartGame() {
  gameStartPoint = std::chrono::steady_clock::now();
  state = GameState::Playing;
}

bool GameManager::ExecuteCommand(std::unique_ptr<ICommand> command) {
  if (!command->Execute())
    return false;

  players[currentTurn].shotsFired++;
  history.push_back(command);
  currentTurn = (currentTurn + 1) % players.size();
  return true;
}

void GameManager::HandleGameOver() {
  auto gameEndPoint = std::chrono::steady_clock::now();
  playtime = std::chrono::duration_cast<std::chrono::seconds>(gameEndPoint - gameStartPoint);

  for (unsigned int i = 0; i < players.size(); ++i) {
    players[i].profile.statistics.gamesPlayed++;

    if (winnerId == i) {
      players[i].profile.statistics.gamesWon++;

      players[i].profile.statistics.fastestWonGame =
          std::max(players[i].profile.statistics.fastestWonGame, playtime);
    } else
      players[i].profile.statistics.gamesLost++;

    players[i].profile.statistics.totalShotsFired += players[i].shotsFired;

    for (const auto& unit : players[i].board.GetAllUnits()) {
      players[i].shotsHit += unit->GetDestroyedSegments();
      players[i].score += unit->GetDestroyedSegments() * unit->GetTotalSegments();
      players[i].unitsDestroyed += unit->IsDestroyed() ? 1 : 0;
    }

    players[i].profile.statistics.highestScore =
        std::max(players[i].profile.statistics.highestScore, players[i].score);

    players[i].profile.statistics.totalShotsFired += players[i].shotsFired;
    players[i].profile.statistics.totalShotsHit += players[i].shotsHit;
    players[i].profile.statistics.totalUnitsDestroyed += players[i].unitsDestroyed;

    players[i].profile.statistics.totalPlaytime += playtime;
  }

  // TODO: Update player achievements

  state = GameState::Over;
}

Replay GameManager::GetReplay() {
  for (auto& p : players) {
    p.board.GetSegmentBoard().Clear();

    for (const auto& unit : p.board.GetAllUnits())
      unit->Reset();
  }

  return {gameId, std::move(players), std::move(history), winnerId, playtime, 0};
}
