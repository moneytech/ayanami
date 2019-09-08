#pragma once

#include "material.h"
#include "nicemath.h"

class metal : public material {
public:
  metal(const nm::float3 attn, float fuzz);

  bool scatter(const ray        &in,
               const hit_record &rec,
               nm::float3       &attn,
               ray              &scattered) const override;

private:
  nm::float3 attn_;
  float      fuzz_;
};