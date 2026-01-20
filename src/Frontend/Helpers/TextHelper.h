#pragma once

#include <chrono>
#include <cstddef>
#include <ostream>

class TextHelper {
public:
  static void DrawCenteredText(int y, const char* text);
  static size_t DrawWrappedText(int x, int y, int maxWidth, const char* text);
  static size_t CalculateWrappedText(
      int x, int y, int maxWidth, const char* text, size_t& usedWidth
  );
  static void FormatDuration(char* buffer, size_t bufferSize, std::chrono::seconds duration);

private:
  static size_t InternalDrawWrappedText(
      std::ostream& os, int x, int y, int maxWidth, const char* text, size_t& usedWidth
  );
};
