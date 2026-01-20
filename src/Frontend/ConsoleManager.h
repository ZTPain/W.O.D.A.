#pragma once

#include <cstdint>

class ConsoleManager {
public:
  static void SetTitle(const char* title);
  static void GetSize(int& width, int& height);
  static void GetCursorPosition(int& x, int& y);
  static void ConsoleWrite(const uint8_t* data, uint64_t length);
  static void FlushConsole();
};
