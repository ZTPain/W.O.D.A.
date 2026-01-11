#pragma once

class TextHelper {
public:
  static void DrawCenteredText(int y, const char* text);
  static void DrawWrappedText(int x, int y, int maxWidth, const char* text);
};
