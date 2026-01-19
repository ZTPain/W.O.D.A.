#include "Battleships.h"

#include "Backend/Games/GameManager.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Replays/ReplayManager.h"
#include "Backend/Units/BattleUnitType.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include "SaveManager.h"
#include <cstddef>
#include <memory>
#include <vector>

Battleships& Battleships::GetInstance() {
  static Battleships instance;
  return instance;
}

const GameMode Battleships::EXTENDED_SALVO_GAME_MODE = GameMode{
    "Extended Salvo Mode",
    "Combines the larger zoned game board from Extended mode with salvo firing rules. This is the "
    "ultimate tactical challenge, as winning in this mode requires players to carefully plan each "
    "round of attacks in order to quickly destroy an even larger and more diverse fleet of enemy "
    "units.",
    22,
    14,
    true,
    FireCommandType::SalvoFireCommand,
    {
                        {BattleUnitType::PatrolBoat, 4},
                        {BattleUnitType::Interceptor, 3},
                        {BattleUnitType::Cruiser, 2},
                        {BattleUnitType::Dreadnought, 1},

                        {BattleUnitType::InfantrySquadron, 4},
                        {BattleUnitType::GrenadeLauncher, 3},
                        {BattleUnitType::MobileArtillery, 2},
                        {BattleUnitType::ArmoredTrain, 1},
                        {BattleUnitType::OperationsHeadquarter, 1},

                        {BattleUnitType::FighterJet, 3},
                        }
};

const GameMode Battleships::EXTENDED_GAME_MODE = GameMode{
    "Extended Mode",
    "The game is played on a much larger, rectangular board (22x14), "
    "which is divided into 'sea' and 'land' zones, introducing a new strategic dimension. The "
    "fleet is enlarged and diversified, containing units dedicated only to sea, only to land, and "
    "hybrid ones (like airplanes in the shape of the letter T) that can operate in both zones.",
    22,
    14,
    true,
    FireCommandType::FireCommand,
    {
                   {BattleUnitType::PatrolBoat, 4},
                   {BattleUnitType::Interceptor, 3},
                   {BattleUnitType::Cruiser, 2},
                   {BattleUnitType::Dreadnought, 1},

                   {BattleUnitType::InfantrySquadron, 4},
                   {BattleUnitType::GrenadeLauncher, 3},
                   {BattleUnitType::MobileArtillery, 2},
                   {BattleUnitType::ArmoredTrain, 1},
                   {BattleUnitType::OperationsHeadquarter, 1},

                   {BattleUnitType::FighterJet, 3},
                   }
};

const GameMode Battleships::SALVO_GAME_MODE = GameMode{
    "Salvo Mode",
    "In this dynamic version, the number of shots per turn equals the number of ships the player "
    "still has on the board. A player with five ships therefore fires five shots simultaneously, "
    "but loses one shot with each of their own sunken ships. This requires more strategic planning "
    "of attacks and prioritizes fully destroying enemy ships.",
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
    "The classic battleships experience. This is the most well-known and commonly played variant. "
    "Players take turns making single shots by guessing coordinates on the opponent's board. The "
    "winner is the one who first sinks the entire enemy fleet, which consists of four ships of "
    "different lengths.",
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
  SaveManager::LoadGame();
}

std::unique_ptr<GameManager> Battleships::NewGame(
    const GameMode& mode, std::vector<UserProfile*>& profiles
) {
  return std::make_unique<GameManager>(mode, profiles);
}

