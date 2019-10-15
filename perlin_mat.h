#pragma once

#include "material.h"
#include "perlin.h"

class perlin_mat : public material {
public:
  explicit perlin_mat(const nm::float3 &col1,
                      const nm::float3 &col2,
                      noise *n) :
      colors_ { col1, col2 }, noise_(n) {}

  bool scatter(const ray        &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override {
    const float noiseval = 0.5f * noise_->sample(rec.p * 0.25f) + 0.5f;
    attn =  ((1.0f - noiseval) * colors_[0] + (noiseval) * colors_[1]);
    scattered = ray{ rec.p, rec.normal + random_unit_vector() };
    return true;
  }

private:
  nm::float3 colors_[2];
  noise    *noise_;
};
