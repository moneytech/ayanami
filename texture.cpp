#include "texture.h"
#include <math.h>

nm::float2 texture::sampler::transform_uvs(const nm::float2 &uv) const {
  static const auto transform_coord = [](float c, wrap w) {
    double dummy;
    switch(w) {
    case wrap::repeat:
      if (c > 1.0f) return (float)modf(c - 1.0, &dummy);
      else if (c < 0.0f) return (float)(1.0f + modf(c, &dummy));
      else return c;
    case wrap::clamp:
      return (float)fmin(1.0f, fmax(0.0f, c));
    }
  };
  return nm::float2 { transform_coord(uv[0], wu),
                      transform_coord(uv[1], wv) };
}
