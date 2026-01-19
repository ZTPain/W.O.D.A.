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
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
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

  // Allocate buffer
  std::vector<uint8_t> buffer(totalSize);

  // Serialize save state into buffer
  SaveData(buffer.data(), 0, totalSize, saveState);

  // Write buffer to file
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  file.write(reinterpret_cast<const char*>(buffer.data()), static_cast<int64_t>(buffer.size()));
  file.close();
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
    ReplayManager::GetInstance().SaveReplay({
        replayEntry.replayId,
        std::move(players),
        std::move(actions),
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
  if (offset == 0)
    return;

  if (offset >= length)
    return;

  int count = 0;
  ReadBytes(data, offset, length, &count, sizeof(count));

  while (count > 0 && offset < length) {
    // Deserialize entry
    UserProfileEntry entry{};
    ReadBytes(data, offset, length, &entry, sizeof(entry));

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
  ReadBytes(data, offset, length, &count, sizeof(count));

  while (count > 0 && offset < length) {
    // Deserialize entry
    AchievementEntry entry{};
    ReadBytes(data, offset, length, &entry, sizeof(entry));
    outAchievements.push_back(entry);
    count--;
  }

  assert(count == 0);
}

void SaveManager::LoadGameModes(
    const uint8_t* data, size_t offset, size_t length, std::vector<GameModeEntry>& outGameModes
) {
  if (offset == 0)
    return;

  if (offset >= length)
    return;

  int count = 0;
  ReadBytes(data, offset, length, &count, sizeof(count));

  while (count > 0 && offset < length) {
    // Deserialize entry
    GameModeEntry entry{};
    ReadBytes(data, offset, length, &entry, sizeof(entry));
    outGameModes.push_back(entry);
    count--;
  }

  assert(count == 0);
}

void SaveManager::LoadGameBoards(
    const uint8_t* data, size_t offset, size_t length, std::vector<GameBoardEntry>& outBoards
) {
  if (offset == 0)
    return;

  if (offset >= length)
    return;

  int count = 0;
  ReadBytes(data, offset, length, &count, sizeof(count));

  while (count > 0 && offset < length) {
    // Deserialize entry
    GameBoardEntry entry{};
    ReadBytes(data, offset, length, &entry.gameModeIndex, sizeof(entry.gameModeIndex));

    size_t boardDataSize = 0;
    ReadBytes(data, offset, length, &boardDataSize, sizeof(boardDataSize));
    entry.boardData.resize(boardDataSize);

    ReadBytes(data, offset, length, entry.boardData.data(), entry.boardData.size());
    outBoards.push_back(entry);
    count--;
  }

  assert(count == 0);
}

void SaveManager::LoadReplays(
    const uint8_t* data, size_t offset, size_t length, std::vector<ReplayEntry>& outReplays
) {
  if (offset == 0)
    return;

  if (offset >= length)
    return;

  int count = 0;
  ReadBytes(data, offset, length, &count, sizeof(count));

  while (count > 0 && offset < length) {
    // Deserialize entry
    ReplayEntry entry{};
    ReadBytes(data, offset, length, &entry.replayId, sizeof(entry.replayId));
    ReadBytes(data, offset, length, &entry.winnerId, sizeof(entry.winnerId));
    ReadBytes(data, offset, length, &entry.playtimeSeconds, sizeof(entry.playtimeSeconds));
    ReadBytes(data, offset, length, &entry.timestamp, sizeof(entry.timestamp));
    ReadBytes(data, offset, length, &entry.gameModeId, sizeof(entry.gameModeId));
    int playerCount = 0;
    ReadBytes(data, offset, length, &playerCount, sizeof(playerCount));
    for (int i = 0; i < playerCount; i++) {
      uint16_t playerIndex = 0;
      ReadBytes(data, offset, length, &playerIndex, sizeof(playerIndex));
      entry.replayPlayerIndices.push_back(playerIndex);
    }
    int actionCount = 0;
    ReadBytes(data, offset, length, &actionCount, sizeof(actionCount));
    for (int i = 0; i < actionCount; i++) {
      uint16_t actionIndex = 0;
      ReadBytes(data, offset, length, &actionIndex, sizeof(actionIndex));
      entry.replayActionIndices.push_back(actionIndex);
    }
    outReplays.push_back(entry);
    count--;
  }
  assert(count == 0);
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
  if (offset == 0)
    return;

  if (offset >= length)
    return;

  int count = 0;
  ReadBytes(data, offset, length, &count, sizeof(count));

  while (count > 0 && offset < length) {
    // Deserialize entry
    ReplayPlayerEntry entry{};
    ReadBytes(data, offset, length, &entry.playerId, sizeof(entry.playerId));
    ReadBytes(data, offset, length, &entry.name, sizeof(entry.name));
    ReadBytes(data, offset, length, &entry.ai, sizeof(entry.ai));
    ReadBytes(data, offset, length, &entry.gameBoardIndex, sizeof(entry.gameBoardIndex));
    outReplayPlayers.push_back(entry);
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
    WriteBytes(data, offset, length, &board.gameModeIndex, sizeof(board.gameModeIndex));
    const size_t boardDataSize = board.boardData.size();
    WriteBytes(data, offset, length, &boardDataSize, sizeof(boardDataSize));
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
