#pragma once

#include "hitable.h"
#include "material.h"
#include "random.h"
#include <memory>

/**
 * A bounded volume of participating medium of constant density.
 */
class volume : public hitable {
public:
  volume(std::unique_ptr<hitable>  boundary,
         float                     extinction_factor,
         const nm::float3         &albedo);

  /**
   * Since the volume has no "surface" per se, the hit test
   * indicates whether the ray was scattered/absorbed (positive)
   * or went through (negative).
   */
  bool hit_test(const ray  &r,
                float       tmin,
                float       tmax,
                hit_record &hit) const override;

  std::optional<aabb> bbox() const override;

private:
  /**
   * Scatters light uniformly.
   */
  class isotropic : public material {
  public:
    explicit isotropic(const nm::float3 &alb) : albedo_(alb) {}

    bool scatter(const ray        &in,
                 const hit_record &rec,
                 nm::float3       &attn,
                 ray              &scattered) const override {
      attn = albedo_;
      scattered = ray { rec.p, random_unit_vector() };
      return true;
    }

  private:
    nm::float3 albedo_;
  };

  std::unique_ptr<hitable> boundary_;
  float                    extinction_factor_;
  isotropic                mat_;
};
