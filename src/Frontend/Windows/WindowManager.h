#pragma once

#include <memory>
#include <unordered_map>

#include "Api/Window.h"

class WindowManager {
public:
  void Initialize();

private:
  std::unordered_map<WindowType, std::unique_ptr<Window>> windows;
};
