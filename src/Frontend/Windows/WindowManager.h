#pragma once

#include <memory>
#include <unordered_map>

#include "Api/Window.h"

class WindowManager {
public:
  void Initialize();

  static WindowManager& GetInstance() {
    static WindowManager instance;
    return instance;
  }

  WindowManager(const WindowManager&) = delete;
  const WindowManager& operator=(const WindowManager&) = delete;

  void SwitchToWindow(WindowType type);

private:
  std::unordered_map<WindowType, std::unique_ptr<Window>> windows;
  WindowManager() = default;
  WindowType currentWindowType{};
};
