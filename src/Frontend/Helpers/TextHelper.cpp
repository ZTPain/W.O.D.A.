#include "TextHelper.h"

#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ostream>

void TextHelper::DrawCenteredText(int y, const char* text) {
  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  const auto textLength = static_cast<int>(std::strlen(text));
  const auto x = ((width - textLength) / 2) + 1;

  IO::cout << AnsiHelper::MoveCursor(x, y) << text;
}

size_t TextHelper::DrawWrappedText(int x, int y, int maxWidth, const char* text) {
  size_t usedWidth = 0;
  return InternalDrawWrappedText(IO::cout, x, y, maxWidth, text, usedWidth);
}

size_t TextHelper::CalculateWrappedText(
    int x, int y, int maxWidth, const char* text, size_t& usedWidth
) {
  std::ostream os(nullptr);
  return InternalDrawWrappedText(os, x, y, maxWidth, text, usedWidth);
}

void TextHelper::FormatDuration(char* buffer, size_t bufferSize, std::chrono::seconds duration) {
  const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
  duration -= minutes;
  const auto seconds = duration;

  std::snprintf(
      buffer,
      bufferSize,
      "%02lld:%02lld",
      static_cast<long long>(minutes.count()),
      static_cast<long long>(seconds.count())
  );
}

size_t TextHelper::InternalDrawWrappedText(
    std::ostream& os, int x, int y, int maxWidth, const char* text, size_t& usedWidth
) {
  int currentX = x;
  int currentY = y;

  const char* wordStart = text;
  const char* ptr = text;

  while (*ptr != '\0') {
    if (*ptr == ' ' || *ptr == '\n') {
      const auto wordLength = static_cast<int>(ptr - wordStart);
      if (currentX + wordLength > x + maxWidth) {
        usedWidth = static_cast<size_t>(std::max(usedWidth, static_cast<size_t>(currentX - x)));
        currentX = x;
        currentY++;
      }

      os << AnsiHelper::MoveCursor(currentX, currentY);
      os.write(wordStart, wordLength);
      currentX += wordLength + 1;

      if (*ptr == '\n') {
        usedWidth = static_cast<size_t>(std::max(usedWidth, static_cast<size_t>(currentX - x)));
        currentX = x;
        currentY++;
      }

      wordStart = ptr + 1;
    }

    ptr++;
  }

  // Print the last word
  if (wordStart != ptr) {
    const auto wordLength = static_cast<int>(ptr - wordStart);
    if (currentX + wordLength > x + maxWidth) {
      currentX = x;
      currentY++;
    }

    os << AnsiHelper::MoveCursor(currentX, currentY);
    os.write(wordStart, wordLength);
    usedWidth = static_cast<size_t>(std::max(usedWidth, static_cast<size_t>(currentX - x)));
  }

  return currentY - y + 1;
}
