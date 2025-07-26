#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <cmath>

Camera::Camera(CameraType type)
    : position(0.0f, 0.0f, 3.0f)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)
    , front(0.0f, 0.0f, -1.0f)
    , right(1.0f, 0.0f, 0.0f)
    , fov(45.0f)
    , nearPlane(0.1f)
    , farPlane(1000.0f)
    , aspectRatio(16.0f / 9.0f)
    , type(type)
    , distance(3.0f)
    , yaw(-90.0f)
    , pitch(0.0f)
{
    UpdateVectors();
}

void Camera::SetPosition(const glm::vec3& pos) {
    position = pos;
    UpdateVectors();
}

void Camera::SetTarget(const glm::vec3& t) {
    target = t;
    UpdateVectors();
}

void Camera::SetUp(const glm::vec3& u) {
    up = u;
    UpdateVectors();
}

void Camera::Orbit(float deltaX, float deltaY) {
    yaw += deltaX;
    pitch += deltaY;
    
    // Constrain pitch
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    
    // Calculate new position
    float x = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    float y = distance * sin(glm::radians(pitch));
    float z = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    
    position = target + glm::vec3(x, y, z);
    UpdateVectors();
}

void Camera::Pan(float deltaX, float deltaY) {
    glm::vec3 right = glm::normalize(glm::cross(front, up));
    glm::vec3 worldUp = glm::normalize(glm::cross(right, front));
    
    position += right * deltaX + worldUp * deltaY;
    target += right * deltaX + worldUp * deltaY;
    UpdateVectors();
}

void Camera::Zoom(float delta) {
    distance += delta;
    if (distance < 0.1f) distance = 0.1f;
    
    // Recalculate position based on new distance
    float x = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    float y = distance * sin(glm::radians(pitch));
    float z = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    
    position = target + glm::vec3(x, y, z);
    UpdateVectors();
}

void Camera::Focus(const glm::vec3& point) {
    target = point;
    UpdateVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    if (type == CameraType::PERSPECTIVE) {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    } else {
        float halfHeight = distance * 0.5f;
        float halfWidth = halfHeight * aspectRatio;
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
    }
}

glm::mat4 Camera::GetViewProjectionMatrix() const {
    return GetProjectionMatrix() * GetViewMatrix();
}

void Camera::SetFOV(float f) {
    fov = f;
}

void Camera::SetNearPlane(float near) {
    nearPlane = near;
}

void Camera::SetFarPlane(float far) {
    farPlane = far;
}

void Camera::SetAspectRatio(float aspect) {
    aspectRatio = aspect;
}

glm::vec3 Camera::GetWorldRay(double mouseX, double mouseY, int screenWidth, int screenHeight) const {
    // Convert mouse coordinates to normalized device coordinates
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    
    // Convert to clip coordinates
    glm::vec4 clipCoords(x, y, -1.0f, 1.0f);
    
    // Convert to eye coordinates
    glm::mat4 projectionMatrix = GetProjectionMatrix();
    glm::vec4 eyeCoords = glm::inverse(projectionMatrix) * clipCoords;
    eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
    
    // Convert to world coordinates
    glm::mat4 viewMatrix = GetViewMatrix();
    glm::vec4 worldCoords = glm::inverse(viewMatrix) * eyeCoords;
    
    return glm::normalize(glm::vec3(worldCoords));
}

void Camera::UpdateVectors() {
    front = glm::normalize(target - position);
    right = glm::normalize(glm::cross(front, up));
    up = glm::normalize(glm::cross(right, front));
} 