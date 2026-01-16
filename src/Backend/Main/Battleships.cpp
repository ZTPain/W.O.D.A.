#include "Battleships.h"

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Main/SerializationHelper.h"
// #include "Backend/Replays/ReplayManager.h"
#include "Backend/Units/BattleUnitType.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <memory>
#include <vector>

Battleships& Battleships::GetInstance() {
  static Battleships instance;
  return instance;
}

const GameMode Battleships::EXTENDED_GAME_MODE = GameMode{
    "Extended Mode",
    "An extended mode with additional ships and larger board. (TMP)",
    21,
    14,
    true,
    FireCommandType::FireCommand,
    {
                   {BattleUnitType::PatrolBoat, 6},
                   {BattleUnitType::Interceptor, 5},
                   {BattleUnitType::Cruiser, 4},
                   {BattleUnitType::Dreadnought, 2},
                   }
};

const GameMode Battleships::SALVO_GAME_MODE = GameMode{
    "Salvo Mode",
    "A mode where players can fire multiple shots per turn. (TMP)",
    10,
    10,
    false,
    FireCommandType::SalvoFireCommand,
    {
                        {BattleUnitType::PatrolBoat, 4},
                        {BattleUnitType::Interceptor, 3},
                        {BattleUnitType::Cruiser, 2},
                        {BattleUnitType::Dreadnought, 1},
                        }
};

const GameMode Battleships::STANDARD_GAME_MODE = GameMode{
    "Standard Mode",
    "The classic battleships experience. (TMP)",
    10,
    10,
    false,
    FireCommandType::FireCommand,
    {
                   {BattleUnitType::PatrolBoat, 4},
                   {BattleUnitType::Interceptor, 3},
                   {BattleUnitType::Cruiser, 2},
                   {BattleUnitType::Dreadnought, 1},
                   }
};

Battleships::Battleships()
    : userManager(UserManager::GetInstance()) /*, replayManager(ReplayManager::GetInstance())*/ {
  ReadSave();
}

std::unique_ptr<GameManager> Battleships::NewGame(
    const GameMode& mode, std::vector<UserProfile*>& profiles
) {
  return std::make_unique<GameManager>(mode, profiles);
}

void Battleships::ReadSave() {
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
  size_t offset = 0;

  const auto playerCount = SerializationHelper::DeserializeInt32(buffer, offset, bufferSize);

  for (uint32_t i = 0; i < playerCount; ++i) {
    size_t bytesRead = 0;
    const auto profile = UserProfile::Deserialize(buffer, offset, bufferSize, bytesRead);

    userManager.AddUserProfile(profile);
  }
}

void Battleships::WriteToSave() const {
  std::ofstream file("save.dat", std::ios::binary | std::ios::trunc);

  if (!file)
    return;

  const auto& users = userManager.Users();

  // 1 MB should be enough
  constexpr auto BUFFER_SIZE = static_cast<const size_t>(1 * 1024 * 1024);

  static std::array<uint8_t, BUFFER_SIZE> buffer{};
  size_t offset = 0;

  SerializationHelper::SerializeInt32(buffer.data(), offset, BUFFER_SIZE, users.size());
  for (const auto& [userId, user] : users) {
    offset = user.Serialize(buffer.data(), offset, BUFFER_SIZE);
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  file.write(reinterpret_cast<const char*>(buffer.data()), static_cast<uint32_t>(offset));
  file.close();
}
