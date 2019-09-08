#pragma once

#include <atomic>
#include <stdint.h>
#include <vector>
#include <thread>
#include <mutex>
#include "framebuffer.h"
#include "scene.h"

class worker_pool {
public:
  struct worker_config {
    uint32_t nsamples;
  };

  struct work_item {
    size_t tile_row,
           tile_col,
           tile_w,
           tile_h;
  };

  struct context {
    framebuffer  &fb;
    const scene  &scn;
  };

  worker_pool(const context           &ctx,
              size_t                   nworkers,
              const worker_config     &worker_cfg,
              std::vector<work_item> &&work_items);

  ~worker_pool();

  bool finished() const;

  void join();

private:
  void worker_main(const worker_config &cfg);
 
  context                  ctx_;
  std::vector<work_item>   work_queue_;
  mutable std::mutex       work_queue_mu_;
  std::vector<std::thread> workers_;
  std::atomic<uint32_t>    nfinished_workers_ = 0u;
};
