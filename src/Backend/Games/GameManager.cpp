// GameManager.cpp

#include "Backend/Games/GameManager.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/ICommand.h"
#include "Backend/Games/Player.h"
#include "Backend/Main/Battleships.h"
#include "Backend/Main/SaveManager.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Replays/ReplayManager.h"
#include "Backend/Users/UserProfile.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <ctime>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

constexpr unsigned int UNIT_DESTROYED_MULTIPLIER = 4;

GameManager::GameManager(const GameMode& mode, std::vector<UserProfile*>& profiles)
    : gameId(nextGameId++), mode(mode), state(GameState::Setting), currentTurn(0), winnerId(-1),
      playtime(0) {
  for (auto* profile : profiles) {
    // Store all game boards globally to preserve their lifetime
    // They will be deleted when the program ends
    // NOLINTNEXTLINE
    auto* const board = new GameBoard(mode);
    gameBoards.push_back(board);
    players.emplace_back(*profile, board);
  }
}

const GameMode& GameManager::Mode() const { return mode; }

const std::vector<Player>& GameManager::Players() { return players; }

unsigned int GameManager::CurrentTurn() const { return currentTurn; }

Player& GameManager::GetCurrentPlayer() { return players[currentTurn]; }

Player& GameManager::GetPlayerAtIndex(unsigned int playerIndex) { return players.at(playerIndex); }

GameState GameManager::State() const { return state; }

unsigned int GameManager::WinnerId() const { return winnerId; }

std::chrono::seconds GameManager::Playtime() const { return playtime; }

void GameManager::StartGame() {
  // Start timing the game
  gameStartPoint = std::chrono::steady_clock::now();
  state = GameState::Playing;

  // Set up all the boards
  for (auto& player : players) {
    player.board->ParseSegments();
  }
}

bool GameManager::ExecuteCommand(std::unique_ptr<ICommand> command, size_t enemyIndex) {
  if (!command->Execute())
    return false;

  const unsigned int commandShotsHit = command->ShotsHit();
  const unsigned int commandUnitsDestroyed = command->UnitsDestroyed();
  const unsigned int commandShots = command->Shots();

  players[currentTurn].shotsFired += commandShots;
  players[currentTurn].shotsHit += commandShotsHit;
  players[currentTurn].unitsDestroyed += commandUnitsDestroyed;
  players[currentTurn].score +=
      commandShotsHit + (UNIT_DESTROYED_MULTIPLIER * commandUnitsDestroyed);

  // Add command to history
  history.emplace_back(currentTurn, enemyIndex, std::move(command));

  // Save turn of the player that executed the command
  const unsigned int commandPlayerTurn = currentTurn;

  // Change turn until another not dead player is found
  currentTurn = (currentTurn + 1) % players.size();
  while (players[currentTurn].board->IsGameOver())
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

    // All players who didn't win - lose
    if (i != winnerId)
      stats.gamesLost++;

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

  bool hasDestroyedSegments = false;
  for (const auto& unit : players[winnerId].board->GetAllUnits()) {
    if (unit->GetDestroyedSegments() != 0) {
      hasDestroyedSegments = true;
      break;
    }
  }

  // Win without getting hit.
  if (!hasDestroyedSegments)
    players[winnerId].profile.achievements->Unlock("Smooth Sailing");

  // OTHER ACHIEVEMENTS:
  for (auto& player : players) {
    auto& achievements = player.profile.achievements;

    // Land 50 shots.
    if (player.profile.statistics.totalShotsHit >= 50)
      achievements->Unlock("Texas Sharpshooter");

    // Score over 100 points.
    if (player.score > 100)
      achievements->Unlock("Per Aspera ad Astra");

    // Lose without landing a shot.
    if (player.shotsHit == 0)
      achievements->Unlock("Pacifish");

    // Play 3 games.
    if (player.profile.statistics.gamesPlayed >= 3)
      achievements->Unlock("Oh Man, Look at Those Cavemen Go");

    bool areAnyDestroyed = false;
    for (const auto& unit : player.board->GetAllUnits()) {
      if (unit->IsDestroyed()) {
        areAnyDestroyed = true;
        break;
      }
    }

    // Lose a ship.
    if (areAnyDestroyed)
      achievements->Unlock("For the Voyage Is Long and the Winds Don't Blow");
  }
}

void GameManager::HandleGameOver() {
  // Calculate playtime
  const auto gameEndPoint = std::chrono::steady_clock::now();
  playtime = std::chrono::duration_cast<std::chrono::seconds>(gameEndPoint - gameStartPoint);

  // Mark current (last standing) player as a winner
  winnerId = currentTurn;

  // Update player profiles
  UpdatePlayerStatistics();
  UpdatePlayerAchievements();

  state = GameState::Over;

  // Save post game end
  SaveManager::SaveGame();
}

void GameManager::SaveReplay() {
  // Reset game boards to their set (game start) state
  for (auto& p : players) {
    p.board->GetSegmentBoard().Clear();

    for (const auto& unit : p.board->GetAllUnits())
      unit->Reset();
  }

  time_t timestamp = 0;
  time(&timestamp);

  // Compile and save a replay from the game info
  ReplayManager::GetInstance().SaveReplay(
      {gameId, std::move(players), std::move(history), winnerId, playtime, timestamp, mode}
  );
}

Computer* GameManager::GetComputerByType(ComputerType computerType) {
  switch (computerType) {
    case ComputerType::None:
      return nullptr;
      break;
    case ComputerType::Easy:
      return &easyComputer;
      break;
    case ComputerType::Medium:
      return &mediumComputer;
      break;
    case ComputerType::Hard:
      return &hardComputer;
      break;
    default:
      throw std::invalid_argument("Invalid ComputerType");
      break;
  }
}

void GameManager::RegisterGameBoard(GameBoard* board) { gameBoards.push_back(board); }

GameMode GameManager::GetGameModeByName(const char* name) {
  if (name == Battleships::STANDARD_GAME_MODE.name)
    return Battleships::STANDARD_GAME_MODE;

  if (name == Battleships::SALVO_GAME_MODE.name)
    return Battleships::SALVO_GAME_MODE;

  if (name == Battleships::EXTENDED_GAME_MODE.name)
    return Battleships::EXTENDED_GAME_MODE;

  if (name == Battleships::EXTENDED_SALVO_GAME_MODE.name)
    return Battleships::EXTENDED_SALVO_GAME_MODE;

  throw std::invalid_argument("Game mode with the specified name does not exist!");
}
