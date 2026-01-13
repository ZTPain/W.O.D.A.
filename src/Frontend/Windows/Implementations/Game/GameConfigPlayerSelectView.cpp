#include "GameConfigPlayerSelectView.h"

#include "Backend/Computers/Computer.h"
#include "Backend/Computers/ComputerHelper.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Main/Battleships.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/AppState.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <iterator>
#include <utility>
#include <vector>

static bool playerRemoveSelfWarningVisible = false;
static bool compactModeEnabled = false;

void GameConfigPlayerSelectView::OnEnter() {
  const auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  selectedPlayerOptions.clear();
  selectedPlayerOptions.emplace_back(currentUser.UserId());
  highlightedOptionIndex = 0;
  ForceRender();
}

void GameConfigPlayerSelectView::OnExit() { IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset(); }

bool GameConfigPlayerSelectView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }

  playerRemoveSelfWarningVisible = false;

  if (HandleInputMovement(keyDetails))
    return true;

  if (HandleInputSelection(keyDetails))
    return true;

  // Key is temporiary untill a "Continue" button is added
  if (keyDetails.key == ConsoleKey::H) {
    std::vector<UserProfile*> profiles{};
    profiles.reserve(selectedPlayerOptions.size());
    for (const auto& playerId : selectedPlayerOptions) {
      profiles.emplace_back(&UserManager::GetInstance().GetUserById(playerId));
    }

    auto gameManager = Battleships::NewGame(AppState::GetCurrentGameMode(), profiles);
    AppState::SetCurrentGameManager(std::move(gameManager));

    WindowManager::GetInstance().SwitchToWindow(WindowType::GameSetup);
    return true;
  }

  return false;
}

bool GameConfigPlayerSelectView::HandleInputMovement(ConsoleKeyDetails keyDetails) {
  switch (keyDetails.key) {
    case ConsoleKey::W:
    case ConsoleKey::UpArrow:
      if ((highlightedOptionIndex < 100 && highlightedOptionIndex > 0) ||
          (highlightedOptionIndex >= 100 && highlightedOptionIndex > 100))
        highlightedOptionIndex--;
      ForceRender();
      return true;

    case ConsoleKey::S:
    case ConsoleKey::DownArrow:
      if ((highlightedOptionIndex < 100 &&
           highlightedOptionIndex + 1 < UserManager::GetInstance().Users().size() +
                                            3 /* AI add options */ -
                                            selectedPlayerOptions.size()) ||
          (highlightedOptionIndex >= 100 &&
           highlightedOptionIndex + 1 < selectedPlayerOptions.size() + 100))
        highlightedOptionIndex++;
      ForceRender();
      return true;

    case ConsoleKey::A:
    case ConsoleKey::LeftArrow:
      if (highlightedOptionIndex >= 100) {
        highlightedOptionIndex -= 100;
        if (highlightedOptionIndex >=
            UserManager::GetInstance().Users().size() - selectedPlayerOptions.size()) {
          highlightedOptionIndex =
              UserManager::GetInstance().Users().size() - 1 - selectedPlayerOptions.size();
        }
      }
      ForceRender();
      return true;

    case ConsoleKey::D:
    case ConsoleKey::RightArrow:
      if (highlightedOptionIndex < 100) {
        highlightedOptionIndex += 100;
        if (highlightedOptionIndex >= selectedPlayerOptions.size() + 100) {
          highlightedOptionIndex = 100 + selectedPlayerOptions.size() - 1;
        }
      }
      ForceRender();
      return true;

    default:
      return false;
  }
}

bool GameConfigPlayerSelectView::HandleInputSelection(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key != ConsoleKey::Enter && keyDetails.key != ConsoleKey::Spacebar)
    return false;

  if (highlightedOptionIndex < 100)
    HandleInputSelectionLeft();
  else
    HandleInputSelectionRight();

  return true;
}

void GameConfigPlayerSelectView::HandleInputSelectionLeft() {
  // Select unselected player or add AI
  const auto& allProfiles = UserManager::GetInstance().Users();
  size_t selectIndex = highlightedOptionIndex;
  for (const auto& profile : allProfiles) {
    if (std::find(selectedPlayerOptions.begin(), selectedPlayerOptions.end(), profile.UserId()) !=
        selectedPlayerOptions.end()) {
      continue;
    }

    if (selectIndex == 0) {
      selectedPlayerOptions.emplace_back(profile.UserId());
      highlightedOptionIndex = std::min(highlightedOptionIndex - 1, static_cast<size_t>(0));
      ForceRender();
      return;
    }

    selectIndex--;
  }

  // Adding AI player
  ComputerType aiType = ComputerType::None;
  switch (highlightedOptionIndex - allProfiles.size() + selectedPlayerOptions.size()) {
    case 0:
      aiType = ComputerType::Easy;
      break;
    case 1:
      aiType = ComputerType::Medium;
      break;
    case 2:
      aiType = ComputerType::Hard;
      break;
    default:
      // Invalid index
      return;
  }

  const auto& aiProfile = UserManager::GetInstance().CreateComputer("Computer TMP", aiType);
  selectedPlayerOptions.emplace_back(aiProfile.UserId());
  ForceRender();
}

void GameConfigPlayerSelectView::HandleInputSelectionRight() {
  // Deselect selected player
  size_t const deselectIndex = highlightedOptionIndex - 100;
  if (deselectIndex == 0) {
    // Prevent deselecting the current user
    playerRemoveSelfWarningVisible = true;
    ForceRender();
    return;
  }
  auto it = selectedPlayerOptions.begin();
  std::advance(it, deselectIndex);
  selectedPlayerOptions.erase(it);
  highlightedOptionIndex--;
  ForceRender();
}

void GameConfigPlayerSelectView::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool GameConfigPlayerSelectView::IsCorrectSize(int /*width*/, int /*height*/) const { return true; }

void GameConfigPlayerSelectView::ForceRender() {
  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  compactModeEnabled = height < 20;

  BoxDrawing::DrawBox(1, 1, width, height, BoxStyle::Single, true, "Player Configuration");

  IO::cout << AnsiHelper::MoveCursor(3, 3) << "Game Mode: " << AppState::GetCurrentGameMode().name
           << '\n';

  IO::cout << AnsiHelper::MoveCursor(40, 3) << "Players: " << selectedPlayerOptions.size() << "\n";

  RenderPlayerOptions();

  if (playerRemoveSelfWarningVisible) {
    if (compactModeEnabled) {
      IO::cout << AnsiHelper::MoveCursor(40, 3);
    } else {
      IO::cout << AnsiHelper::MoveCursor(3, 5);
    }
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red)
             << "Warning: You cannot remove yourself from the game!" << AnsiHelper::Reset();
  }

  IO::cout.flush();
}

void GameConfigPlayerSelectView::RenderPlayerOptions() const {
  const auto& allProfiles = UserManager::GetInstance().Users();

  size_t selectedIndex = 0;
  size_t unselectedIndex = 0;
  for (const auto& profile : allProfiles) {
    bool const isSelected =
        std::find(selectedPlayerOptions.begin(), selectedPlayerOptions.end(), profile.UserId()) !=
        selectedPlayerOptions.end();

    if (isSelected) {
      RenderSelectedPlayerOption(selectedIndex++, profile);
    } else {
      RenderUnselectedPlayerOption(unselectedIndex++, profile);
    }
  }

  // Render AI add option
  RenderAIAddOption(unselectedIndex);
}

void GameConfigPlayerSelectView::RenderSelectedPlayerOption(
    size_t index, const UserProfile& playerProfile
) const {
  const size_t x = 40;
  const size_t y = (compactModeEnabled ? 4 : 7) + (index * (compactModeEnabled ? 1 : 2));

  const char* name = playerProfile.name.c_str();

  if (playerProfile.AI() != nullptr) {
    std::array<char, 100> buffer{};
    snprintf(
        buffer.data(),
        100,
        "%s (AI - %s)",
        playerProfile.name.c_str(),
        ComputerHelper::GetComputerTypeString(playerProfile.AI()->GetComputerType())
    );
    name = buffer.data();
  }

  if (highlightedOptionIndex == index + 100) {
    IO::cout << AnsiHelper::MoveCursor(x, y) << AnsiHelper::Reversed() << "[X] " << name
             << AnsiHelper::Reset();
    return;
  }

  IO::cout << AnsiHelper::MoveCursor(x, y) << "[ ] " << name;
}

void GameConfigPlayerSelectView::RenderUnselectedPlayerOption(
    size_t index, const UserProfile& playerProfile
) const {
  const size_t x = 5;
  const size_t y = (compactModeEnabled ? 4 : 7) + (index * (compactModeEnabled ? 1 : 2));

  if (highlightedOptionIndex == index) {
    IO::cout << AnsiHelper::MoveCursor(x, y) << AnsiHelper::Reversed() << "[X] "
             << playerProfile.name << AnsiHelper::Reset();
    return;
  }

  IO::cout << AnsiHelper::MoveCursor(x, y) << "[ ] " << playerProfile.name;
}

void GameConfigPlayerSelectView::RenderAIAddOption(size_t index) const {
  RenderAIAddOption(index, ComputerType::Easy);
  RenderAIAddOption(index + 1, ComputerType::Medium);
  RenderAIAddOption(index + 2, ComputerType::Hard);
}

void GameConfigPlayerSelectView::RenderAIAddOption(size_t index, ComputerType type) const {
  const size_t x = 5;
  const size_t y = (compactModeEnabled ? 4 : 7) + (index * (compactModeEnabled ? 1 : 2));

  if (highlightedOptionIndex == index) {
    IO::cout << AnsiHelper::MoveCursor(x, y) << AnsiHelper::Reversed() << "[+] Add AI Player ("
             << ComputerHelper::GetComputerTypeString(type) << ")" << AnsiHelper::Reset();
    return;
  }

  IO::cout << AnsiHelper::MoveCursor(x, y) << "[ ] Add AI Player ("
           << ComputerHelper::GetComputerTypeString(type) << ")";
}
