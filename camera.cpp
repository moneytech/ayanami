#include "camera.h"

#include "random.h"

camera::camera(float             fov_v,
               float             aspect,               
               const nm::float3 &look_from,
               const nm::float3 &look_at,
               const nm::float3 &upvector,
               float             aperture) :
         origin_ (look_from),
    lens_radius_ (aperture / 2.0f),
           camz_ { nm::normalize(look_from - look_at) },
           camx_ { nm::normalize(nm::cross(upvector, camz_)) },
           camy_ { nm::normalize(nm::cross(camz_, camx_)) } {
  const float fov_v_rad = fov_v * Pi / 180.0f;
  const float half_height = tan(fov_v_rad / 2.0f);
  const float half_width = half_height * aspect;
  const float focus_dist = nm::length(look_from - look_at);

  lower_left_ = origin_ - half_width * focus_dist * camx_
    - half_height * focus_dist * camy_
    - focus_dist * camz_;
  hvector_ = camx_ * 2.0f * focus_dist * half_width;
  vvector_ = camy_ * 2.0f * focus_dist * half_height;
}

ray camera::get_ray(float u, float v) const {
  const nm::float2 rd = random_in_unit_disk() * lens_radius_;
  const nm::float3 ray_o = origin_ + camx_ * rd.x() + camy_ * rd.y();
  return ray{ ray_o,
    lower_left_ + u * hvector_ + v * vvector_ - ray_o };
}
