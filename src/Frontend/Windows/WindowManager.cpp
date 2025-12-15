#include "WindowManager.h"

#include "Frontend/Windows/Api/Window.h"
#include "Implementations/MainMenuWindow.h"
#include <memory>

void WindowManager::Initialize() {
  windows[WindowType::MainMenu] = std::make_unique<MainMenuWindow>();

  windows[WindowType::MainMenu]->Enter();
}
