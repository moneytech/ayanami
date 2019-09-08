#pragma once

#include <stdint.h>
#include <utility>
/**
 * A two-dimensional array of pixel colors.
 */
class framebuffer {
  static constexpr uint32_t kBytesPerPixel = 3;
public:
  framebuffer(uint32_t width, uint32_t height);
  framebuffer(const framebuffer&) = delete;
  framebuffer(framebuffer &&other) noexcept { *this = std::move(other); }
  ~framebuffer();
  framebuffer& operator=(const framebuffer&) = delete;
  framebuffer& operator=(framebuffer &&other) noexcept;

  /**
   * Set the color of the pixel at the intersection of the given row and
   * column.
   */
  void set_pixel(uint32_t row,
                 uint32_t col,
                 uint8_t  r,
                 uint8_t  g,
                 uint8_t  b);

  /**
   * Saves the pixel data in uncompressed Targa format.
   */
  void save(const char* file_path) const;

  /**
   * Width of framebuffer in pixels.
   */
  uint32_t width() const { return width_; }

  /**
   * Height of framebuffer in pixels.
   */
  uint32_t height() const { return height_; }

  /**
   * Pointer to RGB image data.
   */
  const void* data() const { return (void*)data_; }

private:
  uint8_t* data_;
  uint32_t   width_;
  uint32_t   height_;
};