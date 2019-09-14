#include "aa_rect.h"

#include <assert.h>
#include <tuple>

aa_rect::aa_rect(aa_rect::axes     a,
                 const nm::float2 &a1b,
                 const nm::float2 &a2b,
                 float             a3d,
                 const material   &mat,
                 bool              face) : axes_   { a },
                                           bounds_ { a1b, a2b },
                                           dist_   { a3d },
                                           mat_    { mat },
                                           face_   { face } {}

bool aa_rect::hit_test(const ray  &r,
                       float       tmin,
                       float       tmax,
                       hit_record &hit) const {
  const auto params = [this, r](){
    switch(axes_) {
    case axes::xy: {
      const float t = (dist_ - r.origin().z()) / r.direction().z();
      const float x = r.origin().x() + t * r.direction().x();
      const float y = r.origin().y() + t * r.direction().y();
      const nm::float3 n { 0.0f, 0.0f, 1.0f };
      return std::make_tuple(t, x, y, n);
    }
    case axes::xz: {
      const float t = (dist_ - r.origin().y()) / r.direction().y();
      const float x = r.origin().x() + t * r.direction().x();
      const float z = r.origin().z() + t * r.direction().z();
      const nm::float3 n { 0.0f, 1.0f, 0.0f };
      return std::make_tuple(t, x, z, n);
    }
    case axes::yz: {
      const float t = (dist_ - r.origin().x()) / r.direction().x();
      const float y = r.origin().y() + t * r.direction().y();
      const float z = r.origin().z() + t * r.direction().z();
      const nm::float3 n { 1.0f, 0.0f, 0.0f };
      return std::make_tuple(t, y, z, n);
    }
    default: assert(false);
    }
  }();
  const float      t  = std::get<0>(params),
                   a1 = std::get<1>(params),
                   a2 = std::get<2>(params);
  const nm::float3 n  = std::get<3>(params);
  if (t > tmax || t < tmin ||
      a1 > bounds_[0][1] || a1 < bounds_[0][0] ||
      a2 > bounds_[1][1] || a2 < bounds_[1][0]) return false;
  hit.mat    = &mat_;
  hit.p      = r.point_at(t);
  hit.normal = (face_ ? 1.0f : -1.0f) * n;
  hit.t      = t;
  return true;
} 

std::optional<aabb> aa_rect::bbox() const {
  switch (axes_) {
  case axes::xy:
    return aabb { bounds_[0],
                  bounds_[1],
                  nm::float2 { dist_ - 0.1f, dist_ + 0.1f } };
  case axes::yz:
    return aabb { nm::float2 { dist_ - 0.1f, dist_ + 0.1f },
                  bounds_[0],
                  bounds_[1]
                };

  case axes::xz:
    return aabb { bounds_[0],
                  nm::float2 { dist_ - 0.1f, dist_ + 0.1f },
                  bounds_[1]
                };
  default: assert(false);
  }
  return aabb();
}


  