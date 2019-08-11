#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <random>
#include "nicemath.h"

float frand( int *seed )
{
    union
    {
        float fres;
        unsigned int ires;
    };

    seed[0] *= 16807;
    ires = ((((unsigned int)seed[0])>>9 ) | 0x3f800000);
    return fres - 1.0f;
}

/**
 * Produces a random floating point number between 0.0 and 1.0.
 */
float randf() {
  static int seed = 15677;
  return frand(&seed);
}

/**
 * Produces a random 3D point within a sphere of radius 1.
 */
nm::float3 random_in_unit_sphere() {
  nm::float3 result;
  do {
    result = 2.0f * nm::float3 { randf(), randf(), randf() } -
                    nm::float3 {    1.0f,    1.0f,    1.0f };
  } while (nm::lengthsq(result) > 1.0f);
  return result;
}

/**
 * A two-dimensional array of pixel colors.
 */
class framebuffer {
  static constexpr size_t kBytesPerPixel = 3;
public:

  framebuffer(size_t width,size_t height):
       width_ (width),
      height_ (height),
        data_ ((uint8_t*)malloc(width* height* kBytesPerPixel)) {}

  ~framebuffer() { free(data_); }

  /**
   * Set the color of the pixel at the intersection of the given row and
   * column.
   */
  void set_pixel(size_t row,
                 size_t col,
                 uint8_t r,
                 uint8_t g,
                 uint8_t b) {
    const size_t idx = kBytesPerPixel * (row * width_ + col);

    data_[idx + 0] = b;
    data_[idx + 1] = g;
    data_[idx + 2] = r;
  }

  /**
   * Saves the pixel data in uncompressed Targa format.
   */
  void save(const char* file_path) const {
    FILE* fptr = fopen(file_path,"wb");
    assert(fptr);

    // Write targa header, this portion taken from
    // http://paulbourke.net/dataformats/tga/
    putc(0,fptr);
    putc(0,fptr);
    putc(2,fptr);                         /* uncompressed RGB */
    putc(0,fptr); putc(0,fptr);
    putc(0,fptr); putc(0,fptr);
    putc(0,fptr);
    putc(0,fptr); putc(0,fptr);           /* X origin */
    putc(0,fptr); putc(0,fptr);           /* y origin */
    putc((width_ & 0x00FF),fptr);
    putc((width_ & 0xFF00) / 256,fptr);
    putc((height_ & 0x00FF),fptr);
    putc((height_ & 0xFF00) / 256,fptr);
    putc(24,fptr);                        /* 24 bit bitmap */
    putc(0,fptr);

    // Write image data.
    fwrite(data_, kBytesPerPixel, width_ * height_, fptr);
    fclose(fptr);
  }

  size_t width() const { return width_; }
  size_t height() const { return height_; }

private:
  uint8_t* data_;
  size_t   width_;
  size_t   height_;
};

/**
 * A ray is a combination of an origin point and a direction.
 */
class ray {
public:
  ray() = default;
  ray(const nm::float3 &o,
      const nm::float3 &d) : 
       origin_ (o),
    direction_ (nm::normalize(d)) {}

  const nm::float3& origin() const {
    return origin_;
  }

  const nm::float3& direction() const {
    return direction_;
  }

  /**
   * Evaluates A + tB where A is the ray's origin point 
   * and B is the direction.
   */
  nm::float3 point_at(float t) const {
    return origin_ + direction_ * t;
  }

private:
  nm::float3 origin_;
  nm::float3 direction_;
};

/**
 * Produces primary rays.
 */
class camera {
public:
  /**
   * Creates a camera with aspect_h / aspect_v aspect ratio.
   * TODO: add focal length.
   */
  camera(float aspect_h, float aspect_v) :
    aspect_h_ (aspect_h),
    aspect_v_ (aspect_v),
      origin_ { 0.0f, 0.0f, 0.0f } {}

  /**
   * Calculate a ray for the given UV coordinate of the camera image.
   */
  ray get_ray(float u, float v) {
    const nm::float3 lower_left { -aspect_h_ / 2.0f, -aspect_v_/2.0f, -1.0f };
    return ray { origin_,
                 lower_left + nm::float3 {u * aspect_h_, v * aspect_v_, 0.0f}};
  }

private:
  float      aspect_h_;
  float      aspect_v_;
  nm::float3 origin_;
};

class material;

/**
 * Information about a point where a ray intersects geometry.
 */
struct hit_record {
  nm::float3  normal; /* Surface normal at the intersection point. */
  float       t;      /* Value of t (ray parameter) corresponding to the
                        intersection point.*/
  nm::float3  p;      /* The intersection point itself. */
  const material   *mat; /* Material at the intersection point. */
};

class material {
public:
  virtual bool scatter(const ray         &in,
                       const hit_record &rec,
                       nm::float3       &attn,
                       ray              &scattered) const = 0; 
};

class lambertian : public material {
public:
  explicit lambertian(const nm::float3 albedo) :
    albedo_(albedo) {}

  bool scatter(const ray         &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override {
    const nm::float3 target = rec.p + rec.normal + random_in_unit_sphere();
    attn      = albedo_;
    scattered = ray { rec.p, target - rec.p };
    return true;
  }

private:
  nm::float3 albedo_;
};

nm::float3 reflect(const nm::float3 &i, const nm::float3 &n) {
  return i - 2.0f * nm::dot(i, n) * n;
}

class metal : public material {
public:
  metal(const nm::float3 attn, float fuzz) :
    attn_(attn),
    fuzz_(fuzz) {}

  bool scatter(const ray        &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override {
    const nm::float3 refl_dir =
      reflect(nm::normalize(in.direction()), rec.normal) + random_in_unit_sphere() * fuzz_;
    attn      = attn_;
    scattered = ray { rec.p, refl_dir };
    return true;
  }

private:
  nm::float3 attn_;
  float      fuzz_;
};

class dielectric : public material {
public:
  explicit dielectric(float ri) : refraction_idx_(ri) {}

  bool scatter(const ray        &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override {
     attn = nm::float3 { 1.0f, 1.0f, 1.0f };
     nm::float3 refracted_dir;
     bool is_refracted = false;
     const nm::float3 &n = rec.normal;
     const nm::float3 &i = in.direction();
     float refl_prob = 1.0f;
     float cosine = 0.0f;
     if (nm::dot(i, n) > 0.0f) {
       is_refracted = refract(i, -n, refraction_idx_, 1.0f, refracted_dir);
       cosine = refraction_idx_ * dot(i, n) / nm::length(i);
     } else {
       is_refracted = refract(i, n, 1.0f, refraction_idx_, refracted_dir);
       cosine = -nm::dot(i, n) / nm::length(i);
     }
     if (is_refracted) {
       refl_prob = schlick(cosine, refraction_idx_);
     }

     scattered = ray {
       rec.p,
       randf() < refl_prob ? reflect(i, n) : refracted_dir
     };

     return true;
  }

private:
  static float schlick(float cosine, float ri) {
    const float r0   = (1.0f - ri) / (1.0f + ri);
    const float r0sq = r0 * r0;
    return r0sq + (1.0f - r0sq) * pow((1.0f - cosine), 5);
  }

  static bool refract(const nm::float3 &i,
                      const nm::float3 &n,
                      float             ni,
                      float             nt,
                      nm::float3       &refracted) {
    const nm::float3 ui = nm::normalize(i);
    const float      dt = nm::dot(i, n);
    const float ni_o_nt = ni / nt;
    float             d = 1.0f - ni_o_nt * ni_o_nt * (1.0f - dt * dt);
    const bool  is_refr = d > 0.0f;
    if (is_refr)
      refracted = ni_o_nt * (ui - n * dt) - n * sqrtf(d);
    return is_refr;
  }

  float refraction_idx_;
};


/**
 * Any geometry that may be intersected by rays.
 */
class hitable {
public:
  /**
   * Test if the given ray hits this piece of geometry.
   * `tmin' and `tmax' are the minimum and maximum allowed values of t at which
   * the intersection is allowed to occur.
   * `hit' shall contain information about the ray/geometry intersection if an
   *  an intersection is present.
   * This function shall return `true' if the ray intersects with the geometry
   * and `false' otherwise.
   */
  virtual bool hit_test(const ray  &r,
                        float       tmin,
                        float       tmax,
                        hit_record &hit) const = 0;
};

class sphere : public hitable {
public:
  sphere(const nm::float3 &center,
         float             radius,
         const material   *mat) :
    center_(center),
    radius_(radius),
    mat_   (mat) {}

   bool hit_test(const ray  &r,
                 float       tmin,
                 float       tmax,
                 hit_record &hit) const override {
    const nm::float3 oc = r.origin() - center_;
    const float       a = nm::dot(r.direction(), r.direction());
    const float       b = nm::dot(oc, r.direction()) * 2.0f;
    const float       c = nm::dot(oc, oc) - radius_ * radius_;
    const float       d = b * b - 4.0f * a * c;

    if (d > 0.0f) {
      const float t1 = (-b - sqrtf(d)) / (2.0f * a);
      const float t2 = (-b + sqrtf(d)) / (2.0f * a);
      if (t1 > tmin && t1 < tmax) hit.t = t1;
      else if (t2 > tmin && t2 < tmax) hit.t = t2;
      else return false; /* neither of the roots are within acceptable range. */
      hit.p      = r.point_at(hit.t);
      hit.normal = (hit.p - center_) / radius_;
      hit.mat    = mat_;
    }

    return d > 0.0f;
  }

private:
  nm::float3        center_;
  float             radius_;
  const material   *mat_;
};

/**
 * List of spheres.
 */
class sphere_list : hitable {
public:
  template <class ...Args>
  sphere_list(Args... args) : list_ { std::forward<Args>(args)... } {}

  /**
   * A hit is registerted if the ray intersects any of the spheres in the list.
   */
  bool hit_test(const ray  &r,
                float       tmin,
                float       tmax,
                hit_record &hit) const override {
    bool       anyhit = false;
    float closest_hit = tmax;

    for (const sphere &s : list_) {
      bool has_hit = s.hit_test(r, tmin, closest_hit, hit);
      if (has_hit) closest_hit = hit.t; /* ensure we get the closest hit. */
      anyhit |= has_hit;
    }
    return anyhit;
  }

private:
  std::vector<sphere> list_;
};

/**
 * Cast a ray into the scene and determine color.
 */
nm::float3 color(const ray &r, int bounce) {
  static constexpr int max_bounces = 50;
  static lambertian diffuse_gray { nm::float3 { 0.5f, 0.5f, 0.5f } };
  static lambertian diffuse_pink { nm::float3 { 0.8f, 0.3f, 0.3f } };
  static lambertian diffuse_yellow { nm::float3 { 0.8f, 0.8f, 0.0f }};
  static lambertian diffuse_blue  { nm::float3 { 0.1f, 0.2f, 0.5f } };
  static dielectric dielectric_1  { 1.5f };
  static metal      reddish_metal { nm::float3 {0.8f, 0.6f, 0.2f },  0.0f };
  static metal      gray_metal    { nm::float3 { 0.8f, 0.8f, 0.8f }, 0.3f };

  static sphere_list scene {
    sphere { nm::float3{ 0.0f,    0.0f, -1.0f }, 0.5f, &diffuse_blue},
    sphere { nm::float3{ 0.0f, -100.5f, -1.0f }, 100.0f, &diffuse_yellow },
    sphere { nm::float3{1.0f, 0.0f, -1.0f}, 0.5f, &reddish_metal },
    sphere { nm::float3{-1.0f, 0.0f, -1.0f}, 0.5f, &dielectric_1 },
    sphere { nm::float3{-1.0f, 0.0f, -1.0f},-0.45f, &dielectric_1 },
  };
  hit_record hit;
  if (bounce < max_bounces && scene.hit_test(r, 0.001f, 1000.0f, hit)) {
    const nm::float3 target = hit.p + hit.normal + random_in_unit_sphere();
    nm::float3 attn;
    ray        scattered;
    if (hit.mat->scatter(r, hit, attn, scattered)) {
      return attn * color(scattered, bounce + 1);
    }
  }
  const float t = 0.5f * (r.direction().y() + 1.0f);
  return (1.0f - t) * nm::float3 { 1.0f, 1.0f, 1.0f } +
                 t  * nm::float3 { 0.5f, 0.7f, 1.0f }; 
}

int main(int argc, char *argv[]) {
  constexpr size_t kNumSamples = 100u;

  framebuffer  fb { 400u, 200u };
  camera      cam { 4.0f, 2.0f };

  for (size_t r = 0u; r < fb.height(); r++) {
    for (size_t c = 0u; c < fb.width(); c++) {
      nm::float3 col { 0.0f, 0.0f, 0.0f };
      for (size_t s = 0u; s < kNumSamples; ++s) {
        const float u = ((float) c + randf()) / (float) fb.width();
        const float v = ((float) r + randf()) / (float) fb.height();
        col = col + color(cam.get_ray(u, v), 0);
      }
      col /= (float)kNumSamples;
      fb.set_pixel(r, c,
                   255.99f * sqrt(col.x()),
                   255.99f * sqrt(col.y()),
                   255.99f * sqrt(col.z()));
    }
  }
  fb.save("d:\\image.tga");
  return 0;
}
