#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    enum class CameraType {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    Camera(CameraType type = CameraType::PERSPECTIVE);
    
    // Camera properties
    void SetPosition(const glm::vec3& position);
    void SetTarget(const glm::vec3& target);
    void SetUp(const glm::vec3& up);
    
    // Camera controls
    void Orbit(float deltaX, float deltaY);
    void Pan(float deltaX, float deltaY);
    void Zoom(float delta);
    void Focus(const glm::vec3& point);
    
    // Matrix calculations
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewProjectionMatrix() const;
    
    // Getters
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetTarget() const { return target; }
    glm::vec3 GetUp() const { return up; }
    glm::vec3 GetFront() const { return front; }
    glm::vec3 GetRight() const { return right; }
    
    // Camera settings
    void SetFOV(float fov);
    void SetNearPlane(float near);
    void SetFarPlane(float far);
    void SetAspectRatio(float aspect);
    
    float GetFOV() const { return fov; }
    float GetNearPlane() const { return nearPlane; }
    float GetFarPlane() const { return farPlane; }
    float GetAspectRatio() const { return aspectRatio; }
    
    // Ray casting for picking
    glm::vec3 GetWorldRay(double mouseX, double mouseY, int screenWidth, int screenHeight) const;

private:
    void UpdateVectors();
    
    // Camera properties
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;
    
    // Camera settings
    float fov;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    
    // Camera type
    CameraType type;
    
    // Orbit camera parameters
    float distance;
    float yaw;
    float pitch;
}; 