#include "checker_texture.h"

nm::float3 checker_texture::sample(const nm::float2       &uv,
                                   const texture::sampler &sampler) const {
  const nm::float2 real_uv = sampler.transform_uvs(uv);
  auto             cellu   = (int)(real_uv[0] * size_),
                   cellv   = (int)(real_uv[1] * size_);
  return colors_[((cellu & 0x1) ^ (cellv & 0x1)) ? 0 : 1];
}