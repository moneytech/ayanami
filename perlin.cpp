#include "perlin.h"
#include "nicemath.h"
#include "random.h"
#include <algorithm>

namespace {

float fade(float t) {
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

}

perlin_noise::perlin_noise() {
  for (int g = 0; g < kNumGradients; ++g) {
    gradients_[g] = nm::float3{ randf() * 2.0f - 1.0f,
                                randf() * 2.0f - 1.0f,
                                randf() * 2.0f - 1.0f };
  }
  for (int p = 0; p < 3; ++p) {
    for (int i = 0; i < kNumGradients; ++i) {
      perms_[p][i] = i;
    }
    for (int j = 0; j < kNumGradients; ++j) {
      const int k = j + randf() * (kNumGradients - 1 - j);
      const float tmp = perms_[p][k];
      perms_[p][k] = perms_[p][j];
      perms_[p][j] = tmp;
    }
  }
}


const nm::float3 perlin_noise::grad(const nm::float3 &p) const {
  const int gx = p.x(), gy = p.y(), gz = p.z();
  return gradients_[perms_[0][gx & 0xff] ^ perms_[1][gy & 0xff] ^ perms_[2][gz &0xff]];
}

float perlin_noise::sample(const nm::float3 &p) const {
  /* Calculate lattice points. */
  auto       p0 = nm::float3 { floor(p.x()), floor(p.y()), floor(p.z()) };
  nm::float3 p1 = p0 + nm::float3(1.0, 0.0, 0.0);
  nm::float3 p2 = p0 + nm::float3(0.0, 1.0, 0.0);
  nm::float3 p3 = p0 + nm::float3(1.0, 1.0, 0.0);
  nm::float3 p4 = p0 + nm::float3(0.0, 0.0, 1.0);
  nm::float3 p5 = p4 + nm::float3(1.0, 0.0, 0.0);
  nm::float3 p6 = p4 + nm::float3(0.0, 1.0, 0.0);
  nm::float3 p7 = p4 + nm::float3(1.0, 1.0, 0.0);

  /* Look up gradients at lattice points. */
  nm::float3 g0 = grad(p0);
  nm::float3 g1 = grad(p1);
  nm::float3 g2 = grad(p2);
  nm::float3 g3 = grad(p3);
  nm::float3 g4 = grad(p4);
  nm::float3 g5 = grad(p5);
  nm::float3 g6 = grad(p6);
  nm::float3 g7 = grad(p7);

  float t0 = p.x() - p0.x();
  float fade_t0 = fade(t0); /* Used for interpolation in horizontal direction */

  float t1 = p.y() - p0.y();
  float fade_t1 = fade(t1); /* Used for interpolation in vertical direction. */

  float t2 = p.z() - p0.z();
  float fade_t2 = fade(t2);

  /* Calculate dot products and interpolate.*/
  float p0p1 = (1.0 - fade_t0) * dot(g0, (p - p0)) + fade_t0 * dot(g1, (p - p1)); /* between upper two lattice points */
  float p2p3 = (1.0 - fade_t0) * dot(g2, (p - p2)) + fade_t0 * dot(g3, (p - p3)); /* between lower two lattice points */

  float p4p5 = (1.0 - fade_t0) * dot(g4, (p - p4)) + fade_t0 * dot(g5, (p - p5)); /* between upper two lattice points */
  float p6p7 = (1.0 - fade_t0) * dot(g6, (p - p6)) + fade_t0 * dot(g7, (p - p7)); /* between lower two lattice points */

  float y1 = (1.0 - fade_t1) * p0p1 + fade_t1 * p2p3;
  float y2 = (1.0 - fade_t1) * p4p5 + fade_t1 * p6p7;

  /* Calculate final result */
  return (1.0 - fade_t2) * y1 + fade_t2 * y2;
}
