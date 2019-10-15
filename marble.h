#pragma once

#include "material.h"
#include "perlin.h"
#include "random.h"

class marble : public material {
public:
  explicit marble(const nm::float3 &col1,
                  const nm::float3 &col2,
                  float             scale,
                  noise            *n) :
      colors_ { col1, col2 }, noise_(n), scale_(scale) {}

  bool scatter(const ray        &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override {
    const float noiseval = 0.5f * (1.0f + sin(scale_ * rec.p.x() + 10.0f*fabs(noise_->sample(rec.p))));
    attn =  ((1.0f - noiseval) * colors_[0] + (noiseval) * colors_[1]);
    scattered = ray{ rec.p, rec.normal + random_unit_vector() };
    return true;
  }

private:
  nm::float3   colors_[2];
  const noise *noise_;
  const float  scale_;
};
