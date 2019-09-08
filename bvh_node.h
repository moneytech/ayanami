#pragma once

#include "hitable.h"
#include <memory>
#include <variant>

/**
 * A node in a bounding volume hierarchy.
 */
class bvh_node : public hitable {
public:
  enum class cons_strat {
    MaxUniqueElements,
    RandomAxis
  };
  bvh_node(hitable **list, int nhitables, cons_strat strat);

  bool hit_test(const ray  &r,
                float       tmin,
                float       tmax,
                hit_record &hit) const override;

  std::optional<aabb> bbox() const override;

  const hitable* left_child() const { return get_child(left_child_); }
  const hitable* right_child() const { return get_child(right_child_); }

private:
  void construct_random_axis(hitable **list, int nhitables);
  void construct_max_unique_elements(hitable **list, int nhitables);

  using owned_subtree = std::unique_ptr<bvh_node>;
  using subtree = std::variant<owned_subtree, hitable*>;

  const hitable* get_child(const subtree &st) const {
    if (auto pval = std::get_if<owned_subtree>(&st)) {
      return pval->get();
    } else if (auto pval = std::get_if<hitable*>(&st)) {
      return *pval;
    } else {
      return nullptr;
    }
  }
  subtree left_child_    = nullptr,
          right_child_   = nullptr;
  aabb    bbox_;
};