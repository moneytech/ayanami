#include "ayanami.h"

ayanami::ayanami(framebuffer         &fb,
                 const scene         &scn,
                 const render_params &p) :
    fb_   { fb },
    pool_ {
      worker_pool::context { fb_, scn },
      p.nworkers,
      worker_pool::worker_config { p.nsamples },
      create_work_items(fb_.width(),
                        fb_.height(),
                        p.ntilesx,
                        p.ntilesy)
     } {}

std::vector<worker_pool::work_item> ayanami::create_work_items(uint32_t iw,
                                                               uint32_t ih,
                                                               uint32_t ntx,
                                                               uint32_t nty) {
  const uint32_t tile_w = iw / ntx;
  const uint32_t tile_h = ih / nty;
  const uint32_t edge_tile_slack_h = iw % ntx;
  const uint32_t edge_tile_slack_v = ih % nty;
  std::vector<worker_pool::work_item> work_items;
  for (uint32_t tx = 0; tx < ntx; tx++) {
    for (uint32_t ty = 0; ty < nty; ty++) {
      work_items.emplace_back(
          worker_pool::work_item{
            ty * tile_h,
            tx * tile_w,
            tile_w + (tx == ntx - 1u ? edge_tile_slack_h : 0u),
            tile_h + (ty == nty - 1u ? edge_tile_slack_v : 0u)
          });
    }
  }
  return work_items;
}

