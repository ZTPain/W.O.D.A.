#include "SaveManager.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <vector>

void SaveManager::WriteBytes(
    uint8_t* data, size_t& offset, size_t length, const void* inBuffer, size_t inBufferSize
) {
  assert(offset + inBufferSize <= length);
  memcpy(data + offset, inBuffer, inBufferSize);
  offset += inBufferSize;
}

template <typename T>
void SaveManager::WriteBytes(
    uint8_t* data, size_t& offset, size_t length, const std::vector<T>& inBuffer
) {
  size_t bufferSize = inBuffer.size();
  WriteBytes(data, offset, length, &bufferSize, sizeof(bufferSize));
  for (const auto& item : inBuffer) {
    WriteBytes(data, offset, length, &item, sizeof(T));
  }
}

template <typename T>
void SaveManager::WriteBytes(
    uint8_t* data,
    size_t& offset,
    size_t length,
    const std::vector<T>& inBuffer,
    const std::function<void(const T& item)>& writer
) {
  size_t bufferSize = inBuffer.size();
  WriteBytes(data, offset, length, &bufferSize, sizeof(bufferSize));
  for (const auto& item : inBuffer) {
    writer(item);
  }
}

void SaveManager::SaveData(
    uint8_t* data, size_t& offset, size_t length, const SaveState& saveState
) {
  WriteBytes(data, offset, length, &saveState.header, sizeof(saveState.header));

  TableOfContent toc{};
  size_t const tocOffset = offset;
  WriteBytes(data, offset, length, &toc, sizeof(toc));

  toc.offsetToUserProfiles = static_cast<uint32_t>(offset);
  SaveUserProfiles(data, offset, length, saveState.userProfiles);

  toc.offsetToAchievements = static_cast<uint32_t>(offset);
  SaveAchievements(data, offset, length, saveState.achievements);

  toc.offsetToGameModes = static_cast<uint32_t>(offset);
  SaveGameModes(data, offset, length, saveState.gameModes);

  toc.offsetToGameBoards = static_cast<uint32_t>(offset);
  SaveGameBoards(data, offset, length, saveState.gameBoards);

  toc.offsetToReplays = static_cast<uint32_t>(offset);
  SaveReplays(data, offset, length, saveState.replays);

  toc.offsetToReplayActions = static_cast<uint32_t>(offset);
  SaveReplayActions(data, offset, length, saveState.replayActions);

  toc.offsetToReplayPlayers = static_cast<uint32_t>(offset);
  SaveReplayPlayers(data, offset, length, saveState.replayPlayers);

  offset = tocOffset;
  WriteBytes(data, offset, length, &toc, sizeof(toc));
}

void SaveManager::SaveUserProfiles(
    uint8_t* data, size_t& offset, size_t length, const std::vector<UserProfileEntry>& profiles
) {
  WriteBytes(data, offset, length, profiles);
}

void SaveManager::SaveAchievements(
    uint8_t* data, size_t& offset, size_t length, const std::vector<AchievementEntry>& achievements
) {
  WriteBytes(data, offset, length, achievements);
}

void SaveManager::SaveGameModes(
    uint8_t* data, size_t& offset, size_t length, const std::vector<GameModeEntry>& gameModes
) {
  WriteBytes(data, offset, length, gameModes);
}

void SaveManager::SaveGameBoards(
    uint8_t* data, size_t& offset, size_t length, const std::vector<GameBoardEntry>& boards
) {
  const std::function<void(const GameBoardEntry&)> writer = [&](const GameBoardEntry& board) {
    WriteBytes(data, offset, length, &board.gameModeIndex, sizeof(board.gameModeIndex));
    WriteBytes(data, offset, length, board.boardData);
  };

  WriteBytes(data, offset, length, boards, writer);
}

void SaveManager::SaveReplays(
    uint8_t* data, size_t& offset, size_t length, const std::vector<ReplayEntry>& replays
) {
  const std::function<void(const ReplayEntry&)> writer = [&](const ReplayEntry& replay) {
    WriteBytes(data, offset, length, &replay.replayId, sizeof(replay.replayId));
    WriteBytes(data, offset, length, &replay.winnerId, sizeof(replay.winnerId));
    WriteBytes(data, offset, length, &replay.playtimeSeconds, sizeof(replay.playtimeSeconds));
    WriteBytes(data, offset, length, &replay.timestamp, sizeof(replay.timestamp));
    WriteBytes(data, offset, length, &replay.gameModeId, sizeof(replay.gameModeId));

    WriteBytes(data, offset, length, replay.replayPlayerIndices);
    WriteBytes(data, offset, length, replay.replayActionIndices);
  };

  WriteBytes(data, offset, length, replays, writer);
}

void SaveManager::SaveReplayActions(
    uint8_t* data, size_t& offset, size_t length, const std::vector<ReplayActionEntry>& actions
) {
  const std::function<void(const ReplayActionEntry&)> writer =
      [&](const ReplayActionEntry& action) {
        WriteBytes(data, offset, length, &action.playerIndex, sizeof(action.playerIndex));
        WriteBytes(data, offset, length, &action.enemyIndex, sizeof(action.enemyIndex));
        WriteBytes(data, offset, length, &action.commandType, sizeof(action.commandType));
        WriteBytes(data, offset, length, action.commandData.data(), action.commandData.size());
      };

  WriteBytes(data, offset, length, actions, writer);
}

void SaveManager::SaveReplayPlayers(
    uint8_t* data, size_t& offset, size_t length, const std::vector<ReplayPlayerEntry>& players
) {
  WriteBytes(data, offset, length, players);
}
