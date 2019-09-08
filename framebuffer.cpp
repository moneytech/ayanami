#include "framebuffer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

framebuffer::framebuffer(uint32_t width, uint32_t height) :
    width_ (width),
    height_ (height),
    data_ ((uint8_t*)malloc(width* height* kBytesPerPixel)) {}

framebuffer::~framebuffer() { if(data_) free(data_); }

framebuffer& framebuffer::operator=(framebuffer &&other) noexcept {
  if (data_) free(data_);

  data_   = other.data_;
  width_  = other.width_;
  height_ = other.height_;

  other.width_  = 0u;
  other.height_ = 0u;
  other.data_   = nullptr;

  return *this;
}

void framebuffer::set_pixel(uint32_t  row,
                            uint32_t  col,
                            uint8_t   r,
                            uint8_t   g,
                            uint8_t   b) {
  const size_t idx = kBytesPerPixel * (row * width_ + col);

  data_[idx + 0] = b;
  data_[idx + 1] = g;
  data_[idx + 2] = r;
}

void framebuffer::save(const char* file_path) const {
  FILE* fptr = fopen(file_path, "wb");
  assert(fptr);

  // Write targa header, this portion taken from
  // http://paulbourke.net/dataformats/tga/
  putc(0, fptr);
  putc(0, fptr);
  putc(2, fptr);                         /* uncompressed RGB */
  putc(0, fptr); putc(0, fptr);
  putc(0, fptr); putc(0, fptr);
  putc(0, fptr);
  putc(0, fptr); putc(0, fptr);           /* X origin */
  putc(0, fptr); putc(0, fptr);           /* y origin */
  putc((width_ & 0x00FF), fptr);
  putc((width_ & 0xFF00) / 256, fptr);
  putc((height_ & 0x00FF), fptr);
  putc((height_ & 0xFF00) / 256, fptr);
  putc(24, fptr);                        /* 24 bit bitmap */
  putc(0, fptr);

  // Write image data.
  fwrite(data_, kBytesPerPixel, width_ * height_, fptr);
  fclose(fptr);
}

