#include "GameConfigModeSelectView.h"

#include "Backend/Games/GameMode.h"
#include "Backend/Main/Battleships.h"
#include "Backend/Users/AchievementPool.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/AppState.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/TextHelper.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <cstddef>
#include <string>

static constexpr size_t OPTION_COUNT = 5;

static size_t selectedIndex = 0;

void GameConfigModeSelectView::OnEnter() { ForceRender(); }

void GameConfigModeSelectView::OnExit() { IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset(); }

static bool IsContentUnlocked(const size_t index) {
  const auto currentUser = UserManager::GetInstance().GetCurrentUser();

  switch (index) {
    case 1:
      return currentUser.unlockedContent & UnlockableContent::SalvoMode;
    case 2:
      return currentUser.unlockedContent & UnlockableContent::ExtendedMode;
    default:
      return true;
  }
}

bool GameConfigModeSelectView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    AppState::Reset();
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }

  switch (keyDetails.key) {
    case ConsoleKey::W:
    case ConsoleKey::UpArrow:
      if (selectedIndex > 0) {
        const auto prevIndex = selectedIndex;
        --selectedIndex;
        while (!IsContentUnlocked(selectedIndex) && selectedIndex > 0)
          --selectedIndex;

        DrawOption(prevIndex);
        DrawOption(selectedIndex);
      }
      return true;

    case ConsoleKey::S:
    case ConsoleKey::DownArrow:
      if (selectedIndex + 1 < OPTION_COUNT) {
        const auto prevIndex = selectedIndex;
        ++selectedIndex;
        while (!IsContentUnlocked(selectedIndex) && selectedIndex + 1 < OPTION_COUNT)
          ++selectedIndex;

        DrawOption(prevIndex);
        DrawOption(selectedIndex);
      }
      return true;

    case ConsoleKey::Spacebar:
    case ConsoleKey::Enter:
      HandleInputSelect();
      return true;

    default:
      break;
  }

  return false;
}

void GameConfigModeSelectView::HandleInputSelect() {
  const auto currentUser = UserManager::GetInstance().GetCurrentUser();

  switch (selectedIndex) {
    case 0:
      // Standard Mode
      AppState::SetCurrentGameMode(Battleships::STANDARD_GAME_MODE);
      WindowManager::GetInstance().SwitchToWindow(WindowType::GameConfigPlayersSelect);
      break;

    case 1:
      // Salvo Mode
      if (currentUser.unlockedContent & UnlockableContent::SalvoMode) {
        AppState::SetCurrentGameMode(Battleships::SALVO_GAME_MODE);
        WindowManager::GetInstance().SwitchToWindow(WindowType::GameConfigPlayersSelect);
      }
      break;

    case 2:
      // Extended Mode
      if (currentUser.unlockedContent & UnlockableContent::ExtendedMode) {
        AppState::SetCurrentGameMode(Battleships::EXTENDED_GAME_MODE);
        WindowManager::GetInstance().SwitchToWindow(WindowType::GameConfigPlayersSelect);
      }
      break;

    case 3:
      // Extended Salvo Mode
      if ((currentUser.unlockedContent & UnlockableContent::ExtendedMode) &&
          (currentUser.unlockedContent & UnlockableContent::SalvoMode)) {
        AppState::SetCurrentGameMode(Battleships::EXTENDED_SALVO_GAME_MODE);
        WindowManager::GetInstance().SwitchToWindow(WindowType::GameConfigPlayersSelect);
      }
      break;

    case 4:
      // Cancel
      WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
      break;

    default:
      break;
  }
}

void GameConfigModeSelectView::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool GameConfigModeSelectView::IsCorrectSize(int /*width*/, int /*height*/) const { return true; }

void GameConfigModeSelectView::ForceRender() {
  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  BoxDrawing::DrawWindowFrame(true, "Game Mode Selection");

  DrawOptions();

  IO::cout.flush();
}

void GameConfigModeSelectView::DrawOptions() {
  for (size_t i = 0; i < OPTION_COUNT; ++i) {
    DrawOption(i);
  }
}

void GameConfigModeSelectView::DrawOption(size_t index) {
  std::string text;
  switch (index) {
    case 0:
      text = Battleships::STANDARD_GAME_MODE.name;
      break;
    case 1:
      text = Battleships::SALVO_GAME_MODE.name;
      break;
    case 2:
      text = Battleships::EXTENDED_GAME_MODE.name;
      break;
    case 3:
      text = Battleships::EXTENDED_SALVO_GAME_MODE.name;
      break;

    case 4:
      text = "Cancel";
      break;

    default:
      break;
  }

  if (!IsContentUnlocked(index)) {
    text = "[ ⨂ ] " + text;
  } else if (selectedIndex == index) {
    text = "[ X ] " + text;
  } else {
    text = "[   ] " + text;
  }

  IO::cout << AnsiHelper::MoveCursor(5, static_cast<int>(4 + index)) << text << '\n';
  IO::cout.flush();

  if (selectedIndex == index) {
    DrawDescription(index);
  }
}

void GameConfigModeSelectView::DrawDescription(size_t index) {
  std::string description;
  switch (index) {
    case 0:
      description = Battleships::STANDARD_GAME_MODE.description;
      break;
    case 1:
      description = Battleships::SALVO_GAME_MODE.description;
      break;
    case 2:
      description = Battleships::EXTENDED_GAME_MODE.description;
      break;
    case 3:
      description = Battleships::EXTENDED_SALVO_GAME_MODE.description;
      break;
    case 4:
      description = "Return to the main menu.";
      break;

    default:
      break;
  }

  const auto startX = 40;
  const auto borderOffset = 2;

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);
  for (auto i = 4; i < height - borderOffset; i++) {
    IO::cout << AnsiHelper::MoveCursor(startX, i);
    for (auto j = 0; j < width - startX - borderOffset; j++) {
      IO::cout << ' ';
    }
  }

  TextHelper::DrawWrappedText(startX, 4, width - startX - borderOffset, description.c_str());
  IO::cout.flush();
}
