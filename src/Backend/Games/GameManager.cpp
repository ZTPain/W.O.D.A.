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
  history.push_back(std::move(command));
  currentTurn = (currentTurn + 1) % players.size();
  return true;
}

void GameManager::UpdatePlayerStatistics() {
  // Update the winner first ...
  players[winnerId].profile.statistics.gamesWon++;
  players[winnerId].profile.statistics.fastestWonGame =
      std::max(players[winnerId].profile.statistics.fastestWonGame, playtime);

  // ... then rest of the players
  for (unsigned int i = 0; i < players.size(); ++i) {
    auto& stats = players[i].profile.statistics;

    stats.gamesPlayed++;

    if (i != winnerId)
      stats.gamesLost++;

    stats.totalShotsFired += players[i].shotsFired;

    for (const auto& unit : players[i].board.GetAllUnits()) {
      players[i].shotsHit += unit->GetDestroyedSegments();
      players[i].score +=
          (unit->GetTotalSegments() - unit->GetDestroyedSegments()) * unit->GetTotalSegments() * 5;
      players[i].unitsDestroyed += unit->IsDestroyed() ? 1 : 0;
    }

    stats.highestScore = std::max(stats.highestScore, players[i].score);

    stats.totalShotsFired += players[i].shotsFired;
    stats.totalShotsHit += players[i].shotsHit;
    stats.totalUnitsDestroyed += players[i].unitsDestroyed;

    stats.totalPlaytime += playtime;
  }
}

void GameManager::UpdatePlayerAchievements() {
  // WIN CONDITION ACHIEVEMENTS:

  // Win a game in under 5 minutes.
  if (playtime < 5s * 60)
    players[winnerId].profile.achievements->Unlock("The Fastest Hand in the West");

  bool isPvP = true;
  for (auto& player : players) {
    if (player.profile.Computer() != nullptr) {
      isPvP = false;
      break;
    }
  }

  // Win a PvP game.
  if (isPvP)
    players[winnerId].profile.achievements->Unlock("Do You Feel Lucky?");

  // OTHER ACHIEVEMENTS:
  for (unsigned int i = 0; i < players.size(); ++i) {
    auto& achievements = players[i].profile.achievements;

    // Land 50 shots.
    if (players[i].profile.statistics.totalShotsHit >= 50)
      achievements->Unlock("Texas Sharpshooter");

    // Score over 100 points.
    if (players[i].score > 100)
      achievements->Unlock("Per Aspera ad Astra");

    // Lose without landing a shot.
    if (players[i].shotsHit == 0)
      achievements->Unlock("Pacifish");

    // Play 3 games.
    if (players[i].profile.statistics.gamesPlayed >= 3)
      achievements->Unlock("Oh Man, Look at Those Cavemen Go");

    bool hasDestroyedSegments = false;
    for (const auto& unit : players[i].board.GetAllUnits()) {
      if (unit->GetDestroyedSegments() != 0) {
        hasDestroyedSegments = true;
        break;
      }
    }

    // ONE WIN CONDITION HERE :)
    // Win without getting hit.
    if (i == winnerId && !hasDestroyedSegments)
      achievements->Unlock("Smooth Sailing");

    // Lose a ship.
    if (hasDestroyedSegments)
      achievements->Unlock("For the Voyage Is Long and the Winds Don't Blow");
  }
}

void GameManager::HandleGameOver() {
  auto gameEndPoint = std::chrono::steady_clock::now();
  playtime = std::chrono::duration_cast<std::chrono::seconds>(gameEndPoint - gameStartPoint);

  UpdatePlayerStatistics();

  // TODO: Update player achievements
  UpdatePlayerAchievements();

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
