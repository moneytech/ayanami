#pragma once

#include "hitable.h"
#include "nicemath.h"

class aa_rect : public hitable {
public:
  enum class axes {
    xy, yz, xz
  };

  aa_rect(axes a,
          const nm::float2 &a1b,
          const nm::float2 &a2b,
          float             a3d,
          const material   &mat,
          bool              face);

  bool hit_test(const ray  &r,
                 float       tmin,
                 float       tmax,
                 hit_record &hit) const override;

   std::optional<aabb> bbox() const override;

private:
  axes             axes_;
  nm::float2       bounds_[2];
  float            dist_;
  const material  &mat_;
  bool             face_;
};