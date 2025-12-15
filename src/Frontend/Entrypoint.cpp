#include "Input/InputManager.h"
#include <clocale>
#include <cstdlib>

extern "C" void EntryPoint() {
  // Set locale to support UTF-8 box drawing characters
  std::setlocale(LC_ALL, "en_US.UTF-8");
#ifdef _WIN32
  std::system("chcp 65001");
#endif

  InputManager::WaitUntillKeyPressed();
}
