#pragma once

#include <chrono>
#include <cstddef>
class TextHelper {
public:
  static void DrawCenteredText(int y, const char* text);
  static void DrawWrappedText(int x, int y, int maxWidth, const char* text);
  static void FormatDuration(char* buffer, size_t bufferSize, std::chrono::seconds duration);
};
