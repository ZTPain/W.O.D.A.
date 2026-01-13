#include "Frontend/ConsoleManager.h"
#include "Frontend/Input/IO.h"
#include "Helpers/AnsiHelper.h"
#include "Helpers/AppHelper.h"
#include "Windows/WindowManager.h"
#include <clocale>
#include <cstdlib>
#include <iostream>

extern "C" void EntryPoint() {
  IO::Initialize();
  IO::cout << AnsiHelper::EnableAltScreenBuffer();
  IO::cout << AnsiHelper::HideCursor();

  // Set locale to support UTF-8 box drawing characters
  std::setlocale(LC_ALL, "en_US.UTF-8");
#ifdef _WIN32
  std::system("chcp 65001");
#endif

  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();
  IO::cout.flush();

  ConsoleManager::SetTitle("W.O.D.A - Waiting for input...");

  WindowManager::GetInstance().Initialize();

  AppHelper::AwaitExit();
}
