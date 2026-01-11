#include "GameConfigPlayerSelectView.h"

#include "Backend/Computers/Computer.h"
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
#include <utility>
#include <vector>

void GameConfigPlayerSelectView::OnEnter() { ForceRender(); }

void GameConfigPlayerSelectView::OnExit() { IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset(); }

bool GameConfigPlayerSelectView::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }

  if (keyDetails.key == ConsoleKey::Enter || keyDetails.key == ConsoleKey::Spacebar) {
    // Proceed to player configuration (not implemented)

    std::vector<UserProfile*> profiles;
    profiles.push_back(&UserManager::GetInstance().GetCurrentUser());
    profiles.push_back(
        &UserManager::GetInstance().CreateComputer("Computer 1", ComputerType::Easy)
    );

    auto gameManager = Battleships::NewGame(AppState::GetCurrentGameMode(), profiles);
    AppState::SetCurrentGameManager(std::move(gameManager));

    WindowManager::GetInstance().SwitchToWindow(WindowType::GameSetup);
    return true;
  }

  return false;
}

void GameConfigPlayerSelectView::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool GameConfigPlayerSelectView::IsCorrectSize(int /*width*/, int /*height*/) const { return true; }

void GameConfigPlayerSelectView::ForceRender() {
  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();

  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  BoxDrawing::DrawBox(1, 1, width, height, BoxStyle::Single, true, "Player Configuration");

  IO::cout << AnsiHelper::MoveCursor(3, 3) << "Game Mode: " << AppState::GetCurrentGameMode().name
           << '\n'
           << AnsiHelper::MoveCursor(3, 5)
           << "This is where player configuration would be implemented." << '\n';

  IO::cout.flush();
}
