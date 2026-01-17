#pragma once

#include "Frontend/Input/InputManager.h"
#include <cstdint>

enum class WindowType : uint8_t {
  None,

  UserSelect,
  MainMenu,
  Settings,
  MatchHistory,
  UserProfile,

  GameConfigModeSelect,
  GameConfigPlayersSelect,

  GameSetup,

  InGame,
  GameReplay,

  PostGameSummary,
};

class Window {
public:
  [[nodiscard]] WindowType GetType() const { return type; }
  virtual ~Window() = default;

  void Enter() { OnEnter(); }

  void Exit() { OnExit(); }

  virtual void OnResize(int /*width*/, int /*height*/) {}
  virtual bool OnKeyPressed(ConsoleKeyDetails keyDetails) = 0;
  [[nodiscard]] virtual bool IsCorrectSize(int /*width*/, int /*height*/) const { return true; }
  virtual void ForceRender() {}

protected:
  Window(WindowType t) : type(t) {}

  virtual void OnEnter() = 0;
  virtual void OnExit() = 0;

private:
  WindowType type = WindowType::None;
};
