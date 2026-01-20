#include "SaveManager.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/FireCommand.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/Player.h"
#include "Backend/Games/SalvoFireCommand.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Replays/ReplayManager.h"
#include "Backend/Users/AchievementPool.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

SaveManager::SaveState SaveManager::CreateSaveState() {
  SaveState saveState{};
  saveState.header.version = 1;
  saveState.header.dataSize = 0; // Will be updated later

  SaveGameUserProfiles(saveState);
  SaveGameReplays(saveState);

  // Calculate total data size
  size_t totalSize = sizeof(Header) + sizeof(TableOfContent);
  totalSize += (saveState.userProfiles.size() * sizeof(UserProfileEntry)) + sizeof(int);
  totalSize += (saveState.achievements.size() * sizeof(AchievementEntry)) + sizeof(int);
  totalSize += (saveState.gameModes.size() * sizeof(GameModeEntry)) + sizeof(int);
  totalSize += (saveState.replayPlayers.size() * sizeof(ReplayPlayerEntry)) + sizeof(int);
  totalSize += (saveState.gameBoards.size() * sizeof(GameBoardEntry)) + sizeof(int);
  totalSize += (saveState.replays.size() * sizeof(ReplayEntry)) + sizeof(int);
  totalSize += (saveState.replayActions.size() * sizeof(ReplayActionEntry)) + sizeof(int);
  saveState.header.dataSize = static_cast<uint32_t>(totalSize);

  return saveState;
}

void SaveManager::SaveGameUserProfiles(SaveState& saveState) {
  const auto& users = UserManager::GetInstance().Users();
  if (!users.empty()) {
    const auto& user = users.begin()->second;
    for (const auto& [achievementName, achievement] : user.achievements->NameToAchievementMap()) {
      AchievementEntry entry{};
      assert(achievementName.size() < entry.achievementId.size());
      std::strncpy(entry.achievementId.data(), achievementName.c_str(), entry.achievementId.size());
      saveState.achievements.push_back(entry);
    }
  }

  for (const auto& [userId, user] : users) {
    UserProfileEntry entry{};
    entry.userId = user.UserId();
    assert(user.name.size() < entry.name.size());
    std::strncpy(entry.name.data(), user.name.c_str(), entry.name.size());
    entry.ai = user.AI() != nullptr ? user.AI()->GetComputerType() : ComputerType::None;
    entry.unlockedContent = user.unlockedContent;
    entry.statistics = user.statistics;
    entry.settings = user.settings;
    entry.unlockedAchievements = 0;
    for (const auto& [achievementName, achievement] : user.achievements->NameToAchievementMap()) {
      if (achievement.unlocked) {
        const auto id = std::distance(
            user.achievements->NameToAchievementMap().begin(),
            user.achievements->NameToAchievementMap().find(achievementName)
        );

        assert(id >= 0 && id < 64);
        entry.unlockedAchievements |= (1ULL << id);
      }
    }

    saveState.userProfiles.push_back(entry);
  }
}

void SaveManager::SaveGameReplays(SaveState& saveState) {
  for (const auto& replay : ReplayManager::GetInstance().Replays()) {
    ReplayEntry entry{};
    entry.replayId = replay.replayId;
    entry.winnerId = replay.winnerId;
    entry.playtimeSeconds = static_cast<uint32_t>(replay.playtime.count());
    entry.timestamp = static_cast<uint32_t>(replay.timestamp);
    const auto gameModeIndex = entry.gameModeId =
        CreateOrGetGameModeIndex(replay.mode, saveState.gameModes);

    std::vector<GameBoard*> gameBoards;
    gameBoards.reserve(replay.players.size());
    for (const auto& player : replay.players) {
      const auto index = CreateReplayPlayerIndex(player, gameBoards, saveState.replayPlayers);
      entry.replayPlayerIndices.push_back(index);
    }

    for (const auto& action : replay.history) {
      const auto index = CreateAndAddReplayAction(action, saveState.replayActions);
      entry.replayActionIndices.push_back(index);
    }

    saveState.replays.push_back(entry);

    assert(saveState.gameBoards.empty());
    saveState.gameBoards.reserve(gameBoards.size());
    for (const auto& gameBoard : gameBoards) {
      GameBoardEntry entry{};
      CreateReplayBoardEntry(*gameBoard, gameBoard->GetGameMode(), entry, gameModeIndex);
      saveState.gameBoards.push_back(entry);
    }
  }
}

uint16_t SaveManager::CreateOrGetGameModeIndex(
    const GameMode& gameMode, std::vector<GameModeEntry>& gameModes
) {
  for (size_t i = 0; i < gameModes.size(); ++i) {
    if (std::string(gameModes[i].gameModeName.data()) == gameMode.name) {
      return static_cast<uint16_t>(i);
    }
  }

  GameModeEntry entry{};
  assert(gameMode.name.size() < entry.gameModeName.size());
  std::strncpy(entry.gameModeName.data(), gameMode.name.c_str(), entry.gameModeName.size());
  gameModes.push_back(entry);
  return static_cast<uint16_t>(gameModes.size() - 1);
}

uint16_t SaveManager::CreateReplayPlayerIndex(
    const Player& player,
    std::vector<GameBoard*>& gameBoards,
    std::vector<ReplayPlayerEntry>& replayPlayers
) {
  ReplayPlayerEntry entry{};
  entry.playerId = player.profile.UserId();
  assert(player.profile.name.size() < entry.name.size());
  std::strncpy(entry.name.data(), player.profile.name.c_str(), entry.name.size());
  entry.ai =
      player.profile.AI() != nullptr ? player.profile.AI()->GetComputerType() : ComputerType::None;

  // Find the index of the player's game board
  gameBoards.push_back(player.board);
  entry.gameBoardIndex = static_cast<uint16_t>(gameBoards.size() - 1);

  replayPlayers.push_back(entry);
  return static_cast<uint16_t>(replayPlayers.size() - 1);
}

uint16_t SaveManager::CreateAndAddReplayAction(
    const ReplayAction& action, std::vector<ReplayActionEntry>& replayActions
) {
  ReplayActionEntry entry{};
  entry.playerIndex = action.playerIndex;
  entry.enemyIndex = action.enemyIndex;

  // Serialize command
  if (auto* fireCommand = dynamic_cast<FireCommand*>(action.command.get())) {
    entry.commandType = static_cast<uint8_t>(FireCommandType::FireCommand); // FireCommand type
    // Serialize FireCommand data
    size_t offset = 0;
    Coordinates coords = fireCommand->GetCoordinates().front();
    entry.commandData.resize(sizeof(Coordinates));
    WriteBytes(entry.commandData.data(), offset, entry.commandData.size(), &coords, sizeof(coords));

    replayActions.push_back(entry);
    return static_cast<uint16_t>(replayActions.size() - 1);
  }

  if (auto* salvoCommand = dynamic_cast<SalvoFireCommand*>(action.command.get())) {
    entry.commandType =
        static_cast<uint8_t>(FireCommandType::SalvoFireCommand); // SalvoFireCommand type
    // Serialize SalvoFireCommand data
    const auto coords = salvoCommand->GetCoordinates();
    size_t offset = 0;
    const size_t coordCount = coords.size();
    entry.commandData.resize(sizeof(coordCount) + (sizeof(Coordinates) * coords.size()));
    WriteBytes(
        entry.commandData.data(), offset, entry.commandData.size(), &coordCount, sizeof(coordCount)
    );
    for (const auto& coord : coords) {
      WriteBytes(
          entry.commandData.data(), offset, entry.commandData.size(), &coord, sizeof(Coordinates)
      );
    }

    replayActions.push_back(entry);
    return static_cast<uint16_t>(replayActions.size() - 1);
  }

  throw std::invalid_argument("Unsupported command type for serialization");
}
