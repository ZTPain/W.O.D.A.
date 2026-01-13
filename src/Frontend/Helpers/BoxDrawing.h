#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

enum class BoxStyle : uint8_t { Single, Double, Bold, Rounded };

struct BoxStyleDefinition {
  std::string topLeft;
  std::string topRight;
  std::string bottomLeft;
  std::string bottomRight;
  std::string horizontal;
  std::string vertical;

  std::string breakRight;
  std::string breakLeft;
};

class BoxDrawing {
public:
  static std::unordered_map<BoxStyle, BoxStyleDefinition> boxStyles;

  static void DrawWindowFrame(bool filled, const char* title = nullptr, bool flush = true);

  static void DrawBox(
      size_t x,
      size_t y,
      size_t width,
      size_t height,
      BoxStyle style,
      bool filled,
      const char* title = nullptr,
      bool flush = true
  );

  static void ClearBox(
      size_t x, size_t y, size_t width, size_t height, bool filled, bool flush = true
  );

  static void DrawHorizontalLine(
      size_t x, size_t y, size_t length, BoxStyle style, bool flush = true
  );
  static void DrawVerticalLine(
      size_t x, size_t y, size_t length, BoxStyle style, bool flush = true
  );
};
