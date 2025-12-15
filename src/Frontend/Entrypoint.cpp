#include "Input/InputManager.h"
#include "Windows/WindowManager.h"
#include <clocale>
#include <cstdlib>

extern "C" void EntryPoint() {
  WindowManager wm;
  // Set locale to support UTF-8 box drawing characters
  std::setlocale(LC_ALL, "en_US.UTF-8");
#ifdef _WIN32
  std::system("chcp 65001");
#endif
  wm.Initialize();

  InputManager::WaitUntillKeyPressed();
}
