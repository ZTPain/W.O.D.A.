#pragma once

#include "ConsoleKey.h"
#include <cstdint>
#include <functional>
#include <unordered_map>

using OnKeyPressedCallback = std::function<void(ConsoleKey, ConsoleModifiers)>;
class InputManager {
public:
  static int SubscribeToOnKeyPressed(const OnKeyPressedCallback& callback);
  static int UnsubscribeFromOnKeyPressed(int subscriptionId);
  static void NotifyKeyPressed(ConsoleKey key, ConsoleModifiers modifiers);

  static void WaitUntillKeyPressed();
  static void GetCursorPosition(int& x, int& y);
  static void GetNextKeyPress(ConsoleKey& key, ConsoleModifiers& modifiers);

private:
  static std::unordered_map<uint8_t, OnKeyPressedCallback> keyPressedCallbacks;
  static uint8_t nextCallbackId;
};
