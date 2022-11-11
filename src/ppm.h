#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>

struct pixel
{
    uint32_t r, g, b;
};

void write_ppm(pixel *image, uint32_t count, uint32_t width, uint32_t height, const std::string& filename);
