#pragma once

#include <memory>
#include <unordered_map>

#include "Api/Window.h"
#include "Frontend/Input/InputManager.h"

class WindowManager {
public:
  void Initialize();

  static WindowManager& GetInstance() {
    static WindowManager instance;
    return instance;
  }

  WindowManager(const WindowManager&) = delete;
  const WindowManager& operator=(const WindowManager&) = delete;

  void UpdatePendingWindow();

  Window* GetCurrentWindow() {
    if (currentWindowType == WindowType::None)
      return nullptr;

    return windows[currentWindowType].get();
  }

  void SwitchToWindow(WindowType type);
  void OnTerminalResize(int width, int height);
  void OnKeyPressed(ConsoleKeyDetails keyDetails);

private:
  std::unordered_map<WindowType, std::unique_ptr<Window>> windows;
  WindowManager() = default;
  WindowType currentWindowType{};
  WindowType pendingWindowType{};
  bool minSizeShown = false;
  static void ShowMinimumSizeMessage();
  void EnterPendingWindow();
};
