#pragma once

#include "hitable.h"
#include "nicemath.h"
#include "material.h"

class sphere : public hitable {
public:
  sphere(const nm::float3 &center,
         float             radius,
         const material   *mat);

   bool hit_test(const ray  &r,
                 float       tmin,
                 float       tmax,
                 hit_record &hit) const override;

   std::optional<aabb> bbox() const override;

private:
  nm::float3        center_;
  float             radius_;
  const material   *mat_;
};
