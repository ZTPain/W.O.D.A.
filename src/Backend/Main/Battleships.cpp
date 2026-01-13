#include "Battleships.h"

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Replays/ReplayManager.h"
#include "Backend/Units/BattleUnitType.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include <cstdint>
#include <memory>
#include <vector>

Battleships Battleships::instance;

const GameMode Battleships::EXTENDED_GAME_MODE = GameMode{
    "Extended Mode",
    "An extended mode with additional ships and larger board. (TMP)",
    21,
    14,
    true,
    FireCommandType::FireCommand,
    {
        // TODO: Add units for extended mode
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

Battleships& Battleships::GetInstance() { return instance; }

Battleships::Battleships()
    : userManager(UserManager::GetInstance()), replayManager(ReplayManager::GetInstance()) {
  ReadSave();
}

Battleships::~Battleships() { WriteToSave(); }

std::unique_ptr<GameManager> Battleships::NewGame(
    const GameMode& mode, std::vector<UserProfile*>& profiles
) {
  return std::make_unique<GameManager>(mode, profiles);
}

void Battleships::ReadSave() {
  // Read user data
  userManager.CreateUser("Debug User");
  auto& debugUser = userManager.GetUserById(0);
  debugUser.unlockedContent = UINT64_MAX;

  userManager.CreateUser("Test User 1");
  userManager.CreateUser("Test User 2");
}

void Battleships::WriteToSave() const {
  // Write user data
}
