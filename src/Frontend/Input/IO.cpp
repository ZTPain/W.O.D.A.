#include "IO.h"

#include "Frontend/ConsoleManager.h"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <streambuf>

std::ostream IO::cout = std::ostream(nullptr);

class CustomOstreamBuf : public std::streambuf {
public:
  CustomOstreamBuf() = default;

protected:
  int_type overflow(int_type c) override {
    if (c != traits_type::eof()) {
      auto const ch = static_cast<uint8_t>(c);
      ConsoleManager::ConsoleWrite(&ch, 1);
    }
    return c;
  }

  int sync() override {
    ConsoleManager::FlushConsole();
    return 0;
  }
};

void IO::Initialize() {
  // static CustomOstreamBuf customBuf;
  // cout.rdbuf(&customBuf);
  cout.rdbuf(std::cout.rdbuf());
}
