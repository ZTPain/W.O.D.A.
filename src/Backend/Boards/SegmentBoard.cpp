#include "SegmentBoard.h"
#include "Backend/Computers/ComputerStrategyHelper.h"
#include "ISegment.h"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

SegmentBoard::SegmentBoard(size_t width, size_t height, bool hasLandSegments)
    : width(width), height(height), segments(height, std::vector<bool>(width, false)) {
  if (hasLandSegments) {
    landSegments = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));

    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        if (x < 10)
          landSegments->at(y)[x] = true;
        else if (x >= (width - 10))
          landSegments->at(y)[x] = false;
        else {
          if (landSegments->at(y)[x - 1] && ComputerStrategyHelper::GetRandomFromRange(0, 100) < 50)
            landSegments->at(y)[x] = true;
          else
            landSegments->at(y)[x] = false;
        }
      }
    }

  } else {
    landSegments = std::nullopt;
  }
}
bool SegmentBoard::ToggleSegment(size_t x, size_t y) {
  if (x >= width || y >= height)
    return false;

  segments[y][x] = !segments[y][x];

  return true;
}

void SegmentBoard::Clear() {
  for (auto& row : segments) {
    std::fill(row.begin(), row.end(), false);
  }
}

const UnitsMap& SegmentBoard::GetUnits() const {
  throw std::runtime_error("Not implemented in base SegmentBoard!");
}

size_t SegmentBoard::Width() const { return width; }

size_t SegmentBoard::Height() const { return height; }

const std::vector<std::vector<bool>>& SegmentBoard::Segments() const { return segments; }

const std::vector<std::vector<bool>>& SegmentBoard::LandSegments() const {
  if (!landSegments.has_value()) {
    throw std::runtime_error("Land segments not available in this SegmentBoard!");
  }

  return landSegments.value();
}

std::unique_ptr<ISegment> SegmentBoard::Clone() const {
  return std::make_unique<SegmentBoard>(*this);
}
