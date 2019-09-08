#include "metal.h"

#include "random.h"

metal::metal(const nm::float3 attn, float fuzz) :
  attn_(attn),
  fuzz_(fuzz) {}

bool metal::scatter(const ray        &in,
                    const hit_record &rec,
                    nm::float3       &attn,
                    ray              &scattered) const {
  const nm::float3 refl_dir =
    nm::reflect(nm::normalize(in.direction()), rec.normal) +
    random_unit_vector() * fuzz_;

  attn      = attn_;
  scattered = ray { rec.p, refl_dir };

  return true;
}

