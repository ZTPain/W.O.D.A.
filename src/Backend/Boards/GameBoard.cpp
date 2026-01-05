#include "GameBoard.h"
#include "ISegment.h"
#include <cstddef>
#include <memory>

// GameBoard::GameBoard(size_t width, size_t height) : width(width), height(height) {
//   segmentBoard = std::make_unique(SegmentBoard(width, height));
// }

// pain
void GameBoard::ParseSegments() {
  // ...
}

// sprawdza pozycje i F/T ewemntualnie togglesegment + T
bool GameBoard::FireAt(size_t x, size_t y) {
  if (x >= width || y >= height)
    return false;

  if (segmentBoard->Segments()[y][x])
    return false;

  segmentBoard->ToggleSegment(x, y);

  if (units[y][x] != nullptr) {
    units[y][x]->DestroySegment();
  }

  return true;
}

void GameBoard::FixSegment(size_t x, size_t y) {
  if (x >= width || y >= height)
    return;

  segmentBoard->ToggleSegment(x, y);

  if (units[y][x] != nullptr) {
    units[y][x]->FixSegment();
  }
}

// czy unists sa zniszczone
bool GameBoard::IsGameOver() {
  for (const auto& item : units) {
    for (const auto& item2 : item) {
      if (item2 == nullptr)
        continue;

      if (!item2->IsDestroyed())
        return false;
    }
  }

  return true;
}
