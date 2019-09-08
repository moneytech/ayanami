#pragma once

#include "nicemath.h"

void set_seed(int s);

/**
 * Produces a random float in [0; 1] range.
 */
float randf();

/**
 * Produces a random 3D point on a sphere of radius 1.
 */
nm::float3 random_unit_vector();
/**
 * Produces a random 3D point within a disk of radius 1.
 */
nm::float2 random_in_unit_disk();
