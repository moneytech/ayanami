#pragma once

#include "perlin.h"

class fractal_noise : public noise {
public:
  fractal_noise(int noctaves) : noctaves_ { noctaves } {}

  float sample(const nm::float3 &p) const override {
    float freq_mult = 1.0f,
          ampl_mult = 1.0f,
          result    = 0.0f;
    for (int o = 0; o < noctaves_; ++o) {
      result += noise_.sample(p * freq_mult) * ampl_mult;
      ampl_mult *= 0.5f;
      freq_mult *= 2.0f;
    }
    return result;
  }

private:
  const perlin_noise noise_;
  const int noctaves_;
};
