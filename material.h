#pragma once

#include "ray.h"
#include "hit_record.h"
#include "nicemath.h"

class material {
public:
  virtual bool scatter(const ray        &in,
                       const hit_record &rec,
                       nm::float3       &attn,
                       ray              &scattered) const = 0;
  virtual nm::float3 emitted (const nm::float3 &p) const { return nm::float3 { 0.0f, 0.0f, 0.0f }; }
};
