#include "PromptHelper.h"

#include "BoxDrawing.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/TextHelper.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"

#include <cstddef>
#include <functional>

void PromptHelper::ShowYesNoPrompt(
    const char* message, const std::function<void(bool result)>& callback
) {
  int width = 0;
  int height = 0;
  InputManager::GetTerminalSize(width, height);

  size_t const windowWidth = 50;
  size_t const windowHeight = 7;

  size_t const posX = (width - windowWidth) / 2;
  size_t const posY = (height - windowHeight) / 2;

  BoxDrawing::DrawBox(
      posX, posY, windowWidth, windowHeight, BoxStyle::Single, true, "Confirmation"
  );

  TextHelper::DrawWrappedText(
      static_cast<int>(posX) + 2, static_cast<int>(posY) + 2, windowWidth - 4, message
  );

  IO::cout << AnsiHelper::MoveCursor(
                  static_cast<int>(posX) + 2, static_cast<int>(posY) + windowHeight - 2
              )
           << "[Y] Yes    [N] No";
  IO::cout.flush();

  storedCallback = callback;

  subscriptionId = InputManager::onKeyPressedProvider.Subscribe([](ConsoleKeyDetails details) {
    if (details.key == ConsoleKey::Y) {
      PromptHelper::storedCallback(true);
      InputManager::onKeyPressedProvider.Unsubscribe(PromptHelper::subscriptionId);
      PromptHelper::subscriptionId = -1;
      return true;
    }

    if (details.key == ConsoleKey::N) {
      PromptHelper::storedCallback(false);
      InputManager::onKeyPressedProvider.Unsubscribe(PromptHelper::subscriptionId);
      PromptHelper::subscriptionId = -1;
      return true;
    }

    return true;
  });
}
