#pragma once

#include "Backend/Users/UserProfile.h"
#include "Frontend//Windows/Api/Window.h"
#include "Frontend/Input/InputManager.h"
#include <array>
#include <cstddef>

class SettingsWindow : public Window {
public:
  SettingsWindow() : Window(WindowType::Settings) {};
  ~SettingsWindow() override = default;

  void OnEnter() override;
  void OnExit() override;
  bool OnKeyPressed(ConsoleKeyDetails keyDetails) override;
  void OnResize(int width, int height) override;
  [[nodiscard]] bool IsCorrectSize(int width, int height) const override;

private:
  void ForceRender() override;

  size_t selectedIndex = 0;

  void RenderOptions() const;
  void RenderOption(size_t index) const;

  static constexpr const std::array<const char*, 4> OPTION_NAMES = {
      // "Unit Color",
      // "Board Color",
      "Border Color",
      "Border Pattern",
      "Unit Pattern",
      "Auto Mark Empty Fields",
  };

  static void RenderOptionValue(size_t index);
  static void RenderOptionUnitPattern(UnitPattern value);
  static void RenderOptionBorderPattern(BorderPattern value);
  static void RenderOptionColor(Color value);
  static void RenderOptionBoolean(bool value);

  void RenderOptionValueChangeMenu(size_t index) const;

  void ChangeSelectedSetting(bool increase);
  static void ChangeUnitPatternSetting(bool increase);
  static void ChangeBorderPatternSetting(bool increase);
  static void ChangeBorderColorSetting(bool increase);
  static void ChangeBoardColorSetting(bool increase);
  static void ChangeUnitColorSetting(bool increase);
};
