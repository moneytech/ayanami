#pragma once

#include "texture.h"
#include "perlin.h"

class perlin_texture : public texture {
public:
  nm::float3 sample(const nm::float2 &uv,
                    const sampler    &s) const override {
    const float noiseval = 0.05f * noise_.sample(nm::float3{5.0f * uv, 0.0f}) +
                           2.0f * noise_.sample(nm::float3{10.0f * uv, 0.0f}) +
                           0.05f * noise_.sample(nm::float3{30.0f * uv, 0.0f});
    return nm::float3 { 1.0f, 1.0f, 1.0f } * (0.5f*noiseval + 0.5f);
  }

private:
  perlin_noise noise_;
};
