#include "InputManager.h"
#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "ConsoleKey.h"

std::unordered_map<uint8_t, OnKeyPressedCallback> InputManager::keyPressedCallbacks;
uint8_t InputManager::nextCallbackId = 0;

static std::condition_variable waitForKeyCv;
static std::mutex waitForKeyCvM;

static bool suppressNextKeyPress = false;

int InputManager::SubscribeToOnKeyPressed(const OnKeyPressedCallback& callback) {
  keyPressedCallbacks[nextCallbackId] = callback;
  return nextCallbackId++;
}

int InputManager::UnsubscribeFromOnKeyPressed(int subscriptionId) {
  if (subscriptionId < 0 || subscriptionId >= static_cast<int>(nextCallbackId)) {
    return -1; // Invalid subscription ID
  }

  keyPressedCallbacks.erase(subscriptionId);
  return 0;
}

void InputManager::NotifyKeyPressed(ConsoleKey key, ConsoleModifiers modifiers) {
  for (auto& callback : keyPressedCallbacks) {
    callback.second(key, modifiers);
  }
}

static int lastKeyCode = -1;
static ConsoleKey lastKeyPressed = ConsoleKey::None;
static ConsoleModifiers lastModifiers = ConsoleModifiers::None;

extern "C" void OnKeyPressed(uint8_t key, uint8_t modifier, int32_t keyCode) {

  lastKeyCode = keyCode;
  lastKeyPressed = static_cast<ConsoleKey>(key);
  lastModifiers = static_cast<ConsoleModifiers>(modifier);

  if (lastKeyPressed == ConsoleKey::None) {
    // Fallback to keyCode if key is None
    lastKeyPressed = static_cast<ConsoleKey>(lastKeyCode);
  }

  if (suppressNextKeyPress) {
    suppressNextKeyPress = false;
    waitForKeyCv.notify_all();
    return;
  }

  waitForKeyCv.notify_all();

  InputManager::NotifyKeyPressed(lastKeyPressed, lastModifiers);
}

using CallbackFunction = void (*)(int* x, int* y);
static CallbackFunction consoleGetCursorPositionCallback = nullptr;
extern "C" void SetConsoleGetCursorPositionCallback(CallbackFunction callback) {
  consoleGetCursorPositionCallback = callback;
}

void InputManager::WaitUntillKeyPressed() {
  std::unique_lock<std::mutex> lk(waitForKeyCvM);
  waitForKeyCv.wait(lk);
}

void InputManager::GetCursorPosition(int& x, int& y) {
  std::cout.flush();
  consoleGetCursorPositionCallback(&x, &y);
}

void InputManager::GetNextKeyPress(ConsoleKey& key, ConsoleModifiers& modifiers) {
  suppressNextKeyPress = true;
  WaitUntillKeyPressed();

  key = lastKeyPressed;
  modifiers = lastModifiers;
}
