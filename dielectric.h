#pragma once

#include "material.h"
#include "nicemath.h"

/**
 * Models a surface that lets light rays through.
 */
class dielectric : public material {
public:
  explicit dielectric(float ri);

  bool scatter(const ray        &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override;

private:
  static float schlick(float cosine, float ri);
  static bool refract(const nm::float3 &i,
                      const nm::float3 &n,
                      float             ni,
                      float             nt,
                      nm::float3       &refracted);

private:
  float refraction_idx_;
};