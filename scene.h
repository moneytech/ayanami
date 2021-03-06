#pragma once

#include <stddef.h>
#include <vector>
#include <memory>
#include "lua_env.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "hitable_list.h"
#include "camera.h"
#include "nicemath.h"
#include "ray.h"
#include "bvh_node.h"
#include "noise.h"

class scene {
public:
  struct stats {
    float lua_runtime;
    float bvh_construction;
  };

  scene(lua_env    &lua,
        float       aspect,
        const char *script,
        size_t      script_len,
        bvh_node::cons_strat bvh_cons_strat);

  ~scene();
  
  const camera& cam() const;
  nm::float3 color(const ray &r, int bounce) const;
  void set_camera(float             fov_v,
                  const nm::float3 &look_from,
                  const nm::float3 &look_at,
                  const nm::float3 &upvector,
                  float             aperture);
  void scene::add_material(std::unique_ptr<material>&& mat) {
    mats_.emplace_back(std::move(mat));
  }
  void scene::add_texture(std::unique_ptr<texture>&& tex) {
    texs_.emplace_back(std::move(tex));
  }
  void scene::add_hitable(std::unique_ptr<hitable>&& h) {
    hitables_.push_back(h.release());
  }
  void add_noise(std::unique_ptr<noise> &&n) {
    noises_.emplace_back(std::move(n));
  }
  stats get_stats() const { return stats_; }
  void rebuild_bvh(bvh_node::cons_strat);
  void set_sky_gradient(const nm::float3 &btm,
                        const nm::float3 &top) {
    sky_gradient_[0] = btm;
    sky_gradient_[1] = top;
  }

private:
  std::vector<hitable*>                  hitables_;
  std::unique_ptr<bvh_node>              root_node_;
  camera                                 cam_;
  float                                  aspect_;
  std::vector<std::unique_ptr<material>> mats_;
  std::vector<std::unique_ptr<texture>>  texs_;
  std::vector<std::unique_ptr<noise>>    noises_;
  stats                                  stats_;
  nm::float3                             sky_gradient_[2];
};