#pragma once

class ConsoleManager {
public:
  static void SetTitle(const char* title);
  static void GetSize(int& width, int& height);
  static void GetCursorPosition(int& x, int& y);
};
