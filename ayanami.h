#pragma once

#include <stdint.h>
#include <vector>
#include "framebuffer.h"
#include "scene.h"
#include "worker_pool.h"

class ayanami {
public:
  struct render_params {
    uint32_t nworkers,
             ntilesx,
             ntilesy,
             nsamples;
  };

  ayanami(framebuffer         &fb,
          const scene         &scn,
          const render_params &p);

  bool finished() const { return pool_.finished(); }
  void join() { pool_.join(); }

private:
  static std::vector<worker_pool::work_item> create_work_items(uint32_t iw,
                                                               uint32_t ih,
                                                               uint32_t ntx,
                                                               uint32_t nty);
private:
  framebuffer &fb_;
  worker_pool  pool_;
};