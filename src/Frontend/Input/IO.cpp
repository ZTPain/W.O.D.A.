#include "IO.h"

#include "Frontend/ConsoleManager.h"
#include <array>
#include <cstdint>
#include <cstdio>
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
  static std::array<char, 16384> stdCoutBuffer{};
  setvbuf(stdout, stdCoutBuffer.data(), _IOLBF, stdCoutBuffer.size());

  // static CustomOstreamBuf customBuf;
  // cout.rdbuf(&customBuf);
  cout.rdbuf(std::cout.rdbuf());
}
