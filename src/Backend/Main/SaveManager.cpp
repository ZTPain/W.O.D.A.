#include "SaveManager.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/GameManager.h"
#include "Backend/Users/AchievementPool.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

void SaveManager::ReadBytes(
    const uint8_t* data, size_t& offset, size_t length, void* outBuffer, size_t outBufferSize
) {
  assert(offset + outBufferSize <= length);
  memcpy(outBuffer, data + offset, outBufferSize);
  offset += outBufferSize;
}

void SaveManager::WriteBytes(
    uint8_t* data, size_t& offset, size_t length, const void* inBuffer, size_t inBufferSize
) {
  assert(offset + inBufferSize <= length);
  memcpy(data + offset, inBuffer, inBufferSize);
  offset += inBufferSize;
}

void SaveManager::SaveGame() {
  std::ofstream file("save.dat", std::ios::binary | std::ios::trunc);

  if (!file)
    return;

  // Prepare save state
  SaveState saveState{};
  saveState.header.version = 1;
  saveState.header.dataSize = 0; // Will be updated later

  // Populate user profiles
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

  // Calculate total data size
  size_t totalSize = sizeof(Header) + sizeof(TableOfContent);
  totalSize += (saveState.userProfiles.size() * sizeof(UserProfileEntry)) + sizeof(int);
  totalSize += (saveState.achievements.size() * sizeof(AchievementEntry)) + sizeof(int);
  saveState.header.dataSize = static_cast<uint32_t>(totalSize);

  // Allocate buffer
  std::vector<uint8_t> buffer(totalSize);

  // Serialize save state into buffer
  SaveData(buffer.data(), 0, totalSize, saveState);

  // Write buffer to file
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  file.write(reinterpret_cast<const char*>(buffer.data()), static_cast<int64_t>(buffer.size()));
  file.close();
}

void SaveManager::LoadGame() {
  if (!std::filesystem::exists("save.dat"))
    return;

  std::ifstream file("save.dat", std::ios::binary);

  if (!file)
    return;

  std::vector<char> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  file.close();

  const auto bufferSize = bytes.size();
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  const auto* buffer = reinterpret_cast<const uint8_t*>(bytes.data());
  size_t const offset = 0;

  LoadData(buffer, offset, bufferSize);
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
  std::memcpy(&saveState.header, data + offset, sizeof(Header));
  offset += sizeof(Header);

  if (!IsVersionSupported(saveState.header.version)) {
    // Unsupported version
    return;
  }

  TableOfContent toc{};

  // Deserialize Table of Content
  std::memcpy(&toc, data + offset, sizeof(TableOfContent));
  offset += sizeof(TableOfContent);

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
}

void SaveManager::LoadUserProfiles(
    const uint8_t* data, size_t offset, size_t length, std::vector<UserProfileEntry>& outProfiles
) {
  if (offset == 0)
    return;

  if (offset >= length)
    return;

  int count = 0;
  std::memcpy(&count, data + offset, sizeof(int));
  offset += sizeof(int);

  while (count > 0 && offset < length) {
    UserProfileEntry entry{};

    if (offset + sizeof(UserProfileEntry) > length) {
      // Invalid entry size
      break;
    }

    // Deserialize entry
    std::memcpy(&entry, data + offset, sizeof(UserProfileEntry));
    offset += sizeof(UserProfileEntry);

    outProfiles.push_back(entry);
    count--;
  }

  assert(count == 0);
}

void SaveManager::LoadAchievements(
    const uint8_t* data,
    size_t offset,
    size_t length,
    std::vector<AchievementEntry>& outAchievements
) {
  if (offset == 0)
    return;

  if (offset >= length)
    return;

  int count = 0;
  std::memcpy(&count, data + offset, sizeof(int));
  offset += sizeof(int);

  while (count > 0 && offset < length) {
    AchievementEntry entry{};

    if (offset + sizeof(AchievementEntry) > length) {
      // Invalid entry size
      break;
    }

    // Deserialize entry
    std::memcpy(&entry, data + offset, sizeof(AchievementEntry));
    offset += sizeof(AchievementEntry);

    outAchievements.push_back(entry);
    count--;
  }

  assert(count == 0);
}

void SaveManager::SaveData(
    uint8_t* data, size_t offset, size_t length, const SaveState& saveState
) {
  assert(length >= sizeof(Header) + sizeof(TableOfContent));

  memcpy(data, &saveState.header, sizeof(Header));
  offset += sizeof(Header);

  TableOfContent toc{};
  size_t const tocOffset = offset;
  offset += sizeof(TableOfContent);

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

  memcpy(data + tocOffset, &toc, sizeof(TableOfContent));
}

void SaveManager::SaveUserProfiles(
    uint8_t* data, size_t& offset, size_t length, const std::vector<UserProfileEntry>& profiles
) {
  const int size = static_cast<int>(profiles.size());
  WriteBytes(data, offset, length, &size, sizeof(int));

  for (const auto& profile : profiles) {
    WriteBytes(data, offset, length, &profile, sizeof(UserProfileEntry));
  }
}

void SaveManager::SaveAchievements(
    uint8_t* data, size_t& offset, size_t length, const std::vector<AchievementEntry>& achievements
) {
  const int size = static_cast<int>(achievements.size());
  WriteBytes(data, offset, length, &size, sizeof(int));

  for (const auto& achievement : achievements) {
    WriteBytes(data, offset, length, &achievement, sizeof(AchievementEntry));
  }
}

void SaveManager::SaveGameModes(
    uint8_t* data, size_t& offset, size_t length, const std::vector<GameModeEntry>& gameModes
) {
  const int size = static_cast<int>(gameModes.size());
  WriteBytes(data, offset, length, &size, sizeof(int));

  for (const auto& gameMode : gameModes) {
    WriteBytes(data, offset, length, &gameMode, sizeof(GameModeEntry));
  }
}

void SaveManager::SaveGameBoards(
    uint8_t* data, size_t& offset, size_t length, const std::vector<GameBoardEntry>& boards
) {
  const int size = static_cast<int>(boards.size());
  WriteBytes(data, offset, length, &size, sizeof(int));

  for (const auto& board : boards) {
    WriteBytes(data, offset, length, &board.playerIndex, sizeof(board.playerIndex));
    WriteBytes(data, offset, length, board.boardData.data(), board.boardData.size());
  }
}

void SaveManager::SaveReplays(
    uint8_t* data, size_t& offset, size_t length, const std::vector<ReplayEntry>& replays
) {
  const int size = static_cast<int>(replays.size());
  WriteBytes(data, offset, length, &size, sizeof(int));

  for (const auto& replay : replays) {
    WriteBytes(data, offset, length, &replay.replayId, sizeof(replay.replayId));
    WriteBytes(data, offset, length, &replay.winnerId, sizeof(replay.winnerId));
    WriteBytes(data, offset, length, &replay.playtimeSeconds, sizeof(replay.playtimeSeconds));
    WriteBytes(data, offset, length, &replay.timestamp, sizeof(replay.timestamp));
    WriteBytes(data, offset, length, &replay.gameModeId, sizeof(replay.gameModeId));

    const int playerCount = static_cast<int>(replay.replayPlayerIndices.size());
    WriteBytes(data, offset, length, &playerCount, sizeof(int));
    for (const auto& playerIndex : replay.replayPlayerIndices) {
      WriteBytes(data, offset, length, &playerIndex, sizeof(playerIndex));
    }

    const int actionCount = static_cast<int>(replay.replayActionIndices.size());
    WriteBytes(data, offset, length, &actionCount, sizeof(int));
    for (const auto& actionIndex : replay.replayActionIndices) {
      WriteBytes(data, offset, length, &actionIndex, sizeof(actionIndex));
    }
  }
}

void SaveManager::SaveReplayActions(
    uint8_t* data, size_t& offset, size_t length, const std::vector<ReplayActionEntry>& actions
) {
  const int size = static_cast<int>(actions.size());
  WriteBytes(data, offset, length, &size, sizeof(int));

  for (const auto& action : actions) {
    WriteBytes(data, offset, length, &action.playerIndex, sizeof(action.playerIndex));
    WriteBytes(data, offset, length, &action.enemyIndex, sizeof(action.enemyIndex));
    WriteBytes(data, offset, length, &action.commandType, sizeof(action.commandType));
    WriteBytes(data, offset, length, action.commandData.data(), action.commandData.size());
  }
}

void SaveManager::SaveReplayPlayers(
    uint8_t* data, size_t& offset, size_t length, const std::vector<ReplayPlayerEntry>& players
) {
  const int size = static_cast<int>(players.size());
  WriteBytes(data, offset, length, &size, sizeof(int));

  for (const auto& player : players) {
    WriteBytes(data, offset, length, &player, sizeof(player));
  }
}
