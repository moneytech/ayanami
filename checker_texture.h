#pragma once

#include "texture.h"

class checker_texture : public texture {
public:
  checker_texture(const nm::float3 &col1,
                  const nm::float3 &col2,
                  int               size) : colors_ { col1, col2 },
                                            size_   { size } {}

  nm::float3 sample(const nm::float2 &uv,
                    const sampler    &s) const override;

private:
  const nm::float3 colors_[2];
  int              size_;
};
