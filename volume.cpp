#include "volume.h"
#include "random.h"
#include "material.h"


volume::volume(std::unique_ptr<hitable> boundary,
               float                    extinction_factor,
               const nm::float3        &albedo) :
    boundary_          { std::move(boundary) },
    extinction_factor_ { extinction_factor },
    mat_               { albedo } { }

bool volume::hit_test(const ray  &r,
                      float       tmin,
                      float       tmax,
                      hit_record &hit) const {
  // find the points where the ray enters and exits the volume boundary.
  // note that we're not using tmin and tmax here - we want to find
  // the entry point even if it's behind the ray origin (for rays
  // originating inside the volume).
  hit_record bound_hits[2];
  if(boundary_->hit_test(r,
                         std::numeric_limits<float>::lowest(),
                         std::numeric_limits<float>::max(),
                         bound_hits[0]) &&
     boundary_->hit_test(r,
                         bound_hits[0].t + 0.0001f,
                         std::numeric_limits<float>::max(),
                         bound_hits[1])) {
    // clamp t of entry/exit points.
    if(bound_hits[0].t < tmin) bound_hits[0].t = tmin;
    if(bound_hits[0].t < .0f) bound_hits[0].t = .0f;
    if(bound_hits[1].t > tmax) bound_hits[1].t = tmax;
 
    // shouldn't happen?
    if(bound_hits[0].t > bound_hits[1].t) { return false; }

    // compute the distance traveled by the ray.
    const float d = bound_hits[1].t - bound_hits[0].t;

    /**
     * Explanation for the code below.
     *
     * T - transmittance, probability of a ray making it through the volume
     *     without being extinguished.
     * tau - optical thickness, obtained by integrating the extinction
             coefficient over the path traveled by the ray. for constant
             extinction coefficient s, tau = d * s where d is the distance
             traveled.
     * In this model, T = e ^ (-tau) = e ^ (-d * s).
     * This code generates a uniformly distributed random value p and compares
     * it to T. If p < T then the ray makes it through and there is no hit.
     * Otherwise, there is a hit.
     * So, a hit is detected when:
     *   p > T = e ^ (-d * s),
     * which is the same as saying:
     *  ln(p) > -d * s 
     *  -ln(p)/s < d
     * Note that (-ln(p)/s) gives us the distance at which the extinction event
     * occurs. 
     * Source for all of the above:
     * https://cs.dartmouth.edu/~wjarosz/publications/dissertation/chapter4.pdf
     */
    const float p = randf();
    const float s = extinction_factor_;
    const float u = -log(p)/s;
    if (u < d) {
      hit.t   = bound_hits[0].t + u;
      hit.p   = r.point_at(hit.t);
      hit.mat = &mat_;
      return true;
    }
  }
  return false;
}

std::optional<aabb> volume::bbox() const { return boundary_->bbox(); }


