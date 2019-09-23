#pragma once

#include "nicemath.h"

class material;

/**
 * Information about a point where a ray intersects geometry.
 */
struct hit_record {
  nm::float3      normal; /* Surface normal at the intersection point. */
  float           t;      /* Parameter value at the intersection.*/
  nm::float3      p;      /* The intersection point itself. */
  const material *mat;    /* Material at the intersection point. */
  nm::float2      uv;     /* Texture coordinates. */
};
