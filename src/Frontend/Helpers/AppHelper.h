#pragma once

#include "Frontend/Input/InputManager.h"
#include <cstddef>

class AppHelper {
public:
  static void Exit();
  static void AwaitExit();

  static size_t GetWidth() {
    int width = 0;
    int height = 0;
    InputManager::GetTerminalSize(width, height);
    return width;
  }

  static size_t GetHeight() {
    int width = 0;
    int height = 0;
    InputManager::GetTerminalSize(width, height);
    return height;
  }
};
