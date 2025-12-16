#include "InputManager.h"
#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "ConsoleKey.h"

std::unordered_map<uint8_t, OnKeyPressedCallback> InputManager::keyPressedCallbacks;
uint8_t InputManager::nextCallbackId = 0;
std::queue<ConsoleKeyDetails> InputManager::keyPressQueue;

static std::condition_variable waitForKeyCv;
static std::mutex waitForKeyCvM;

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

void InputManager::NotifyKeyPressed(ConsoleKeyDetails keyDetails) {
  for (auto& callback : keyPressedCallbacks) {
    if (callback.second(keyDetails)) {
      return; // Suppress further processing
    }
  }

  keyPressQueue.push(keyDetails);
}

static ConsoleKeyDetails lastKeyDetails;

extern "C" void OnKeyPressed(uint8_t key, uint8_t modifier, int32_t keyCode) {

  lastKeyDetails.keyCode = keyCode;
  lastKeyDetails.key = static_cast<ConsoleKey>(key);
  lastKeyDetails.modifiers = static_cast<ConsoleModifiers>(modifier);

  if (lastKeyDetails.key == ConsoleKey::None) {
    // Fallback to keyCode if key is None
    lastKeyDetails.key = static_cast<ConsoleKey>(lastKeyDetails.keyCode);
  }

  waitForKeyCv.notify_all();

  InputManager::NotifyKeyPressed(lastKeyDetails);
}

using CallbackFunction = void (*)(int* x, int* y);
static CallbackFunction consoleGetCursorPositionCallback = nullptr;
extern "C" void SetConsoleGetCursorPositionCallback(CallbackFunction callback) {
  consoleGetCursorPositionCallback = callback;
}

void InputManager::WaitUntillKeyPressed(bool flushQueue) {
  if (flushQueue) {
    while (!keyPressQueue.empty()) {
      keyPressQueue.pop();
    }
  } else if (!keyPressQueue.empty()) {
    return;
  }

  std::unique_lock<std::mutex> lk(waitForKeyCvM);
  waitForKeyCv.wait(lk);
}

void InputManager::GetCursorPosition(int& x, int& y) {
  std::cout.flush();
  consoleGetCursorPositionCallback(&x, &y);
}

void InputManager::GetNextKeyPress(ConsoleKeyDetails& keyDetails) {
  WaitUntillKeyPressed();

  keyDetails = lastKeyDetails;
}
