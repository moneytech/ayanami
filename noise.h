#pragma once

#include "nicemath.h"

class noise {
public:
  virtual float sample(const nm::float3 &p) const = 0;
};
