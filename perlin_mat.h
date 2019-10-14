#pragma once

#include "material.h"
#include "perlin.h"

class perlin_mat : public material {
public:
  explicit perlin_mat(material *src1, material *src2) : src1_(src1), src2_(src2) {}

  bool scatter(const ray        &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override {
    nm::float3 attns[2];
    ray        outsc[2];
    const bool result[] = { src1_->scatter(in, rec, attns[0], outsc[0]),
                            src2_->scatter(in, rec, attns[1], outsc[1]) };

    if (result[0] || result[1]) {
      const float noiseval = 0.5f * noise_.sample(rec.p * 0.25f) + 0.5f;
      attn =  ((1.0f - noiseval) * attns[0] + (noiseval) * attns[1]);
      scattered = ray(rec.p,
                      noiseval * outsc[0].direction() + (1.0f - noiseval) * outsc[1].direction());
    }
    return result[0] || result[1];
  }

private:
  material *src1_;
  material *src2_;
  perlin_noise noise_;
};
