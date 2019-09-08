#include "hitable_list.h"

void hitable_list::append(std::unique_ptr<hitable> &&h) {
  list_.emplace_back(std::move(h));
}

bool hitable_list::hit_test(const ray  &r,
                            float       tmin,
                            float       tmax,
                            hit_record &hit) const {
  bool       anyhit = false;
  float closest_hit = tmax;

  for (const auto &s : list_) {
    bool has_hit = s->hit_test(r, tmin, closest_hit, hit);
    if (has_hit) closest_hit = hit.t; /* ensure we get the closest hit. */
    anyhit |= has_hit;
  }
  return anyhit;
}
