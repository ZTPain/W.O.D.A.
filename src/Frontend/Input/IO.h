#pragma once

#include <ostream>

class IO {
public:
  static void Initialize();
  static std::ostream cout;
};
