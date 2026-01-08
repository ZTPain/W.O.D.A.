#include "SegmentBoard.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Units/BattleUnitType.h"
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <unordered_map>
#include <vector>

SegmentBoard::SegmentBoard(size_t width, size_t height)
    : width(width), height(height), segments(height, std::vector<bool>(width, false)) {}

bool SegmentBoard::ToggleSegment(size_t x, size_t y) { // toggle na T/F
  if (x >= width || y >= height)
    return false;

  segments[y][x] = !segments[y][x];

  return true;
}

void SegmentBoard::Clear() { // caly segmentBoard na F
  for (auto& row : segments) {
    std::fill(row.begin(), row.end(), false);
  }
}

void SegmentBoard::GetUnits(
    std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>&
) const {
  throw std::runtime_error("Not implemented in base SegmentBoard!");
}

size_t SegmentBoard::Width() const { return width; }

size_t SegmentBoard::Height() const { return height; }

const std::vector<std::vector<bool>>& SegmentBoard::Segments() const { return segments; }
