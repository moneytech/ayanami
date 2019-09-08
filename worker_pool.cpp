#include "worker_pool.h"
#include "random.h"

worker_pool::worker_pool(const context           &ctx,
                         size_t                   nworkers,
                         const worker_config     &worker_cfg,
                         std::vector<work_item> &&work_items) :
           ctx_ (ctx),
    work_queue_ (std::move(work_items)) {
  for (size_t w = 0u; w < nworkers; ++w) {
    workers_.emplace_back([this, w, worker_cfg]() {
                            worker_main(worker_cfg);
                          });
  }
}

worker_pool::~worker_pool() {
  for (auto& t : workers_) t.detach();
  workers_.clear();
}

bool worker_pool::finished() const {
  std::unique_lock<std::mutex> lck(work_queue_mu_);
  return nfinished_workers_ == workers_.size();
}

void worker_pool::join() {
  for (auto& t : workers_) t.join();
  nfinished_workers_ = 0u;
  workers_.clear();
}

void worker_pool::worker_main(const worker_config &cfg) {
  set_seed(15677);
  while (1) {
    std::unique_lock<std::mutex> lck(work_queue_mu_);
    if (work_queue_.empty()) {
      nfinished_workers_++;
      return;
    }
    const work_item wi = work_queue_.back();
    work_queue_.pop_back();
    lck.unlock();
    for (size_t r = wi.tile_row; r < wi.tile_row + wi.tile_h; r++) {
      for (size_t c = wi.tile_col; c < wi.tile_col + wi.tile_w; c++) {
        nm::float3 col{ 0.0f, 0.0f, 0.0f };
        for (size_t s = 0u; s < cfg.nsamples; ++s) {
          const float u = ((float)c + randf()) / (float)ctx_.fb.width();
          const float v = ((float)r + randf()) / (float)ctx_.fb.height();
          col = col + ctx_.scn.color(ctx_.scn.cam().get_ray(u, v), 0);
        }
        col /= (float)cfg.nsamples;
        ctx_.fb.set_pixel(r, c,
          255.99f * sqrt(col.x()),
          255.99f * sqrt(col.y()),
          255.99f * sqrt(col.z()));
      }
    }
  }
}
