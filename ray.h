#pragma once

#include "nicemath.h"

/**
 * A ray is a combination of an origin point and a direction.
 */
class ray {
public:
  ray() = default;
  ray(const nm::float3 &o,
      const nm::float3 &d) :
       origin_ (o),
    direction_ (nm::normalize(d)) {}

  const nm::float3& origin() const {
    return origin_;
  }

  const nm::float3& direction() const {
    return direction_;
  }

  /**
   * Evaluates A + tB where A is the ray's origin point 
   * and B is the direction.
   */
  nm::float3 point_at(float t) const {
    return origin_ + direction_ * t;
  }

private:
  nm::float3 origin_;
  nm::float3 direction_;
};
