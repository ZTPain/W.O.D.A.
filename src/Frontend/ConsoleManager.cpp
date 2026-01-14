#include "ConsoleManager.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/WindowManager.h"
#include <cstdint>
#include <iostream>

extern "C" void OnTerminalResize(int width, int height) {
  InputManager::OnTerminalResize(width, height);
}

static void (*consoleSetTitleCallback)(const char*) = nullptr;
extern "C" void SetConsoleSetTitleCallback(void (*callback)(const char*)) {
  consoleSetTitleCallback = callback;
}

static void (*consoleGetSizeCallback)(int*, int*) = nullptr;
extern "C" void SetConsoleGetSizeCallback(void (*callback)(int*, int*)) {
  consoleGetSizeCallback = callback;
}

using CallbackFunction = void (*)(int* x, int* y);
static CallbackFunction consoleGetCursorPositionCallback = nullptr;
extern "C" void SetConsoleGetCursorPositionCallback(CallbackFunction callback) {
  consoleGetCursorPositionCallback = callback;
}

extern "C" void OnKeyPressed(uint8_t key, uint8_t modifier, int32_t keyCode) {
  InputManager::OnKeyPressed(key, modifier, keyCode);
}

void ConsoleManager::SetTitle(const char* title) { consoleSetTitleCallback(title); }

void ConsoleManager::GetCursorPosition(int& x, int& y) {
  IO::cout.flush();
  consoleGetCursorPositionCallback(&x, &y);
}

void ConsoleManager::GetSize(int& width, int& height) { consoleGetSizeCallback(&width, &height); }

static void (*consoleWriteCallback)(const uint8_t*, uint64_t) = nullptr;
extern "C" void SetConsoleWriteCallback(void (*callback)(const uint8_t*, uint64_t)) {
  consoleWriteCallback = callback;
}

void ConsoleManager::ConsoleWrite(const uint8_t* data, uint64_t length) {
  consoleWriteCallback(data, length);
}

static void (*consoleFlushCallback)() = nullptr;
extern "C" void SetConsoleFlushCallback(void (*callback)()) { consoleFlushCallback = callback; }

void ConsoleManager::FlushConsole() { consoleFlushCallback(); }

extern "C" void OnUpdate() {
  WindowManager::GetInstance().UpdatePendingWindow();
  InputManager::OnUpdate();
}
