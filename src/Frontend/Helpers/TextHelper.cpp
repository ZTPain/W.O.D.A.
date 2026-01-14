#include "TextHelper.h"

#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include <chrono>
#include <cstdio>
#include <cstring>

void TextHelper::DrawCenteredText(int y, const char* text) {
  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  const auto textLength = static_cast<int>(std::strlen(text));
  const auto x = ((width - textLength) / 2) + 1;

  IO::cout << AnsiHelper::MoveCursor(x, y) << text;
}

void TextHelper::DrawWrappedText(int x, int y, int maxWidth, const char* text) {
  int currentX = x;
  int currentY = y;

  const char* wordStart = text;
  const char* ptr = text;

  while (*ptr != '\0') {
    if (*ptr == ' ' || *ptr == '\n') {
      const auto wordLength = static_cast<int>(ptr - wordStart);
      if (currentX + wordLength > x + maxWidth) {
        currentX = x;
        currentY++;
      }

      IO::cout << AnsiHelper::MoveCursor(currentX, currentY);
      IO::cout.write(wordStart, wordLength);
      currentX += wordLength + 1;

      if (*ptr == '\n') {
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

    IO::cout << AnsiHelper::MoveCursor(currentX, currentY);
    IO::cout.write(wordStart, wordLength);
  }
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
