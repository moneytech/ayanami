#include "lambertian.h"
#include "random.h"

lambertian::lambertian(const nm::float3 &albedo) :
  albedo_(albedo) {}

bool lambertian::scatter(const ray        &in,
                         const hit_record &rec,
                         nm::float3       &attn,
                         ray              &scattered) const {
  const nm::float3 target = rec.p + rec.normal + random_unit_vector();
  attn = albedo_;
  scattered = ray{ rec.p, rec.normal + random_unit_vector() };
  return true;
}
