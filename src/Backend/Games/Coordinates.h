// Coordinates.h

#pragma once

#include <cstddef>

struct Coordinates {
  size_t x;
  size_t y;

  Coordinates(size_t x, size_t y) : x(x), y(y) {}
  Coordinates() : Coordinates(0, 0) {}

  bool operator==(const Coordinates& other) const { return x == other.x && y == other.y; }
  bool operator!=(const Coordinates& other) const { return !(*this == other); }
};
