#pragma once

#include <glm/glm.hpp>

enum class LightType {
    DIRECTIONAL,
    POINT,
    SPOT
};

class Light {
public:
    Light(LightType type = LightType::POINT);
    ~Light();
    
    // Light properties
    void SetType(LightType type);
    LightType GetType() const { return type; }
    
    void SetPosition(const glm::vec3& position);
    glm::vec3 GetPosition() const { return position; }
    
    void SetDirection(const glm::vec3& direction);
    glm::vec3 GetDirection() const { return direction; }
    
    void SetColor(const glm::vec3& color);
    glm::vec3 GetColor() const { return color; }
    
    void SetIntensity(float intensity);
    float GetIntensity() const { return intensity; }
    
    // Spot light properties
    void SetConeAngle(float angle);
    float GetConeAngle() const { return coneAngle; }
    
    void SetRange(float range);
    float GetRange() const { return range; }
    
    // Light state
    void SetEnabled(bool enabled);
    bool IsEnabled() const { return enabled; }
    
    // Rendering
    void Render();
    
private:
    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float coneAngle;
    float range;
    bool enabled;
}; 