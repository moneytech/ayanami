#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <random>
#include "nicemath.h"

/**
 * A two-dimensional array of pixel colors.
 */
class framebuffer {
  static constexpr size_t kBytesPerPixel = 3;
public:

  framebuffer(size_t width,size_t height):
      width_ (width),
      height_(height),
      data_  ((uint8_t*)malloc(width* height* kBytesPerPixel)) {}

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
  ray(const nm::float3 &o,
      const nm::float3 &d) : 
    origin_   (o),
    direction_(nm::normalize(d)) {}

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
    aspect_h_(aspect_h),
    aspect_v_(aspect_v),
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

/**
 * Information about a point where a ray intersects geometry.
 */
struct hit_record {
  nm::float3 normal; /* Surface normal at the intersection point. */
  float      t;      /* Value of t (ray parameter) corresponding to the
                        intersection point.*/
  nm::float3 p;      /* The intersection point itself. */
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
         float radius) :
    center_(center),
    radius_(radius) {}

   bool hit_test(const ray  &r,
                 float       tmin,
                 float       tmax,
                 hit_record &hit) const override {
    const nm::float3 oc = r.origin() - center_;
    const float a = nm::dot(r.direction(), r.direction());
    const float b = nm::dot(oc, r.direction()) * 2.0f;
    const float c = nm::dot(oc, oc) - radius_ * radius_;
    const float d = b * b - 4.0f * a * c;
    if (d > 0.0f) {
      const float t1 = (-b - sqrtf(d)) / (2.0f * a);
      const float t2 = (-b + sqrtf(d)) / (2.0f * a);
      if (t1 > tmin && t1 < tmax) hit.t = t1;
      else if (t2 > tmin && t2 < tmax) hit.t = t2;
      else return false;
      hit.p = r.point_at(hit.t);
      hit.normal = nm::normalize(hit.p - center_);
    }
    return d > 0.0f;
  }

private:
  nm::float3 center_;
  float      radius_;
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
    bool anyhit = false;
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
 * Produces a random floating point number between 0.0 and 1.0.
 */
float randf() {
  static std::random_device rd;
  static std::mt19937  gen(rd());
  static std::uniform_real_distribution<float> d { 0.0f, 1.0 };
  return d(gen);
}

/**
 * Produces a random 3D point within a sphere of radius 1.
 */
nm::float3 random_in_unit_sphere() {
  nm::float3 result;
  do {
    result = 2.0f * nm::float3(randf(), randf(), randf()) -
                    nm::float3 { 1.0f, 1.0f, 1.0f };
  } while (nm::lengthsq(result) > 1.0f);
  return result;
}

/**
 * Cast a ray into the scene and determine color.
 */
nm::float3 color(const ray &r, int bounce) {
  static constexpr int max_bounces = 50;
  sphere_list scene {
    sphere { nm::float3{ 0.0f, 0.0f, -1.0f }, 0.5f },
    sphere { nm::float3{ 0.0f, -100.5f, -1.0f }, 100.0f }
  };
  hit_record hit;
  if (scene.hit_test(r, 0.0f, 1000.0f, hit)) {
    const nm::float3 target = hit.p + hit.normal + random_in_unit_sphere();
    return 0.5f * color(ray { hit.p, target - hit.p }, bounce + 1);
  }
  const float t = 0.5f * (r.direction().y() + 1.0f);
  return (1.0f - t) * nm::float3 { 1.0f, 1.0f, 1.0f } +
                 t  * nm::float3 { 0.5f, 0.7f, 1.0f }; 
}

int main(int argc, char *argv[]) {
  framebuffer fb { 200u, 100u };
  camera      cam { 4.0f, 2.0f };
  for (size_t r = 0u; r < fb.height(); r++) {
    for (size_t c = 0u; c < fb.width(); c++) {
      nm::float3 col { 0.0f, 0.0f, 0.0f };
      constexpr size_t ns = 100u;
      for (size_t s = 0u; s < ns; ++s) {
        const float u = ((float) c + randf()) / (float) fb.width();
        const float v = ((float) r + randf()) / (float) fb.height();
        col = col + color(cam.get_ray(u, v), 0);
      }
      col /= (float)ns;
      fb.set_pixel(r, c,
                   255.99f * sqrt(col.x()),
                   255.99f * sqrt(col.y()),
                   255.99f * sqrt(col.z()));
    }
  }
  fb.save("d:\\image.tga");
  return 0;
}
