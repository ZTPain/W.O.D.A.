#pragma once

#include "../../Input/InputManager.h"
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
        ([this](ConsoleKeyDetails keyDetails) { return OnKeyPressed(keyDetails); })
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
  virtual bool OnKeyPressed(ConsoleKeyDetails keyDetails) = 0;

private:
  WindowType type = WindowType::None;
  int onKeyPressedSubscriptionId = -1;
};
