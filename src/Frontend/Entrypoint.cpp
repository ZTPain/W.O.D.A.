#include "Frontend/ConsoleManager.h"
#include "Helpers/AnsiHelper.h"
#include "Input/InputManager.h"
#include "Windows/WindowManager.h"
#include <clocale>
#include <cstdlib>
#include <iostream>

extern "C" void EntryPoint() {
  WindowManager wm;

  // Set locale to support UTF-8 box drawing characters
  std::setlocale(LC_ALL, "en_US.UTF-8");
#ifdef _WIN32
  std::system("chcp 65001");
#endif

  std::cout << ANSI_HIDE_CURSOR << ANSI_RESET;
  std::cout << ANSI_CLEAR_SCREEN;
  std::cout.flush();

  ConsoleManager::SetTitle("W.O.D.A - Waiting for input...");

  wm.Initialize();

  while (true)
    InputManager::WaitUntillKeyPressed();
}
