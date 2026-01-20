#include "PromptHelper.h"

#include "BoxDrawing.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/TextHelper.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"

#include <cstddef>

bool PromptHelper::ShowYesNoPrompt(const char* message) {
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

  ConsoleKeyDetails keyDetails{};
  InputManager::GetNextKeyPress(keyDetails);

  return keyDetails.key == ConsoleKey::Y || keyDetails.key == ConsoleKey::Enter;
}
