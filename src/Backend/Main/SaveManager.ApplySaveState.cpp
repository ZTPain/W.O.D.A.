#include "SaveManager.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/ICommand.h"
#include "Backend/Games/Player.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Replays/ReplayManager.h"
#include "Backend/Users/AchievementPool.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

bool SaveManager::IsVersionSupported(uint16_t version) {
  // For now, only version 1 is supported
  return version == 1;
}

void SaveManager::LoadData(const uint8_t* data, size_t offset, size_t length) {
  SaveState saveState{};

  if (length < sizeof(Header) + sizeof(TableOfContent)) {
    // Invalid save file
    return;
  }

  // Deserialize Header
  ReadBytes(data, offset, length, &saveState.header, sizeof(Header));

  if (!IsVersionSupported(saveState.header.version)) {
    // Unsupported version
    return;
  }

  // Deserialize Table of Content
  TableOfContent toc{};
  ReadBytes(data, offset, length, &toc, sizeof(TableOfContent));

  LoadAchievements(data, toc.offsetToAchievements, length, saveState.achievements);
  LoadUserProfiles(data, toc.offsetToUserProfiles, length, saveState.userProfiles);
  LoadGameModes(data, toc.offsetToGameModes, length, saveState.gameModes);
  LoadGameBoards(data, toc.offsetToGameBoards, length, saveState.gameBoards);
  LoadReplays(data, toc.offsetToReplays, length, saveState.replays);
  LoadReplayActions(data, toc.offsetToReplayActions, length, saveState.replayActions);
  LoadReplayPlayers(data, toc.offsetToReplayPlayers, length, saveState.replayPlayers);

  // Load UserProfiles
  for (const auto& user : saveState.userProfiles) {
    UserProfile userProfile(
        user.userId,
        std::string(user.name.data()),
        std::make_unique<AchievementPool>(user.userId),
        GameManager::GetComputerByType(user.ai)
    );
    userProfile.unlockedContent = user.unlockedContent;
    userProfile.statistics = user.statistics;
    userProfile.settings = user.settings;

    UserManager::GetInstance().AddUserProfile(userProfile);

    // Load unlocked achievements
    for (size_t i = 0; i < saveState.achievements.size(); ++i) {
      const auto& achievementEntry = saveState.achievements[i];
      const std::string achievementId(achievementEntry.achievementId.data());

      if ((user.unlockedAchievements & (1ULL << i)) != 0U)
        userProfile.achievements->Unlock(achievementId);
    }
  }

  std::vector<GameBoard*> registeredBoards;
  registeredBoards.reserve(saveState.gameBoards.size());
  for (size_t i = 0; i < saveState.gameBoards.size(); i++) {
    // NOLINTNEXTLINE(misc-const-correctness)
    GameBoard* const board = CreateRegisteredBoard(saveState, i);
    assert(board != nullptr);
    registeredBoards.push_back(board);
  }

  // Load Players
  std::vector<Player> players;
  players.reserve(saveState.replayPlayers.size());
  for (const auto& replayPlayerEntry : saveState.replayPlayers) {
    Player const player = CreatePlayer(registeredBoards, replayPlayerEntry);
    players.push_back(player);
  }

  // Load Actions
  std::vector<ReplayAction> actions;
  actions.reserve(saveState.replayActions.size());
  size_t i = 0;
  for (const auto& actionEntry : saveState.replayActions) {
    bool end = false;
    const size_t playersOffset = std::accumulate(
        saveState.replays.begin(),
        saveState.replays.end(),
        0U,
        [&end, &i](size_t sum, const ReplayEntry& replay) {
          if (end)
            return 0ULL;
          if (std::find(replay.replayActionIndices.begin(), replay.replayActionIndices.end(), i) !=
              replay.replayActionIndices.end()) {
            end = true;
            return sum;
          }

          return sum + replay.replayPlayerIndices.size();
        }
    );
    CreateAndAddReplayAction(actionEntry, players, playersOffset, actions);
    i++;
  }

  // Load Replays
  for (const auto& replayEntry : saveState.replays) {
    const auto& gameModeEntry = saveState.gameModes.at(replayEntry.gameModeId);
    auto gameMode = GameManager::GetGameModeByName(gameModeEntry.gameModeName.data());
    std::vector<Player> localPlayers;
    localPlayers.reserve(replayEntry.replayPlayerIndices.size());
    for (const auto& playerIndex : replayEntry.replayPlayerIndices) {
      localPlayers.push_back(players.at(playerIndex));
    }

    std::vector<ReplayAction> localActions;
    localActions.reserve(replayEntry.replayActionIndices.size());
    for (const auto& actionIndex : replayEntry.replayActionIndices) {
      localActions.push_back(actions.at(actionIndex));
    }

    ReplayManager::GetInstance().SaveReplay({
        replayEntry.replayId,
        localPlayers,
        localActions,
        replayEntry.winnerId,
        std::chrono::seconds(replayEntry.playtimeSeconds),
        time_t(replayEntry.timestamp),
        gameMode,
    });
  }
}

GameBoard* SaveManager::CreateRegisteredBoard(const SaveState& saveState, size_t gameBoardIndex) {
  if (gameBoardIndex >= saveState.gameBoards.size())
    return nullptr;

  const auto& boardEntry = saveState.gameBoards[gameBoardIndex];
  const auto& replayGameModeIndex = boardEntry.gameModeIndex;
  const auto& modeEntry = saveState.gameModes.at(replayGameModeIndex);

  const auto& gameMode = GameManager::GetGameModeByName(modeEntry.gameModeName.data());

  if (gameMode.name.empty())
    return nullptr;

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  auto* board = new GameBoard(gameMode);
  GameManager::RegisterGameBoard(board);

  std::vector<std::vector<bool>> landSegments(
      gameMode.boardHeight, std::vector<bool>(gameMode.boardWidth, false)
  );

  auto& segmentBoard = board->GetSegmentBoard();

  Coordinates firstUnitCoords{UINT64_MAX, UINT64_MAX};

  for (size_t y = 0; y < gameMode.boardHeight; y++) {
    for (size_t x = 0; x < gameMode.boardWidth; x++) {
      const size_t index = (y * gameMode.boardWidth) + x;
      assert(index < boardEntry.boardData.size() * 4);

      const uint8_t segmentValue =
          (boardEntry.boardData[index / 4] & (3 << ((index % 4) * 2))) >> ((index % 4) * 2);

      const bool landSegment = (segmentValue & 2) != 0;
      const bool hasUnit = (segmentValue & 1) != 0;

      if (landSegment)
        landSegments[y][x] = true;

      if (hasUnit) {
        segmentBoard.ToggleSegment(x, y);
        if (firstUnitCoords.x == UINT64_MAX && firstUnitCoords.y == UINT64_MAX) {
          firstUnitCoords = {x, y};
        }
      }
    }
  }

  if (gameMode.isExtended) {
    segmentBoard.LoadLandSegments(landSegments);

    // Recalculate units based on loaded segments
    segmentBoard.ToggleSegment(firstUnitCoords.x, firstUnitCoords.y);
    segmentBoard.ToggleSegment(firstUnitCoords.x, firstUnitCoords.y);
  }

  board->ParseSegments();

  return board;
}

Player SaveManager::CreatePlayer(
    const std::vector<GameBoard*>& gameBoards, const ReplayPlayerEntry& replayPlayerEntry
) {
  UserProfile* userProfilePtr = nullptr;
  if (replayPlayerEntry.ai == ComputerType::None) {
    userProfilePtr = &UserManager::GetInstance().GetUserById(replayPlayerEntry.playerId);
  } else {
    userProfilePtr = &UserManager::GetInstance().CreateComputer(
        replayPlayerEntry.name.data(), replayPlayerEntry.ai
    );
  }

  auto* gameBoard = gameBoards.at(replayPlayerEntry.gameBoardIndex);

  const Player player(*userProfilePtr, gameBoard);
  return player;
}

void SaveManager::CreateAndAddReplayAction(
    const ReplayActionEntry& actionEntry,
    const std::vector<Player>& players,
    size_t playersOffset,
    std::vector<ReplayAction>& actions
) {
  std::unique_ptr<ICommand> command = nullptr;
  switch (static_cast<FireCommandType>(actionEntry.commandType)) {
    case FireCommandType::FireCommand:
      command =
          CreateFireCommand(actionEntry, players.at(actionEntry.enemyIndex + playersOffset).board);
      break;

    case FireCommandType::SalvoFireCommand:
      command = CreateSalvoFireCommand(
          actionEntry, players.at(actionEntry.enemyIndex + playersOffset).board
      );
      break;

    default:
      throw std::logic_error("Unknown command type found during replay loading!");
  }

  ReplayAction action(actionEntry.playerIndex, actionEntry.enemyIndex, std::move(command));
  actions.push_back(std::move(action));
}

void SaveManager::CreateReplayBoardEntry(
    const GameBoard& board, const GameMode& mode, GameBoardEntry& outEntry, uint16_t gameModeIndex
) {
  const size_t boardSize = mode.boardWidth * mode.boardHeight;
  outEntry.boardData.resize((boardSize + 3) / 4, 0); // 2 bits per segment
  outEntry.gameModeIndex = gameModeIndex;

  const auto& segmentBoard = board.GetSegmentBoard();
  const auto& landSegments =
      mode.isExtended ? segmentBoard.LandSegments() : std::vector<std::vector<bool>>{};
  const auto& unitSegments = board.Units();

  for (size_t y = 0; y < mode.boardHeight; y++) {
    for (size_t x = 0; x < mode.boardWidth; x++) {
      const size_t index = (y * mode.boardWidth) + x;
      uint8_t segmentValue = 0;

      if (mode.isExtended) {
        if (landSegments[y][x])
          segmentValue |= 2;
      }

      if (unitSegments[y][x] != nullptr)
        segmentValue |= 1;

      outEntry.boardData[index / 4] |= (segmentValue << ((index % 4) * 2));
    }
  }
}
