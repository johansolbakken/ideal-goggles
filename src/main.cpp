
#include "ppm.h"
#include <cstdint>
#include "renderer.h"
#include "camera.h"
#include "timer.hpp"

#include <iostream>

Scene makeScene() {
    Scene scene;
    {
        Sphere sphere;
        sphere.position ={0.0f, 0.0f, 0.0f};
        sphere.radius = 1.0f;
        sphere.materialIndex = 0;
        scene.spheres.push_back(sphere);
    }
    {
        Sphere sphere;
        sphere.position ={0.0f, -101.0f, 0.0f};
        sphere.radius = 100.0f;
        sphere.materialIndex = 1;
        scene.spheres.push_back(sphere);
    }
    {
        Material material;
        material.albedo = {1.0f, 0.0f, 1.0f};
        material.roughness = 1.0f;
        scene.materials.push_back(material);
    }
    {
        Material material;
        material.albedo = {0.2f, 0.3f, 1.0f};
        material.roughness = 1.0f;
        scene.materials.push_back(material);
    }
    return scene;
}

int main()
{
    const int imageWidth = 800;
    const int imageHeight = 800;
    
    Scene scene = makeScene();
    
    Camera camera(45.0f, 0.1f, 100.0f);
    camera.OnResize(imageWidth, imageHeight);
    camera.OnUpdate(0);
    
    Renderer renderer;
    renderer.onResize(imageWidth, imageHeight);
    
    int runs = 5;
    float time = 0;
    {
        Timer timer("Total running time");
        for (int i = 0; i < runs; i++) {
            std::cout << "Render: #" << i << " ";
            renderer.render(scene, camera);
            time += timer.time();
            timer.reset();
        }
    }
    std::cout << "Average render time: " << time / runs << "ms." << std::endl;
    pixel image[imageWidth * imageHeight];
    auto finalImage = renderer.getFinalImage();
    for (int i = 0; i < imageWidth * imageHeight; i++)
    {
        uint32_t color = finalImage[i];
        uint32_t r = color & 0xff;
        uint32_t g = (color >> 8) & 0xff;
        uint32_t b = (color >> 16) & 0xff;
        pixel p = { r, g, b };
        image[i] = p;
    }
    
    for (int y = 0; y < imageHeight / 2; y++)
    {
        for (int x = 0; x < imageWidth; x++)
        {
            int y1 = imageHeight - 1 - y;
            pixel tmp = image[x + y * imageWidth];
            image[x + y * imageHeight] = image[x + y1 * imageWidth];
            image[x + y1 * imageWidth] = tmp;
        }
    }
    
    write_ppm(image, imageWidth * imageHeight, imageWidth,imageHeight, "hei.ppm");
    system("open hei.ppm");
    
    return 0;
}
