#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include "ray.h"
#include "camera.h"
#include "scene.hpp"

class Renderer
{
public:
    struct Settings
    {
        bool accumulate = true;
    };
public:
    void render(const Scene& scene, const Camera& camera);
    void onResize(uint32_t width, uint32_t height);
    
    const uint32_t* getFinalImage() const { return m_finalImage; }
    
    void resetFrameIndex() { m_frameIndex = 1; }
    
    Settings& settings() { return m_settings; }
    
private:
    struct HitPayload
    {
        float hitDistance;
        glm::vec3 worldPosition;
        glm::vec3 worldNormal;
        
        uint32_t objectIndex;
    };
    
    glm::vec4 perPixel(uint32_t x, uint32_t y); // RayGen
    HitPayload traceRay(const Ray& ray);
    HitPayload closestHit(const Ray& ray, float hitDistance, uint32_t objectIndex);
    HitPayload miss(const Ray& ray);
    
private:
    uint32_t* m_finalImage = nullptr;
    uint32_t* m_imageData = nullptr;
    glm::vec4* m_accumulationData = nullptr;
    uint32_t m_width, m_height;
    uint32_t m_frameIndex = 1;

    Settings m_settings;
    
    const Scene* m_activeScene = nullptr;
    const Camera* m_activeCamera = nullptr;
};

