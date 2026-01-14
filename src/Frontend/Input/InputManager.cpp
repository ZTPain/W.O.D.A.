#include "InputManager.h"
#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>

#include "ConsoleKey.h"

#include "Frontend/ConsoleManager.h"
#include "SubscriptionProvider.h"

SubscriptionProvider<OnKeyPressedCallback> InputManager::onKeyPressedProvider;
SubscriptionProvider<OnTerminalResizeCallback> InputManager::onTerminalResizeProvider;

std::queue<ConsoleKeyDetails> InputManager::keyPressQueue;

static std::condition_variable waitForKeyCv;
static std::mutex waitForKeyCvM;

static ConsoleKeyDetails lastKeyDetails;

void InputManager::OnKeyPressed(uint8_t key, uint8_t modifier, int32_t keyCode) {

  lastKeyDetails.keyCode = keyCode;
  lastKeyDetails.key = static_cast<ConsoleKey>(key);
  lastKeyDetails.modifiers = static_cast<ConsoleModifiers>(modifier);

  if (lastKeyDetails.key == ConsoleKey::None) {
    // Fallback to keyCode if key is None
    lastKeyDetails.key = static_cast<ConsoleKey>(lastKeyDetails.keyCode);
  }

  keyPressQueue.push(lastKeyDetails);
}

void InputManager::OnUpdate() {
  while (!keyPressQueue.empty()) {
    const auto keyDetails = keyPressQueue.front();
    keyPressQueue.pop();

    waitForKeyCv.notify_all();

    if (onKeyPressedProvider.Call([keyDetails](const OnKeyPressedCallback& callback) {
          return callback(keyDetails);
        })) {
      continue; // Suppress further processing
    }
  }
}

void InputManager::DiscardPendingKeyPresses() {
  std::scoped_lock const lk(waitForKeyCvM);
  while (!keyPressQueue.empty()) {
    keyPressQueue.pop();
  }
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

void InputManager::GetCursorPosition(int& x, int& y) { ConsoleManager::GetCursorPosition(x, y); }

void InputManager::GetNextKeyPress(ConsoleKeyDetails& keyDetails) {
  WaitUntillKeyPressed();

  keyDetails = lastKeyDetails;
}

void InputManager::GetTerminalSize(int& width, int& height) {
  ConsoleManager::GetSize(width, height);
}

void InputManager::OnTerminalResize(int width, int height) {
  // Handle terminal resize if needed

  InputManager::onTerminalResizeProvider.Call([width,
                                               height](const OnTerminalResizeCallback& callback) {
    callback(width, height);
    return false;
  });
}

bool InputManager::TryGetKeyPress(ConsoleKeyDetails& keyDetails) {
  if (keyPressQueue.empty()) {
    return false;
  }

  keyDetails = keyPressQueue.front();
  keyPressQueue.pop();
  return true;
}
