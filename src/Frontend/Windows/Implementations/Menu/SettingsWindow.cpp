#include "SettingsWindow.h"

#include "Backend/Main/Battleships.h"
#include "Backend/Users/AchievementPool.h"
#include "Backend/Users/UserManager.h"
#include "Backend/Users/UserProfile.h"
#include "Frontend/Helpers/AnsiHelper.h"
#include "Frontend/Helpers/BoxDrawing.h"
#include "Frontend/Input/ConsoleKey.h"
#include "Frontend/Input/IO.h"
#include "Frontend/Input/InputManager.h"
#include "Frontend/Windows/Api/Window.h"
#include "Frontend/Windows/WindowManager.h"
#include <cstddef>

void SettingsWindow::OnEnter() { ForceRender(); }

void SettingsWindow::OnExit() { IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset(); }

bool SettingsWindow::OnKeyPressed(ConsoleKeyDetails keyDetails) {
  if (keyDetails.key == ConsoleKey::Escape) {
    WindowManager::GetInstance().SwitchToWindow(WindowType::MainMenu);
    return true;
  }

  switch (keyDetails.key) {
    case ConsoleKey::W:
    case ConsoleKey::UpArrow:
      if (selectedIndex > 0) {
        --selectedIndex;
        RenderOptions();
        IO::cout.flush();
      }
      return true;

    case ConsoleKey::S:
    case ConsoleKey::DownArrow:
      if (selectedIndex + 1 < OPTION_NAMES.size()) {
        ++selectedIndex;
        RenderOptions();
        IO::cout.flush();
      }
      return true;

    case ConsoleKey::A:
    case ConsoleKey::LeftArrow:
      ChangeSelectedSetting(false);
      return true;

    case ConsoleKey::D:
    case ConsoleKey::RightArrow:
      ChangeSelectedSetting(true);
      return true;

    default:
      return false;
  }

  return false;
}

void SettingsWindow::OnResize(int /*width*/, int /*height*/) { ForceRender(); }

bool SettingsWindow::IsCorrectSize(int width, int height) const {
  const auto requiredWidth = 70;
  const auto requiredHeight = 12;
  return static_cast<size_t>(width) >= requiredWidth &&
         static_cast<size_t>(height) >= requiredHeight;
}

void SettingsWindow::ForceRender() {
  IO::cout << AnsiHelper::ClearScreen() << AnsiHelper::Reset();

  BoxDrawing::DrawWindowFrame(true, "Settings");

  RenderOptions();

  IO::cout.flush();
}

void SettingsWindow::RenderOptions() const {
  for (size_t i = 0; i < OPTION_NAMES.size(); ++i) {
    RenderOption(i);
  }
}

void SettingsWindow::RenderOption(size_t index) const {
  const auto x = 12;
  const auto y = 3 + (index * 2);
  IO::cout << AnsiHelper::MoveCursor(x, y);
  if (selectedIndex == index) {
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Black)
             << AnsiHelper::SetBackgroundColor(AnsiColor::White);
  }
  IO::cout << OPTION_NAMES.at(index) << ": ";
  IO::cout << AnsiHelper::MoveCursor(x + 40, y);
  IO::cout << "<< ";
  RenderOptionValue(index);
  if (selectedIndex == index)
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Black);
  else
    IO::cout << AnsiHelper::SetTextColor(AnsiColor::Default);
  IO::cout << " >>";
  if (selectedIndex == index) {
    IO::cout << AnsiHelper::Reset();
  }

  IO::cout << "                ";
}

void SettingsWindow::RenderOptionValue(size_t index) {
  const auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  const auto& settings = currentUser.settings;

  switch (index) {
      // case 0:
      //   RenderOptionColor(settings.unitColor);
      //   break;

      // case 1:
      //   RenderOptionColor(settings.boardColor);
      //   break;

    // NOLINTNEXTLINE
    case 2 - 2:
      RenderOptionColor(settings.borderColor);
      break;

    case 3 - 2:
      RenderOptionBorderPattern(settings.borderPattern);
      break;

    case 4 - 2:
      RenderOptionUnitPattern(settings.unitPattern);
      break;

    case 5 - 2:
      RenderOptionBoolean(settings.autoMarkEmptyFields);
      break;

    default:
      IO::cout << "Unknown";
      break;
  }
}

void SettingsWindow::RenderOptionUnitPattern(UnitPattern value) {
  switch (value) {
    case UnitPattern::Default:
      IO::cout << "Default";
      break;
    case UnitPattern::FlowerShipIcon:
      IO::cout << "Flower Ship Icon";
      break;
    case UnitPattern::CrosshairShipIcon:
      IO::cout << "Crosshair Ship Icon";
      break;
    case UnitPattern::StarShipIcon:
      IO::cout << "Star Ship Icon";
      break;
    case UnitPattern::StoneShipIcon:
      IO::cout << "Stone Ship Icon";
      break;
    default:
      IO::cout << "Unknown";
      break;
  }
}

void SettingsWindow::RenderOptionBorderPattern(BorderPattern value) {
  switch (value) {
    case BorderPattern::Default:
      IO::cout << "Default";
      break;

    case BorderPattern::Double:
      IO::cout << "Double";
      break;

    case BorderPattern::Bold:
      IO::cout << "Bold";
      break;

    case BorderPattern::Rounded:
      IO::cout << "Rounded";
      break;

    default:
      IO::cout << "Unknown";
      break;
  }
}

void SettingsWindow::RenderOptionColor(Color value) {
  switch (value) {
    case Color::Default:
      IO::cout << "Default";
      break;

    case Color::Red:
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Red) << "Red";
      break;

    case Color::Green:
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Green) << "Green";
      break;

    case Color::Blue:
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Blue) << "Blue";
      break;

    case Color::Gold:
      IO::cout << AnsiHelper::SetTextColor(255, 215, 0) << "Gold";
      break;

    case Color::Black:
      IO::cout << AnsiHelper::SetTextColor(AnsiColor::Black) << "Black";
      break;

    default:
      IO::cout << "Unknown";
  }
}

void SettingsWindow::RenderOptionBoolean(bool value) {
  IO::cout << (value ? "Enabled" : "Disabled");
}

void SettingsWindow::ChangeSelectedSetting(bool increase) {
  auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  auto& settings = currentUser.settings;

  switch (selectedIndex) {
      // case 0: // Unit Color
      //   ChangeUnitColorSetting(increase);
      //   break;

      // case 1: // Board Color
      //   ChangeBoardColorSetting(increase);
      //   break;

    // NOLINTNEXTLINE
    case 2 - 2: // Border Color
      ChangeBorderColorSetting(increase);
      break;

    case 3 - 2: // Border Pattern
      ChangeBorderPatternSetting(increase);
      break;

    case 4 - 2: // Unit Pattern
      ChangeUnitPatternSetting(increase);
      break;

    case 5 - 2: // Auto Mark Empty Fields
      settings.autoMarkEmptyFields = !settings.autoMarkEmptyFields;
      break;

    default:
      break;
  }

  ForceRender(); // To refresh border
  Battleships::GetInstance().WriteToSave();
  IO::cout.flush();
}

void SettingsWindow::ChangeUnitPatternSetting(bool increase) {
  auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  auto& settings = currentUser.settings;

  int patternValue = static_cast<int>(settings.unitPattern);
  patternValue += increase ? 1 : -1;
  patternValue = (patternValue + static_cast<int>(UnitPattern::ValueCount)) %
                 static_cast<int>(UnitPattern::ValueCount);

  while (true) {
    const auto flowerSelectedButNotUnlocked =
        (patternValue == static_cast<int>(UnitPattern::FlowerShipIcon)) &&
        (!(currentUser.unlockedContent & UnlockableContent::FlowerShipIcon));

    const auto crosshairSelectedButNotUnlocked =
        (patternValue == static_cast<int>(UnitPattern::CrosshairShipIcon)) &&
        (!(currentUser.unlockedContent & UnlockableContent::CrosshairShipIcon));

    const auto starSelectedButNotUnlocked =
        (patternValue == static_cast<int>(UnitPattern::StarShipIcon)) &&
        (!(currentUser.unlockedContent & UnlockableContent::StarShipIcon));

    const auto stoneSelectedButNotUnlocked =
        (patternValue == static_cast<int>(UnitPattern::StoneShipIcon)) &&
        (!(currentUser.unlockedContent & UnlockableContent::StoneShipIcon));

    if (flowerSelectedButNotUnlocked || crosshairSelectedButNotUnlocked ||
        starSelectedButNotUnlocked || stoneSelectedButNotUnlocked) {
      patternValue += increase ? 1 : -1;
      patternValue = (patternValue + static_cast<int>(UnitPattern::ValueCount)) %
                     static_cast<int>(UnitPattern::ValueCount);
      continue;
    }

    break;
  }

  settings.unitPattern = static_cast<UnitPattern>(patternValue);
}

void SettingsWindow::ChangeBorderPatternSetting(bool increase) {
  auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  auto& settings = currentUser.settings;

  int patternValue = static_cast<int>(settings.borderPattern);
  patternValue += increase ? 1 : -1;
  patternValue = (patternValue + static_cast<int>(BorderPattern::ValueCount)) %
                 static_cast<int>(BorderPattern::ValueCount);
  settings.borderPattern = static_cast<BorderPattern>(patternValue);
}

void SettingsWindow::ChangeBorderColorSetting(bool increase) {
  auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  auto& settings = currentUser.settings;

  int colorValue = static_cast<int>(settings.borderColor);
  colorValue += increase ? 1 : -1;
  colorValue =
      (colorValue + static_cast<int>(Color::ValueCount)) % static_cast<int>(Color::ValueCount);

  while (true) {
    const auto redSelectedButNotUnlocked =
        (colorValue == static_cast<int>(Color::Red)) &&
        (!(currentUser.unlockedContent & UnlockableContent::RedBorderColor));
    const auto blueSelectedButNotUnlocked =
        (colorValue == static_cast<int>(Color::Blue)) &&
        (!(currentUser.unlockedContent & UnlockableContent::BlueBorderColor));

    if (redSelectedButNotUnlocked || blueSelectedButNotUnlocked) {
      colorValue += increase ? 1 : -1;
      colorValue =
          (colorValue + static_cast<int>(Color::ValueCount)) % static_cast<int>(Color::ValueCount);
      continue;
    }

    break;
  }

  settings.borderColor = static_cast<Color>(colorValue);
}

void SettingsWindow::ChangeBoardColorSetting(bool increase) {
  auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  auto& settings = currentUser.settings;

  int colorValue = static_cast<int>(settings.boardColor);
  colorValue += increase ? 1 : -1;
  colorValue =
      (colorValue + static_cast<int>(Color::ValueCount)) % static_cast<int>(Color::ValueCount);
  settings.boardColor = static_cast<Color>(colorValue);
}

void SettingsWindow::ChangeUnitColorSetting(bool increase) {
  auto& currentUser = UserManager::GetInstance().GetCurrentUser();
  auto& settings = currentUser.settings;

  int colorValue = static_cast<int>(settings.unitColor);
  colorValue += increase ? 1 : -1;
  colorValue =
      (colorValue + static_cast<int>(Color::ValueCount)) % static_cast<int>(Color::ValueCount);
  settings.unitColor = static_cast<Color>(colorValue);
}

