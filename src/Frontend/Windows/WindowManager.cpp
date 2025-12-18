#include "WindowManager.h"

#include "Frontend/Windows/Api/Window.h"
#include <memory>

#include "Implementations/MainMenuWindow.h"
#include "Implementations/POCGameView.h"

void WindowManager::Initialize() {
  windows[WindowType::MainMenu] = std::make_unique<MainMenuWindow>();
  windows[WindowType::InGame] = std::make_unique<POCGameView>();

  windows[WindowType::MainMenu]->Enter();
}
