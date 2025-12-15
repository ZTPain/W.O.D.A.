#pragma once

#include "../../Input/InputManager.h"
#include "Frontend/Input/ConsoleKey.h"
#include <cstdint>

enum class WindowType : uint8_t {
  None,
  LoginScreen,
  MainMenu,
  Settings,
  GameSetup,
  InGame,
  PostGameSummary,
};

class Window {
public:
  [[nodiscard]] WindowType GetType() const { return type; }
  virtual ~Window() = default;

  void Enter() {
    onKeyPressedSubscriptionId = InputManager::SubscribeToOnKeyPressed(
        ([this](ConsoleKey key, ConsoleModifiers modifiers) { OnKeyPressed(key, modifiers); })
    );
    OnEnter();
  }

  void Exit() {
    OnExit();
    InputManager::UnsubscribeFromOnKeyPressed(onKeyPressedSubscriptionId);
    onKeyPressedSubscriptionId = -1;
  }

protected:
  Window(WindowType t) : type(t) {}

  virtual void OnEnter() = 0;
  virtual void OnExit() = 0;
  virtual void OnKeyPressed(ConsoleKey key, ConsoleModifiers modifiers) = 0;

private:
  WindowType type = WindowType::None;
  int onKeyPressedSubscriptionId = -1;
};
