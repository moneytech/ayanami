#pragma once

#include "material.h"
#include "random.h"

class simple_light : public material {
public:
  explicit simple_light(const nm::float3 &color) : color_ { color } {}

  bool scatter(const ray        &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override { 
    scattered = ray{ rec.p, rec.normal + random_unit_vector() };
    return false;
  }

  nm::float3 emitted (const nm::float3 &p) const override{ return color_; }

private:
  nm::float3 color_;
};