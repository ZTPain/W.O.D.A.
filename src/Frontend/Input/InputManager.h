#pragma once

#include "ConsoleKey.h"
#include "SubscriptionProvider.h"
#include <cstdint>
#include <functional>
#include <queue>
#include <string>

struct ConsoleKeyDetails {
  ConsoleKey key;
  ConsoleModifiers modifiers;
  int32_t keyCode;
};

// Returns true to suppress further processing of the key press
using OnKeyPressedCallback = std::function<bool(ConsoleKeyDetails)>;

using OnTerminalResizeCallback = std::function<void(int width, int height)>;

// Returns true if complete
using OnTextInputCallback = std::function<bool(const std::string&)>;

using TextInputValidator =
    std::function<bool(const std::string&, const ConsoleKeyDetails& keyDetails)>;

class InputManager {
public:
  static void StartReadingTextAsync(const OnTextInputCallback& callback);
  static void StartReadingTextBlocking(
      std::string& outText, const TextInputValidator& validator = nullptr
  );

  static void WaitUntillKeyPressed(bool flushQueue = false);
  static void GetCursorPosition(int& x, int& y);
  static void GetNextKeyPress(ConsoleKeyDetails& keyDetails);
  static void GetTerminalSize(int& width, int& height);

  static void OnKeyPressed(uint8_t key, uint8_t modifier, int32_t keyCode);
  static void OnTerminalResize(int width, int height);

  static SubscriptionProvider<OnKeyPressedCallback> onKeyPressedProvider;
  static SubscriptionProvider<OnTerminalResizeCallback> onTerminalResizeProvider;

private:
  static std::queue<ConsoleKeyDetails> keyPressQueue;
};
