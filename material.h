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
};
