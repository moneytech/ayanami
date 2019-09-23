#pragma once

#include "material.h"
#include "nicemath.h"
#include "texture.h"

/**
 * Models a matte surface that scatters light
 * in the same manner regardless of viewing angle.
 */
class lambertian : public material {
public:
  explicit lambertian(const nm::float3 &albedo);
  explicit lambertian(const texture *tex);

  bool scatter(const ray        &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override;
private:
  nm::float3        albedo_;
  const texture    *texture_;
};
