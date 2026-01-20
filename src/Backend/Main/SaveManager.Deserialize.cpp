#include "SaveManager.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/FireCommand.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/ICommand.h"
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
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

void SaveManager::ReadBytes(
    const uint8_t* data, size_t& offset, size_t length, void* outBuffer, size_t outBufferSize
) {
  assert(offset + outBufferSize <= length);
  memcpy(outBuffer, data + offset, outBufferSize);
  offset += outBufferSize;
}

template <typename T>
void SaveManager::ReadBytes(
    const uint8_t* data, size_t& offset, size_t length, std::vector<T>& outBuffer
) {
  size_t bufferSize = 0;
  ReadBytes(data, offset, length, &bufferSize, sizeof(bufferSize));
  outBuffer.resize(bufferSize);
  for (size_t i = 0; i < bufferSize; i++) {
    ReadBytes(data, offset, length, &outBuffer[i], sizeof(T));
  }
}

template <typename T>
void SaveManager::ReadBytes(
    const uint8_t* data,
    size_t& offset,
    size_t length,
    std::vector<T>& outBuffer,
    std::function<void(T& item)> reader
) {
  size_t bufferSize = 0;
  ReadBytes(data, offset, length, &bufferSize, sizeof(bufferSize));
  outBuffer.resize(bufferSize);
  for (size_t i = 0; i < bufferSize; i++) {
    reader(outBuffer[i]);
  }
}

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
        std::make_unique<AchievementPool>(),
        GameManager::GetComputerByType(user.ai)
    );
    userProfile.unlockedContent = user.unlockedContent;
    userProfile.statistics = user.statistics;
    userProfile.settings = user.settings;

    // Load unlocked achievements
    for (size_t i = 0; i < saveState.achievements.size(); ++i) {
      const auto& achievementEntry = saveState.achievements[i];
      const std::string achievementId(achievementEntry.achievementId.data());

      if ((user.unlockedAchievements & (1ULL << i)) != 0U)
        userProfile.achievements->Unlock(achievementId);
    }

    UserManager::GetInstance().AddUserProfile(userProfile);
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
  for (const auto& actionEntry : saveState.replayActions) {
    CreateAndAddReplayAction(actionEntry, players, actions);
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
    std::vector<ReplayAction>& actions
) {
  std::unique_ptr<ICommand> command = nullptr;
  switch (static_cast<FireCommandType>(actionEntry.commandType)) {
    case FireCommandType::FireCommand: {
      size_t offset = 0;
      Coordinates coords{};
      ReadBytes(
          actionEntry.commandData.data(),
          offset,
          actionEntry.commandData.size(),
          &coords,
          sizeof(coords)
      );
      command = std::make_unique<FireCommand>(players.at(actionEntry.enemyIndex).board, coords);
      break;
    }

    case FireCommandType::SalvoFireCommand: {
      size_t offset = 0;

      size_t coordCount{};
      ReadBytes(
          actionEntry.commandData.data(),
          offset,
          actionEntry.commandData.size(),
          &coordCount,
          sizeof(coordCount)
      );

      std::vector<Coordinates> coords;
      coords.resize(coordCount);
      for (size_t i = 0; i < coordCount; i++) {
        Coordinates coord{};
        ReadBytes(
            actionEntry.commandData.data(),
            offset,
            actionEntry.commandData.size(),
            &coord,
            sizeof(coord)
        );
        coords[i] = coord;
      }

      command =
          std::make_unique<SalvoFireCommand>(players.at(actionEntry.enemyIndex).board, coords);
      break;
    }

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

void SaveManager::LoadUserProfiles(
    const uint8_t* data, size_t offset, size_t length, std::vector<UserProfileEntry>& outProfiles
) {
  ReadBytes(data, offset, length, outProfiles);
}
void SaveManager::LoadAchievements(
    const uint8_t* data,
    size_t offset,
    size_t length,
    std::vector<AchievementEntry>& outAchievements
) {
  ReadBytes(data, offset, length, outAchievements);
}

void SaveManager::LoadGameModes(
    const uint8_t* data, size_t offset, size_t length, std::vector<GameModeEntry>& outGameModes
) {
  ReadBytes(data, offset, length, outGameModes);
}

void SaveManager::LoadGameBoards(
    const uint8_t* data, size_t offset, size_t length, std::vector<GameBoardEntry>& outBoards
) {
  const std::function<void(GameBoardEntry&)> reader = [&](GameBoardEntry& entry) {
    ReadBytes(data, offset, length, &entry.gameModeIndex, sizeof(entry.gameModeIndex));
    ReadBytes(data, offset, length, entry.boardData);
  };

  ReadBytes(data, offset, length, outBoards, reader);
}

void SaveManager::LoadReplays(
    const uint8_t* data, size_t offset, size_t length, std::vector<ReplayEntry>& outReplays
) {
  const std::function<void(ReplayEntry&)> reader = [&](ReplayEntry& entry) {
    ReadBytes(data, offset, length, &entry.replayId, sizeof(entry.replayId));
    ReadBytes(data, offset, length, &entry.winnerId, sizeof(entry.winnerId));
    ReadBytes(data, offset, length, &entry.playtimeSeconds, sizeof(entry.playtimeSeconds));
    ReadBytes(data, offset, length, &entry.timestamp, sizeof(entry.timestamp));
    ReadBytes(data, offset, length, &entry.gameModeId, sizeof(entry.gameModeId));
    ReadBytes(data, offset, length, entry.replayPlayerIndices);
    ReadBytes(data, offset, length, entry.replayActionIndices);
  };

  ReadBytes(data, offset, length, outReplays, reader);
}

void SaveManager::LoadReplayActions(
    const uint8_t* data,
    size_t offset,
    size_t length,
    std::vector<ReplayActionEntry>& outReplayActions
) {
  if (offset == 0)
    return;

  if (offset >= length)
    return;

  int count = 0;
  ReadBytes(data, offset, length, &count, sizeof(count));

  while (count > 0 && offset < length) {
    // Deserialize entry
    ReplayActionEntry entry{};
    ReadBytes(data, offset, length, &entry.playerIndex, sizeof(entry.playerIndex));
    ReadBytes(data, offset, length, &entry.enemyIndex, sizeof(entry.enemyIndex));
    ReadBytes(data, offset, length, &entry.commandType, sizeof(entry.commandType));

    // Read command data size
    size_t commandDataSize = 0;
    if (entry.commandType == static_cast<uint8_t>(FireCommandType::FireCommand)) {
      commandDataSize = sizeof(Coordinates);
    } else if (entry.commandType == static_cast<uint8_t>(FireCommandType::SalvoFireCommand)) {
      size_t tempOffset = offset;
      size_t coordCount = 0;
      ReadBytes(data, tempOffset, length, &coordCount, sizeof(coordCount));
      commandDataSize = sizeof(coordCount) + (sizeof(Coordinates) * coordCount);
    } else {
      throw std::logic_error("Unknown command type found during replay action loading!");
    }

    entry.commandData.resize(commandDataSize);
    ReadBytes(data, offset, length, entry.commandData.data(), commandDataSize);

    outReplayActions.push_back(entry);
    count--;
  }

  assert(count == 0);
}

void SaveManager::LoadReplayPlayers(
    const uint8_t* data,
    size_t offset,
    size_t length,
    std::vector<ReplayPlayerEntry>& outReplayPlayers
) {
  const std::function<void(ReplayPlayerEntry&)> reader = [&](ReplayPlayerEntry& entry) {
    ReadBytes(data, offset, length, &entry.playerId, sizeof(entry.playerId));
    ReadBytes(data, offset, length, &entry.name, sizeof(entry.name));
    ReadBytes(data, offset, length, &entry.ai, sizeof(entry.ai));
    ReadBytes(data, offset, length, &entry.gameBoardIndex, sizeof(entry.gameBoardIndex));
  };

  ReadBytes(data, offset, length, outReplayPlayers, reader);
}
