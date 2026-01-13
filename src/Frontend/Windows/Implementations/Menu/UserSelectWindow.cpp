#include "UserSelectWindow.h"

#include "Backend/Users/UserManager.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/AppHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <cstddef>
#include <string>

void UserSelectWindow::OnEnter() { ForceRender(); }

void UserSelectWindow::OnExit() { IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset(); }

bool UserSelectWindow::OnKeyPressed(ConsoleKeyDetails keyDetails) {

  if (keyDetails.key == ConsoleKey::Escape) {
    AppHelper::Exit();
    return true;
  }

  if (keyDetails.key >= ConsoleKey::D0 && keyDetails.key <= ConsoleKey::D9) {
    auto const selection =
        static_cast<size_t>(keyDetails.key) - static_cast<size_t>(ConsoleKey::D0);

    const auto& users = UserManager::GetInstance().Users();
    if (selection == 0) {
      // Create new user
      UserManager::GetInstance().CreateUser("Player " + std::to_string(users.size() + 1));
      ForceRender();
      return true;
    }

    if (selection - 1 < users.size()) {
      // Select existing user
      auto it = users.begin();
      for (size_t i = 0; i < selection - 1; ++i) {
        it++;
      }

      UserManager::GetInstance().ChangeCurrentUser(it->second.UserId());
      WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
      return true;
    }
  }
  return false;
}

void UserSelectWindow::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool UserSelectWindow::IsCorrectSize(int /*width*/, int /*height*/) const { return true; }

void UserSelectWindow::ForceRender() {
  IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset();

  BoxDrawing::DrawWindowFrame(true, "Select User");

  const auto users = UserManager::GetInstance().Users();
  IO::cout << AnsiHelper::MoveCursor(3, 2) << "Select User:\n";
  IO::cout << AnsiHelper::MoveCursor(5, (4 + (-1))) << "0. Create New User";

  auto i = 0;
  for (const auto& [userId, user] : users) {
    IO::cout << AnsiHelper::MoveCursor(5, (4 + i)) << (i + 1) << ". " << user.name;
    i++;
  }

  IO::cout.flush();
}
