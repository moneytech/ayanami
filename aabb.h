#pragma once

#include "nicemath.h"
#include "ray.h"
#include <math.h>

/**
 * Axis-aligned bounding box.
 */
class aabb {
public:
  aabb() = default;

  /**
   * Construct a new aabb from bottom left and top right points.
   */
  aabb(const nm::float3 &min,
       const nm::float3 &max) : aabb(nm::float2(min.x(), max.x()),
                                     nm::float2(min.y(), max.y()),
                                     nm::float2(min.z(), max.z())) {}

  /**
   * Construct a new aabb from slabs.
   */
  aabb(const nm::float2 &xslab,
       const nm::float2 &yslab,
       const nm::float2 &zslab) : slabs_ { xslab,
                                           yslab,
                                           zslab } {}


  /**
   * Tests a ray for an intersection with the bounding box -
   * tmin and tmax are the min and max allowed ray parameter values
   * respectively.
   */
  bool test_ray(const ray &r,
                float      tmin,
                float      tmax) const {
    for(int axis = 0; axis < 3 && tmax > tmin; ++axis) {
      const float inv = 1.0f / r.direction()[axis];
      const bool  swp = inv < 0.0f;  
      const float a   = (slabs_[axis][0] - r.origin()[axis]) * inv,
                  b   = (slabs_[axis][1] - r.origin()[axis]) * inv,
                  t0  = swp ? b : a,
                  t1  = swp ? a : b;
      tmin = t0 > tmin ? t0 : tmin;
      tmax = t1 < tmin ? t1 : tmax;
    }
    return tmin < tmax;
  }

  const nm::float2& xslab() const { return slabs_[0]; }
  const nm::float2& yslab() const { return slabs_[1]; }
  const nm::float2& zslab() const { return slabs_[2]; }
  const nm::float2& slab(int s) const { return slabs_[s]; }
  float volume() const { return (xslab()[1] - xslab()[0]) *
                                (yslab()[1] - yslab()[0]) *
                                (zslab()[1] - zslab()[0]); }

  /**
   * Compute an aabb surrounding the two given aabbs.
   */
  static aabb surround(const aabb &b0, const aabb &b1) {
    return aabb(
      nm::float2 { fmin(b0.xslab()[0], b1.xslab()[0]), fmax(b0.xslab()[1], b1.xslab()[1]) },
      nm::float2 { fmin(b0.yslab()[0], b1.yslab()[0]), fmax(b0.yslab()[1], b1.yslab()[1]) },
      nm::float2 { fmin(b0.zslab()[0], b1.zslab()[0]), fmax(b0.zslab()[1], b1.zslab()[1]) });
  }

  static float intersect_vol(const aabb &b0, const aabb &b1) {
    auto side_len = [](const nm::float2 &s0, const nm::float2 & s1) {
      nm::float2 i { fmax(s0[0], s1[0]), fmin(s0[1], s1[1]) };
      return i[0] < i[1] ? i[1] - i[0] : 0.0f;
    };
    return side_len(b0.xslab(), b1.xslab()) *
           side_len(b0.yslab(), b1.yslab()) *
           side_len(b0.zslab(), b1.zslab());
  }

  bool contains(const aabb &other) const {
    static const auto interval_contains =
      [](const nm::float2 &a, const nm::float2 &b) {
        return a[0] < b[0] && b[1] < a[1];
      };
    return interval_contains(slabs_[0], other.slabs_[0]) &&
           interval_contains(slabs_[1], other.slabs_[1]) &&
           interval_contains(slabs_[2], other.slabs_[2]);
  }

private:
  nm::float2 slabs_[3];
};
