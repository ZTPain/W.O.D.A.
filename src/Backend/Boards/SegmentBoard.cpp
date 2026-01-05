#include "SegmentBoard.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Units/BattleUnitType.h"
#include <cstddef>
#include <stdexcept>
#include <unordered_map>
#include <vector>

// SegmentBoard::SegmentBoard(size_t width, size_t height) : width(width), height(height) {}

bool SegmentBoard::ToggleSegment(size_t x, size_t y) { // toggle na T/F
  if (x >= width || y >= height)
    return false;

  segments[y][x] = !segments[y][x];

  return true;
}

void SegmentBoard::Clear() { // caly segmentBoard na F
  for (auto& row : segments) {
    for (auto col : row) {
      col = false;
    }
  }
}

const std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>& SegmentBoard::
    GetUnits() const {
  throw std::runtime_error("Not implemented in base SegmentBoard!");
}
