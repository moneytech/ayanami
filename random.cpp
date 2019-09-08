#include "random.h"
#include <time.h>

static thread_local int seed = 15677; // time(nullptr);

void set_seed(int s) { seed = s; }

/**
* Generates a random float in 0.0 to 1.0 range.
* (credit to Inigo Quilez:
* http://www.iquilezles.org/www/articles/sfrand/sfrand.htm)
*/
float randf()
{
  union
  {
    float fres;
    unsigned int ires;
  };

  seed *= 16807;
  ires = ((((unsigned int)seed)>>9 ) | 0x3f800000);
  return fres - 1.0f;
}

nm::float3 random_unit_vector() {
    float z = randf() * 2.0f - 1.0f;
    float a = randf() * 2.0f * 3.1415926f;
    float r = sqrtf(1.0f - z * z);
    float x = r * cosf(a);
    float y = r * sinf(a);
    return nm::float3(x, y, z);
}

nm::float2 random_in_unit_disk() {
  nm::float2 result;
  do {
    result = 2.0f * nm::float2 { randf(), randf() } -
                    nm::float2 {    1.0f,    1.0f };
  } while (nm::lengthsq(result) > 1.0f);
  return result;
}


