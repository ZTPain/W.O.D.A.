#pragma once

#include <functional>

class PromptHelper {
public:
  static void ShowYesNoPrompt(
      const char* message, const std::function<void(bool result)>& callback
  );

private:
  static void HandleYesNoInput(const std::function<void(bool result)>& callback);
  static inline std::function<void(bool result)> storedCallback;
  static inline int subscriptionId = -1;
};
