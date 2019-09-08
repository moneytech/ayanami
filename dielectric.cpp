#include "dielectric.h"

#include "random.h"

dielectric::dielectric(float ri) : refraction_idx_(ri) {}

bool dielectric::scatter(const ray        &in,
                         const hit_record &rec,
                         nm::float3       &attn,
                         ray              &scattered) const {
   bool               is_refracted = false;
   float                    cosine = 0.0f;
   const nm::float3             &n = rec.normal;
   const nm::float3             &i = in.direction();
   float                    refl_p = 1.0f;
   nm::float3        refracted_dir;
   if (nm::dot(i, n) > 0.0f) {
     is_refracted = refract(i, -n, refraction_idx_, 1.0f, refracted_dir);
     cosine = refraction_idx_ * dot(i, n) / nm::length(i);
   } else {
     is_refracted = refract(i, n, 1.0f, refraction_idx_, refracted_dir);
     cosine = -nm::dot(i, n) / nm::length(i);
   }
   if (is_refracted) {
     refl_p = schlick(cosine, refraction_idx_);
   }

   attn      = nm::float3 { 1.0f, 1.0f, 1.0f };
   scattered = ray {
     rec.p,
     randf() < refl_p ? nm::reflect(i, n) : refracted_dir
   };

   return true;
}

float dielectric::schlick(float cosine, float ri) {
  const float r0   = (1.0f - ri) / (1.0f + ri);
  const float r0sq = r0 * r0;
  return r0sq + (1.0f - r0sq) * pow((1.0f - cosine), 5);
}

bool dielectric::refract(const nm::float3 &i,
                         const nm::float3 &n,
                         float             ni,
                         float             nt,
                         nm::float3       &refracted) {
  const nm::float3 ui = nm::normalize(i);
  const float      dt = nm::dot(i, n);
  const float ni_o_nt = ni / nt;
  float             d = 1.0f - ni_o_nt * ni_o_nt * (1.0f - dt * dt);
  const bool  is_refr = d > 0.0f;
  if (is_refr) refracted = ni_o_nt * (ui - n * dt) - n * sqrtf(d);
  return is_refr;
}
