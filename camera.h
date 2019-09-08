#pragma once

#include "nicemath.h"
#include "ray.h"

/**
 * Produces primary rays.
 */
class camera {
  static constexpr float Pi = 3.1415926f;
public:
  /**
   * Create a new camera with given parameters.
   */
  camera(float             fov_v,
         float             aspect,               
         const nm::float3 &look_from,
         const nm::float3 &look_at,
         const nm::float3 &upvector,
         float             aperture);
  /**
   * Calculate a ray for the given UV coordinate of the camera image.
   */
  ray get_ray(float u, float v) const;

private:
  nm::float3 hvector_,
             vvector_,
             lower_left_,
             origin_,
             camz_,
             camx_,
             camy_;
  float      lens_radius_;
};
