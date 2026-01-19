#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Backend/Computers/Computer.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Games/Player.h"
#include "Backend/Replays/Replay.h"
#include "Backend/Users/UserProfile.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

class SaveManager {
public:
  static void SaveGame();
  static void LoadGame();

private:
  struct [[gnu::packed]] Header {
    uint16_t version;
    uint32_t dataSize;
  };

  struct [[gnu::packed]] TableOfContent {
    uint32_t offsetToUserProfiles;
    uint32_t offsetToAchievements;
    uint32_t offsetToReplays;
    uint32_t offsetToGameBoards;
    uint32_t offsetToGameModes;
    uint32_t offsetToReplayPlayers;
    uint32_t offsetToReplayActions;
  };

  struct [[gnu::packed]] UserProfileEntry {
    PlayerId userId{};
    std::array<char, 32> name{};
    ComputerType ai{};
    uint64_t unlockedContent{};
    [[gnu::packed]] Statistics statistics;
    [[gnu::packed]] UserSettings settings;
    uint64_t unlockedAchievements{};
  };

  struct [[gnu::packed]] AchievementEntry {
    std::array<char, 64> achievementId;
  };

  struct [[gnu::packed]] GameModeEntry {
    std::array<char, 32> gameModeName;
    uint8_t width;
    uint8_t height;
  };

  struct [[gnu::packed]] ReplayPlayerEntry {
    PlayerId playerId{};
    std::array<char, 32> name{};
    ComputerType ai{};
    uint16_t gameBoardIndex{};
  };

  struct GameBoardEntry {
    uint16_t gameModeIndex{};
    std::vector<uint8_t> boardData;
  };

  struct ReplayActionEntry {
    uint16_t playerIndex{};
    uint16_t enemyIndex{};
    uint8_t commandType{};
    std::vector<uint8_t> commandData;
  };

  struct ReplayEntry {
    uint32_t replayId{};
    uint32_t winnerId{};
    uint32_t playtimeSeconds{};
    uint32_t timestamp{};
    uint32_t gameModeId{};
    std::vector<uint16_t> replayPlayerIndices;
    std::vector<uint16_t> replayActionIndices;
  };

  struct SaveState {
    Header header;
    std::vector<UserProfileEntry> userProfiles;
    std::vector<AchievementEntry> achievements;
    std::vector<GameModeEntry> gameModes;
    std::vector<ReplayPlayerEntry> replayPlayers;
    std::vector<GameBoardEntry> gameBoards;
    std::vector<ReplayEntry> replays;
    std::vector<ReplayActionEntry> replayActions;
  };

  static void LoadData(const uint8_t* data, size_t offset, size_t length);

  static GameBoard* CreateRegisteredBoard(const SaveState& saveState, size_t gameBoardIndex);
  static Player CreatePlayer(
      const std::vector<GameBoard*>& gameBoards, const ReplayPlayerEntry& replayPlayerEntry
  );

  static void CreateAndAddReplayAction(
      const ReplayActionEntry& actionEntry,
      const std::vector<Player>& players,
      std::vector<ReplayAction>& actions
  );

  static bool IsVersionSupported(uint16_t version);
  static void LoadUserProfiles(
      const uint8_t* data, size_t offset, size_t length, std::vector<UserProfileEntry>& outProfiles
  );
  static void LoadAchievements(
      const uint8_t* data,
      size_t offset,
      size_t length,
      std::vector<AchievementEntry>& outAchievements
  );
  static void LoadReplays(
      const uint8_t* data, size_t offset, size_t length, std::vector<ReplayEntry>& outReplays
  );
  static void LoadGameBoards(
      const uint8_t* data, size_t offset, size_t length, std::vector<GameBoardEntry>& outBoards
  );
  static void LoadGameModes(
      const uint8_t* data, size_t offset, size_t length, std::vector<GameModeEntry>& outGameModes
  );
  static void LoadReplayPlayers(
      const uint8_t* data,
      size_t offset,
      size_t length,
      std::vector<ReplayPlayerEntry>& outReplayPlayers
  );
  static void LoadReplayActions(
      const uint8_t* data,
      size_t offset,
      size_t length,
      std::vector<ReplayActionEntry>& outReplayActions
  );

  static void SaveGameUserProfiles(SaveState& saveState);
  static void SaveGameReplays(SaveState& saveState);

  static void SaveData(uint8_t* data, size_t& offset, size_t length, const SaveState& saveState);

  static uint16_t CreateOrGetGameModeIndex(
      const GameMode& gameMode, std::vector<GameModeEntry>& gameModes
  );

  static uint16_t CreateReplayPlayerIndex(
      const Player& player,
      std::vector<GameBoard*>& gameBoards,
      std::vector<ReplayPlayerEntry>& replayPlayers
  );

  static uint16_t CreateAndAddReplayAction(
      const ReplayAction& action, std::vector<ReplayActionEntry>& replayActions
  );

  static void CreateReplayBoardEntry(
      const GameBoard& board, const GameMode& mode, GameBoardEntry& outEntry, uint16_t gameModeIndex
  );

  static void SaveUserProfiles(
      uint8_t* data, size_t& offset, size_t length, const std::vector<UserProfileEntry>& profiles
  );
  static void SaveAchievements(
      uint8_t* data,
      size_t& offset,
      size_t length,
      const std::vector<AchievementEntry>& achievements
  );
  static void SaveReplays(
      uint8_t* data, size_t& offset, size_t length, const std::vector<ReplayEntry>& replays
  );
  static void SaveGameBoards(
      uint8_t* data, size_t& offset, size_t length, const std::vector<GameBoardEntry>& boards
  );
  static void SaveGameModes(
      uint8_t* data, size_t& offset, size_t length, const std::vector<GameModeEntry>& gameModes
  );
  static void SaveReplayPlayers(
      uint8_t* data,
      size_t& offset,
      size_t length,
      const std::vector<ReplayPlayerEntry>& replayPlayers
  );
  static void SaveReplayActions(
      uint8_t* data,
      size_t& offset,
      size_t length,
      const std::vector<ReplayActionEntry>& replayActions
  );

  static void ReadBytes(
      const uint8_t* data, size_t& offset, size_t length, void* outBuffer, size_t outBufferSize
  );

  static void WriteBytes(
      uint8_t* data, size_t& offset, size_t length, const void* inBuffer, size_t inBufferSize
  );
};
