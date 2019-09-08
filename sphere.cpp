#include "sphere.h"
 
sphere::sphere(const nm::float3 &center,
               float             radius,
               const material   *mat) :
    center_(center),
    radius_(radius),
    mat_   (mat) {}

bool sphere::hit_test(const ray  &r,
                      float       tmin,
                      float       tmax,
                      hit_record &hit) const {
  const nm::float3 oc = r.origin() - center_;
  const float       a = nm::dot(r.direction(), r.direction());
  const float       b = nm::dot(oc, r.direction()) * 2.0f;
  const float       c = nm::dot(oc, oc) - radius_ * radius_;
  const float       d = b * b - 4.0f * a * c;

  if (d > 0.0f) {
    const float t1 = (-b - sqrtf(d)) / (2.0f * a);
    const float t2 = (-b + sqrtf(d)) / (2.0f * a);
    if (t1 > tmin && t1 < tmax) hit.t = t1;
    else if (t2 > tmin && t2 < tmax) hit.t = t2;
    else return false; /* neither of the roots are within acceptable range. */
    hit.p = r.point_at(hit.t);
    hit.normal = (hit.p - center_) / radius_;
    hit.mat = mat_;
  }

  return d > 0.0f;
}

std::optional<aabb> sphere::bbox() const {
  return aabb(center_ - nm::float3(radius_, radius_, radius_),
              center_ + nm::float3(radius_, radius_, radius_));
}
