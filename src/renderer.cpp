#include "renderer.h"
#include "timer.hpp"

namespace Utils {
    static uint32_t convertToRGBA(const glm::vec4& color)
    {
        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);
        
        uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
        return result;
    }
    
    static float random(float low, float high)
    {
        return low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
    }
    
    static glm::vec3 randomVec3(float low, float high)
    {
        return { random(low, high), random(low, high), random(low, high)};
    }
}

void Renderer::render(const Scene& scene, const Camera& camera)
{
    m_activeScene = &scene;
    m_activeCamera = &camera;
    
    if (m_frameIndex == 1)
        memset(m_accumulationData, 0, m_width * m_height * sizeof(glm::vec4));
    
    Timer timer("Renderer::Render");
    
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; ++x) {
            auto color = perPixel(x, y);
            m_accumulationData[x + y * m_width] += color;
            
            auto accumulatedColor = m_accumulationData[x + y * m_width];
            accumulatedColor /= (float) m_frameIndex;
            
            accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
            m_imageData[x + y * m_width] = Utils::convertToRGBA(accumulatedColor);
        }
    }
    
    for (int i = 0; i < m_width * m_height; i++)
    {
        m_finalImage[i] = m_imageData[i];
    }
    
    if (m_settings.accumulate)
        m_frameIndex ++;
    else
        m_frameIndex = 1;
}

void Renderer::onResize(uint32_t width, uint32_t height)
{
    delete[] m_imageData;
    delete[] m_finalImage;
    delete[] m_accumulationData;
    m_imageData = new uint32_t[width * height];
    m_finalImage = new uint32_t[width * height];
    m_accumulationData = new glm::vec4[width * height];
    m_width = width;
    m_height = height;
}

glm::vec4 Renderer::perPixel(uint32_t x, uint32_t y) // RayGen
{
    Ray ray;
    ray.origin = m_activeCamera->GetPosition();
    ray.direction = m_activeCamera->GetRayDirections()[x + y * m_width];
    
    glm::vec3 color(0.0f);
    float multiplier = 1.0f;
    
    int bounces = 5;
    for (int i = 0; i < bounces; i++)
    {
        auto payload = traceRay(ray);
        if (payload.hitDistance < 0.0f)
        {
            glm::vec3 skyColor(0.6f, 0.7f, 0.9f);
            color += skyColor * multiplier;
            break;
        }
        
        glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
        float lightIntensity = glm::max(glm::dot(payload.worldNormal, -lightDir), 0.0f);
        
        const Sphere& sphere = m_activeScene->spheres[payload.objectIndex];
        const Material& material = m_activeScene->materials[sphere.materialIndex];
        glm::vec3 sphereColor = material.albedo;
        sphereColor *= lightIntensity;
        sphereColor *= multiplier;
        color += sphereColor;
        
        multiplier *= 0.5f;
        
        ray.origin = payload.worldPosition + payload.worldNormal * 0.0001f;
        ray.direction = glm::reflect(ray.direction,
                                     payload.worldNormal + material.roughness * Utils::randomVec3(-0.5f, 0.5f));
    }
    
    return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::traceRay(const Ray& ray)
{
    int closestSphere = -1;
    float hitDistance = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < m_activeScene->spheres.size(); i++)
    {
        const Sphere& sphere = m_activeScene->spheres[i];
        glm::vec3 origin = ray.origin - sphere.position;
        
        float a = glm::dot(ray.direction, ray.direction);
        float b = 2.0f * glm::dot(ray.direction, origin);
        float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;
        
        float discriminant = b * b - 4.0f * a * c;
        float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
        
        if (discriminant < 0.0f)
            continue;
        
        float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        if (closestT > 0.0f && closestT < hitDistance)
        {
            closestSphere = (int) i;
            hitDistance = closestT;
        }
    }
    
    if (closestSphere < 0)
        return miss(ray);
    
    return closestHit(ray, hitDistance, static_cast<uint32_t>(closestSphere));
}

Renderer::HitPayload Renderer::closestHit(const Ray& ray, float hitDistance, uint32_t objectIndex)
{
    Renderer::HitPayload payload;
    payload.hitDistance = hitDistance;
    payload.objectIndex = objectIndex;
    
    const Sphere& closestSphere = m_activeScene->spheres[objectIndex];
    
    glm::vec3 origin = ray.origin - closestSphere.position;
    payload.worldPosition = origin + ray.direction * hitDistance;
    payload.worldNormal = glm::normalize(payload.worldPosition);
    
    payload.worldPosition += closestSphere.position;
    
    return payload;
}

Renderer::HitPayload Renderer::miss(const Ray& ray)
{
    Renderer::HitPayload payload;
    payload.hitDistance = -1;
    return payload;
}
