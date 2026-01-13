#include "Battleships.h"

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Replays/ReplayManager.h"
#include "Backend/Units/BattleUnitType.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"

#include <cstddef>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

Battleships Battleships::instance;

Battleships& Battleships::GetInstance() { return instance; }

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
  std::ifstream file("save.dat");

  if (!file) {
    userManager.CreateUser("Player1");
    userManager.CreateUser("Player2");
    return;
  }

  std::string tag;
  size_t userCount = 0;
  file >> tag >> userCount;

  if (tag != "USERS") {
    userManager.CreateUser("Player1");
    userManager.CreateUser("Player2");
    return;
  }

  for (size_t i = 0; i < userCount; ++i) {
    unsigned int id = 0;
    std::string name;
    file >> id >> name;
    userManager.CreateUser(name);
  }

  file >> tag;
  if (tag == "CURRENT") {
    unsigned int currentId = 0;
    file >> currentId;
    userManager.ChangeCurrentUser(currentId);
  }
}

void Battleships::WriteToSave() const {
  std::ofstream file("save.dat");
  if (!file)
    return;

  const auto& users = userManager.Users();

  file << "USERS " << users.size() << "\n";

  for (const auto& user : users) {
    file << user.UserId() << " " << user.name << "\n";
  }

  if (!users.empty()) {
    file << "CURRENT " << userManager.GetCurrentUser().UserId() << "\n";
  }
}
