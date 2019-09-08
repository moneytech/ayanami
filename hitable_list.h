#pragma once

#include <memory>
#include <vector>
#include "hitable.h"

/**
 * List of hitable objects.
 */
class hitable_list : hitable {
public:

  /**
   * Adds another hitable to the list.
   */
  void append(std::unique_ptr<hitable> &&h);

  /**
   * A hit is registerted if the ray intersects any of the objects in the list.
   */
  bool hit_test(const ray  &r,
                float       tmin,
                float       tmax,
                hit_record &hit) const override;

private:
  std::vector<std::unique_ptr<hitable>> list_;
};
