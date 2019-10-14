#pragma once

#include "noise.h"

class perlin_noise : public noise {
public:
  perlin_noise(float freq_factor = 1.0f, float amplitude_factor = 1.0f);
  float sample(const nm::float3 &point) const override;

private:
  const nm::float3 grad(const nm::float3 &p) const;

  static constexpr int kNumGradients = 256;
  nm::float3 gradients_[kNumGradients] = {};
  int perms_[3][kNumGradients];
  const float freq_, amplitude_;
};
