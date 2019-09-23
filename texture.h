#pragma once

#include "nicemath.h"

class texture {
public:
  struct sampler {
    enum class wrap {
      repeat,
      clamp
    };
    nm::float2 transform_uvs(const nm::float2 &uv) const;
    wrap wu, wv;
  };

  virtual nm::float3 sample(const nm::float2 &uv,
                            const sampler    &s) const = 0;
};
