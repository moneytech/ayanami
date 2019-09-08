#pragma once

#include "aabb.h"
#include "ray.h"
#include "hit_record.h"
#include <optional>

/**
 * Any geometry that may be intersected by rays.
 */
class hitable {
public:
  /**
   * Test if the given ray hits this piece of geometry.
   * `tmin' and `tmax' are the minimum and maximum allowed values of t at which
   * the intersection is allowed to occur.
   * `hit' shall contain information about the ray/geometry intersection if an
   *  an intersection is present.
   * This function shall return `true' if the ray intersects with the geometry
   * and `false' otherwise.
   */
  virtual bool hit_test(const ray  &r,
                        float       tmin,
                        float       tmax,
                        hit_record &hit) const = 0;

  /**
   * Obtain the object's surrounding aabb, if it has one.
   */
  virtual std::optional<aabb> bbox() const { return std::nullopt; }
};
