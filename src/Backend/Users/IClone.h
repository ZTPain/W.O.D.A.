// IClone.h

#pragma once

#include <memory>

// Look up CRTP for Clone() implementation
template <class Derived> class IClone {
public:
  virtual ~IClone() = default;
  virtual std::unique_ptr<Derived> Clone() = 0;
};
