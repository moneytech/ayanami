#include "lambertian.h"
#include "random.h"

lambertian::lambertian(const nm::float3 &albedo) :
  albedo_(albedo), texture_(nullptr) {}

lambertian::lambertian(const texture *tex) : albedo_ { 0.0f, 0.0f, 0.0f },
                                             texture_ { tex } {}

bool lambertian::scatter(const ray        &in,
                         const hit_record &rec,
                         nm::float3       &attn,
                         ray              &scattered) const {
  const nm::float3 target = rec.p + rec.normal + random_unit_vector();
  if (texture_) attn = texture_->sample(rec.uv, texture::sampler { texture::sampler::wrap::repeat,
                                                                   texture::sampler::wrap::repeat });
  else attn = albedo_;
  scattered = ray{ rec.p, rec.normal + random_unit_vector() };
  return true;
}
