#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform {
public:
    Transform();
    ~Transform();
    
    // Position
    void SetPosition(const glm::vec3& position);
    void SetPosition(float x, float y, float z);
    glm::vec3 GetPosition() const { return position; }
    
    // Rotation
    void SetRotation(const glm::quat& rotation);
    void SetRotation(const glm::vec3& eulerAngles);
    void SetRotation(float x, float y, float z);
    glm::quat GetRotation() const { return rotation; }
    glm::vec3 GetEulerAngles() const;
    
    // Scale
    void SetScale(const glm::vec3& scale);
    void SetScale(float x, float y, float z);
    void SetScale(float uniformScale);
    glm::vec3 GetScale() const { return scale; }
    
    // Transform matrices
    glm::mat4 GetLocalMatrix() const;
    glm::mat4 GetWorldMatrix() const;
    glm::mat4 GetInverseWorldMatrix() const;
    
    // Transform vectors
    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;
    
    // Local space transformations
    void Translate(const glm::vec3& translation);
    void Rotate(const glm::vec3& eulerAngles);
    void Rotate(const glm::quat& quaternion);
    void RotateAround(const glm::vec3& point, const glm::vec3& axis, float angle);
    
    // World space transformations
    void SetWorldPosition(const glm::vec3& worldPosition);
    void SetWorldRotation(const glm::quat& worldRotation);
    void SetWorldScale(const glm::vec3& worldScale);
    
    glm::vec3 GetWorldPosition() const;
    glm::quat GetWorldRotation() const;
    glm::vec3 GetWorldScale() const;
    
    // Look at
    void LookAt(const glm::vec3& target);
    void LookAt(const glm::vec3& target, const glm::vec3& up);
    
    // Utility
    float GetDistance(const Transform& other) const;
    bool IsChildOf(const Transform& parent) const;
    
    // Hierarchy
    void SetParent(Transform* parent);
    Transform* GetParent() const { return parent; }
    
    void AddChild(Transform* child);
    void RemoveChild(Transform* child);
    const std::vector<Transform*>& GetChildren() const { return children; }
    
    // Dirty flag management
    void MarkDirty();
    bool IsDirty() const { return isDirty; }
    void UpdateMatrices();

private:
    void UpdateLocalMatrix();
    void UpdateWorldMatrix();
    void PropagateDirty();
    
    // Transform data
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    
    // Cached matrices
    mutable glm::mat4 localMatrix;
    mutable glm::mat4 worldMatrix;
    mutable glm::mat4 inverseWorldMatrix;
    
    // Hierarchy
    Transform* parent;
    std::vector<Transform*> children;
    
    // Dirty flags
    mutable bool isDirty;
    mutable bool localMatrixDirty;
    mutable bool worldMatrixDirty;
}; 