#pragma once

#include "perlin.h"

class fractal_noise : public noise {
public:
  fractal_noise(const perlin_noise *oc1,
                const perlin_noise *oc2,
                const perlin_noise *oc3) : octaves_ { oc1, oc2, oc3 } {}

  float sample(const nm::float3 &p) const override {
    return octaves_[0]->sample(p) +
           octaves_[1]->sample(p) +
           octaves_[2]->sample(p);
  }

private:
  const perlin_noise *octaves_[3];
};
