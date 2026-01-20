#include "SaveManager.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/FireCommand.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/ICommand.h"
#include "Backend/Games/SalvoFireCommand.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <memory>
#include <stdexcept>
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
    const std::function<void(T& item)>& reader
) {
  size_t bufferSize = 0;
  ReadBytes(data, offset, length, &bufferSize, sizeof(bufferSize));
  outBuffer.resize(bufferSize);
  for (size_t i = 0; i < bufferSize; i++) {
    reader(outBuffer[i]);
  }
}

std::unique_ptr<ICommand> SaveManager::CreateFireCommand(
    const ReplayActionEntry& entry, GameBoard* board
) {
  size_t offset = 0;
  Coordinates coords{};
  ReadBytes(entry.commandData.data(), offset, entry.commandData.size(), &coords, sizeof(coords));
  return std::make_unique<FireCommand>(board, coords);
}

std::unique_ptr<ICommand> SaveManager::CreateSalvoFireCommand(
    const ReplayActionEntry& entry, GameBoard* board
) {
  size_t offset = 0;
  std::vector<Coordinates> coords;
  ReadBytes(entry.commandData.data(), offset, entry.commandData.size(), coords);

  return std::make_unique<SalvoFireCommand>(board, coords);
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
