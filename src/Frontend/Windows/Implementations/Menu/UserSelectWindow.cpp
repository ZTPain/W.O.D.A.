#include "UserSelectWindow.h"

#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/AppHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Helpers/ColorHelper.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <cstddef>
#include <string>

void UserSelectWindow::OnEnter() { ForceRender(); }

void UserSelectWindow::OnExit() {
  IO::cout << AnsiHelper::HideCursor();
  IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset();
}

bool UserSelectWindow::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    if (UserManager::GetInstance().GetCurrentUser().UserId() != 0) {
      WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
      return true;
    }

    AppHelper::Exit();
    return true;
  }

  const auto& users = UserManager::GetInstance().Users();

  switch (keyDetails.key) {
    case ConsoleKey::W:
    case ConsoleKey::UpArrow:
      if (selectedIndex > 0) {
        selectedIndex--;
        ForceRender();
      }
      return true;

    case ConsoleKey::S:
    case ConsoleKey::DownArrow:
      if (selectedIndex + 1 < users.size() + 1) {
        selectedIndex++;
        ForceRender();
      }
      return true;

    case ConsoleKey::Enter:
    case ConsoleKey::Spacebar:
      if (selectedIndex == 0) {
        HandleCreateNewUser();
        return true;
      } else {
        // Select existing user
        auto it = users.begin();
        for (size_t i = 0; i < selectedIndex - 1; ++i) {
          it++;
        }

        UserManager::GetInstance().ChangeCurrentUser(it->second.UserId());
        WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
        return true;
      }

    default:
      return false;
  }
}

void UserSelectWindow::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool UserSelectWindow::IsCorrectSize(int width, int height) const {
  const auto requiredWidth = 50;
  const auto requiredHeight = UserManager::GetInstance().Users().size() + 5;
  return static_cast<size_t>(width) >= requiredWidth &&
         static_cast<size_t>(height) >= requiredHeight;
}

void UserSelectWindow::ForceRender() {
  IO::cout << ANSI_CLEAR_SCREEN << AnsiHelper::Reset();

  BoxDrawing::DrawWindowFrame(true, "Select User");

  DrawOptions();
  if (creatingNewUserVisible) {
    DrawCreateNewUserDialog();
  }

  IO::cout.flush();
}

void UserSelectWindow::DrawOptions() const {
  IO::cout << AnsiHelper::MoveCursor(3, 2) << "Select User:\n";
  DrawOption(0, UserProfile());

  const auto& users = UserManager::GetInstance().Users();
  size_t i = 0;
  for (const auto& [userId, user] : users) {
    DrawOption(i + 1, user);
    i++;
  }
}

void UserSelectWindow::DrawOption(size_t index, const UserProfile& user) const {
  const size_t posY = 4 + index;

  if (selectedIndex == index) {
    IO::cout << AnsiHelper::SetColor(SELECTED_COLOR);
  }

  if (index == 0) {
    IO::cout << AnsiHelper::MoveCursor(5, posY) << "Create New User";
  } else {
    IO::cout << AnsiHelper::MoveCursor(5, posY) << user.name;
  }

  if (selectedIndex == index) {
    IO::cout << AnsiHelper::Reset();
  }
}

void UserSelectWindow::HandleCreateNewUser() {
  creatingNewUserVisible = true;
  newUserNameBuffer.clear();
  DrawCreateNewUserDialog();

  InputManager::DiscardPendingKeyPresses();
  while (true) {
    ConsoleKeyDetails keyDetails{};
    InputManager::GetNextKeyPress(keyDetails);

    if (keyDetails.key == ConsoleKey::Escape) {
      creatingNewUserVisible = false;
      ForceRender();
      return;
    }

    if (keyDetails.key == ConsoleKey::R && keyDetails.modifiers == ConsoleModifiers::Control) {
      ForceRender();
      continue;
    }

    if (keyDetails.key == ConsoleKey::Enter) {
      if (newUserNameBuffer.length() < MIN_USER_NAME_LENGTH)
        continue;

      break;
    }

    if (keyDetails.key == ConsoleKey::Backspace) {
      if (!newUserNameBuffer.empty()) {
        newUserNameBuffer.pop_back();
        DrawCreateNewUserDialog();
      }

      continue;
    }

    if (newUserNameBuffer.length() >= MAX_USER_NAME_LENGTH) {
      continue;
    }

    if (keyDetails.key == ConsoleKey::Spacebar ||
        (keyDetails.key >= ConsoleKey::D0 && keyDetails.key <= ConsoleKey::Z)) {
      newUserNameBuffer.push_back(static_cast<char>(keyDetails.keyCode));
      IO::cout << static_cast<char>(keyDetails.keyCode);
      IO::cout.flush();
    }
  }

  IO::cout << AnsiHelper::HideCursor();

  auto& userManager = UserManager::GetInstance();

  for (const auto& user : userManager.Users()) {
    if (user.second.name == newUserNameBuffer) {
      // User with this name already exists
      IO::cout << AnsiHelper::MoveCursor(createNewUserBoxStartX + 2, createNewUserBoxStartY + 3)
               << "User with this name already exists. Press any key to continue.";
      IO::cout.flush();
      ConsoleKeyDetails dummyKey{};
      InputManager::GetNextKeyPress(dummyKey);
      return;
    }
  }

  creatingNewUserVisible = false;

  userManager.CreateUser(newUserNameBuffer);
  ForceRender();
}

void UserSelectWindow::DrawCreateNewUserDialog() {
  IO::cout << AnsiHelper::HideCursor();
  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  const int centerX = width / 2;
  const int centerY = height / 2;

  createNewUserBoxStartX = centerX - (USER_NAME_BOX_WIDTH / 2);
  createNewUserBoxStartY = centerY - (USER_NAME_BOX_HEIGHT / 2);

  BoxDrawing::DrawBox(
      createNewUserBoxStartX,
      createNewUserBoxStartY,
      USER_NAME_BOX_WIDTH,
      USER_NAME_BOX_HEIGHT,
      BoxStyle::Rounded,
      true,
      "Creating New User"
  );

  IO::cout << AnsiHelper::MoveCursor(createNewUserBoxStartX + 2, createNewUserBoxStartY + 2)
           << "Enter new user name: " << newUserNameBuffer;
  IO::cout << AnsiHelper::ShowCursor();
  IO::cout.flush();
}
