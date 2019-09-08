#include "bvh_node.h"
#include "random.h"
  #include <assert.h>
#include <algorithm>
#include <stdexcept>

bvh_node::bvh_node(hitable **list, int n, bvh_node::cons_strat strat) {
  if (n == 1) {
    left_child_ = list[0];
    right_child_ = list[0];
  } else if (n == 2) {
    left_child_  = list[0];
    right_child_ = list[1];
  } else {
    switch(strat) {
    case cons_strat::RandomAxis:
      construct_random_axis(list, n); 
      break;
    case cons_strat::MaxUniqueElements:
      construct_max_unique_elements(list, n);
      break;
    default:
      assert(false);
    }
  }
  bbox_ = aabb::surround(left_child()->bbox().value(), right_child()->bbox().value());
}

void bvh_node::construct_random_axis(hitable **list, int n) {
  const int axis = 3.0f * randf();
  auto box_compare = [axis](const hitable* h0, const hitable* h1) {
    return h0->bbox().value().slab(axis)[0] < h1->bbox().value().slab(axis)[0];
  };
  std::sort(list, list + n, box_compare);
  left_child_ = std::make_unique<bvh_node>(list, n / 2, cons_strat::RandomAxis);
  right_child_= std::make_unique<bvh_node>(list + n / 2, n - n / 2, cons_strat::RandomAxis);
}

void bvh_node::construct_max_unique_elements(hitable **list, int n) {
  int max_i = 0;
  for (int axis = 0; axis < 3; ++axis) {
    auto box_compare = [axis](const hitable* h0, const hitable* h1) {
      return h0->bbox().value().slab(axis)[0] < h1->bbox().value().slab(axis)[0];
    };
    std::sort(list, list + n, box_compare);
    auto left_candidate = std::make_unique<bvh_node>(list, n / 2, cons_strat::MaxUniqueElements);
    auto right_candidate = std::make_unique<bvh_node>(list + n / 2, n - n / 2, cons_strat::MaxUniqueElements);
    int i = 0;
    const aabb &left_bbox = left_candidate->bbox().value();
    const aabb &right_bbox = right_candidate->bbox().value();
    for (int j = 0; j < n; ++j) {
      const aabb &bbox = list[j]->bbox().value();
      if ((j < n / 2 && !right_bbox.contains(bbox)) ||
          (j >= n / 2 && !left_bbox.contains(bbox))) {
        ++i;
      }
    } 
    if (i > max_i) {
      left_child_ = std::move(left_candidate);
      right_child_ = std::move(right_candidate);
      max_i = i;
    }
  }
}

bool bvh_node::hit_test(const ray  &r,
                        float       tmin,
                        float       tmax,
                        hit_record &hit) const {
  if (bbox_.test_ray(r, tmin, tmax)) {
    hit_record hrl, hrr;
    const bool hit_left  = left_child()->hit_test(r, tmin, tmax, hrl),
               hit_right = right_child()->hit_test(r, tmin, tmax, hrr);
    if (hit_left && hit_right) {
      hit = hrl.t < hrr.t ? hrl : hrr;
    } else if (hit_left) {
      hit = hrl;
    } else if (hit_right) {
      hit = hrr;
    }
    return hit_left || hit_right;
  } else {
    return false;
  }
}

std::optional<aabb> bvh_node::bbox() const {
  return bbox_;
}
