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
    onKeyPressedSubscriptionId = InputManager::onKeyPressedProvider.Subscribe(
        ([this](ConsoleKeyDetails keyDetails) { return OnKeyPressed(keyDetails); })
    );

    onResizeSubscriptionId = InputManager::onTerminalResizeProvider.Subscribe(
        ([this](int width, int height) { OnResize(width, height); })
    );

    OnEnter();
  }

  void Exit() {
    OnExit();

    InputManager::onKeyPressedProvider.Unsubscribe(onKeyPressedSubscriptionId);
    onKeyPressedSubscriptionId = -1;

    InputManager::onTerminalResizeProvider.Unsubscribe(onResizeSubscriptionId);
    onResizeSubscriptionId = -1;
  }

protected:
  Window(WindowType t) : type(t) {}

  virtual void OnEnter() = 0;
  virtual void OnExit() = 0;
  virtual bool OnKeyPressed(ConsoleKeyDetails keyDetails) = 0;
  virtual void OnResize(int /*width*/, int /*height*/) {}

private:
  WindowType type = WindowType::None;
  int onKeyPressedSubscriptionId = -1;
  int onResizeSubscriptionId = -1;
};
