#pragma once

#include "Backend/Users/UserProfile.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include <cstddef>
#include <string>

class UserSelectWindow : public Window {
public:
  UserSelectWindow() : Window(WindowType::UserSelect) {};
  ~UserSelectWindow() override = default;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;

  size_t selectedIndex = 0;
  bool creatingNewUserVisible = false;
  std::string newUserNameBuffer;

  void DrawOptions() const;
  void DrawOption(size_t index, const UserProfile& user) const;
  void HandleCreateNewUser();
  void DrawCreateNewUserDialog();

  size_t createNewUserBoxStartX = 0;
  size_t createNewUserBoxStartY = 0;

  constexpr static size_t MAX_USER_NAME_LENGTH = 20;
  constexpr static size_t MIN_USER_NAME_LENGTH = 3;
  constexpr static size_t USER_NAME_BOX_WIDTH = 50;
  constexpr static size_t USER_NAME_BOX_HEIGHT = 5;
};
