#pragma once

#include "ConsoleKey.h"
#include <cstdint>
#include <functional>
#include <queue>
#include <unordered_map>

struct ConsoleKeyDetails {
  ConsoleKey key;
  ConsoleModifiers modifiers;
  int32_t keyCode;
};

// Returns true to suppress further processing of the key press
using OnKeyPressedCallback = std::function<bool(ConsoleKeyDetails)>;
class InputManager {
public:
  static int SubscribeToOnKeyPressed(const OnKeyPressedCallback& callback);
  static int UnsubscribeFromOnKeyPressed(int subscriptionId);
  static void NotifyKeyPressed(ConsoleKeyDetails keyDetails);

  static void WaitUntillKeyPressed(bool flushQueue = false);
  static void GetCursorPosition(int& x, int& y);
  static void GetNextKeyPress(ConsoleKeyDetails& keyDetails);

private:
  static std::unordered_map<uint8_t, OnKeyPressedCallback> keyPressedCallbacks;
  static uint8_t nextCallbackId;

  static std::queue<ConsoleKeyDetails> keyPressQueue;
};
