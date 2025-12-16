// SegmentBoardView.h

#pragma once

#include <cstddef>

struct SegmentBoardView {
  const bool* data;
  size_t width;
  size_t height;
};
