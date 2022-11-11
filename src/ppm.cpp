#include "ppm.h"

#include <fstream>

void write_ppm(pixel *image, uint32_t count, uint32_t width, uint32_t height, const std::string& filename)
{
    std::ofstream stream(filename);
    stream<< "P3\n" << width << ' ' << height << "\n255\n";
    
    for (int i = 0; i < count; i++)
    {
        pixel color = image[i];
        stream << color.r << ' ' << color.g << ' ' << color.b << '\n';
    }
    
    stream.close();
}
