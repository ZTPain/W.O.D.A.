// GameManager.cpp

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/ICommand.h"
#include "Backend/Games/Player.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Users/UserProfile.h"
#include <algorithm>
#include <chrono>
#include <memory>
#include <utility>
#include <vector>

GameManager::GameManager(const GameMode& mode, std::vector<UserProfile*>& profiles)
    : gameId(nextGameId++), mode(mode), state(GameState::Setting), currentTurn(0), winnerId(0),
      playtime(0) {
  for (auto* profile : profiles)
    players.emplace_back(*profile, mode);
}

const GameMode& GameManager::Mode() const { return mode; }

const std::vector<Player>& GameManager::Players() const { return players; }

unsigned int GameManager::CurrentTurn() const { return currentTurn; }

void GameManager::StartGame() {
  gameStartPoint = std::chrono::steady_clock::now();
  state = GameState::Playing;
}

bool GameManager::ExecuteCommand(std::unique_ptr<ICommand> command) {
  if (!command->Execute())
    return false;

  players[currentTurn].shotsFired++;
  // Add command to history
  history.push_back(std::move(command));

  // Save turn of the player that executed the command
  const unsigned int commandPlayerTurn = currentTurn;

  // Change turn until another not dead player is found
  currentTurn = (currentTurn + 1) % players.size();
  while (players[currentTurn].board.IsGameOver())
    currentTurn = (currentTurn + 1) % players.size();

  // If the turn rolled over to the same player, mark game as over
  if (commandPlayerTurn == currentTurn)
    HandleGameOver();

  return true;
}

void GameManager::UpdatePlayerStatistics() {
  // Update the winner first ...
  players[winnerId].profile.statistics.gamesWon++;
  players[winnerId].profile.statistics.fastestWonGame =
      std::min(players[winnerId].profile.statistics.fastestWonGame, playtime);

  // ... then rest of the players
  for (unsigned int i = 0; i < players.size(); ++i) {
    auto& stats = players[i].profile.statistics;

    stats.gamesPlayed++;

    // For all non-winners
    if (i != winnerId)
      stats.gamesLost++;

    for (const auto& unit : players[i].board.GetAllUnits()) {
      // Sum up game performance
      players[i].shotsHit += unit->GetDestroyedSegments();
      players[i].score +=
          (unit->GetTotalSegments() - unit->GetDestroyedSegments()) * unit->GetTotalSegments() * 5;
      players[i].unitsDestroyed += unit->IsDestroyed() ? 1 : 0;
    }

    // New highscore
    stats.highestScore = std::max(stats.highestScore, players[i].score);

    // Update totals
    stats.totalShotsFired += players[i].shotsFired;
    stats.totalShotsHit += players[i].shotsHit;
    stats.totalUnitsDestroyed += players[i].unitsDestroyed;

    stats.totalPlaytime += playtime;
  }
}

void GameManager::UpdatePlayerAchievements() {
  // WIN CONDITION ACHIEVEMENTS:

  // Win a game in under 5 minutes.
  static constexpr auto FIVE_MINUTES = std::chrono::seconds(5) * 60;

  if (playtime < FIVE_MINUTES)
    players[winnerId].profile.achievements->Unlock("The Fastest Hand in the West");

  bool isPvP = true;
  for (const auto& player : players) {
    if (player.profile.AI() != nullptr) {
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
  // Calculate playtime
  const auto gameEndPoint = std::chrono::steady_clock::now();
  playtime = std::chrono::duration_cast<std::chrono::seconds>(gameEndPoint - gameStartPoint);

  // Update player profiles
  UpdatePlayerStatistics();
  UpdatePlayerAchievements();

  state = GameState::Over;
}

Replay GameManager::GetReplay() {
  // Reset game boards to their set (game start) state
  for (auto& p : players) {
    p.board.GetSegmentBoard().Clear();

    for (const auto& unit : p.board.GetAllUnits())
      unit->Reset();
  }

  // Compile a replay from the game info
  return {gameId, std::move(players), std::move(history), winnerId, playtime, 0};
}
