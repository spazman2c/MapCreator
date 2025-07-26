#include "Transform.h"
#include <cmath>

Transform::Transform()
    : position(0.0f, 0.0f, 0.0f)
    , rotation(1.0f, 0.0f, 0.0f, 0.0f)
    , scale(1.0f, 1.0f, 1.0f)
    , parent(nullptr)
    , isDirty(true)
    , localMatrixDirty(true)
    , worldMatrixDirty(true)
{
}

Transform::~Transform() {
}

void Transform::SetPosition(const glm::vec3& pos) {
    position = pos;
    MarkDirty();
}

void Transform::SetPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
    MarkDirty();
}

void Transform::SetRotation(const glm::quat& rot) {
    rotation = rot;
    MarkDirty();
}

void Transform::SetRotation(const glm::vec3& eulerAngles) {
    rotation = glm::quat(glm::radians(eulerAngles));
    MarkDirty();
}

void Transform::SetRotation(float x, float y, float z) {
    SetRotation(glm::vec3(x, y, z));
}

glm::vec3 Transform::GetEulerAngles() const {
    return glm::degrees(glm::eulerAngles(rotation));
}

void Transform::SetScale(const glm::vec3& s) {
    scale = s;
    MarkDirty();
}

void Transform::SetScale(float x, float y, float z) {
    scale = glm::vec3(x, y, z);
    MarkDirty();
}

void Transform::SetScale(float uniformScale) {
    scale = glm::vec3(uniformScale);
    MarkDirty();
}

glm::mat4 Transform::GetLocalMatrix() const {
    if (localMatrixDirty) {
        UpdateLocalMatrix();
    }
    return localMatrix;
}

glm::mat4 Transform::GetWorldMatrix() const {
    if (worldMatrixDirty) {
        UpdateWorldMatrix();
    }
    return worldMatrix;
}

glm::mat4 Transform::GetInverseWorldMatrix() const {
    return glm::inverse(GetWorldMatrix());
}

glm::vec3 Transform::GetForward() const {
    return glm::normalize(glm::vec3(GetWorldMatrix()[2]));
}

glm::vec3 Transform::GetRight() const {
    return glm::normalize(glm::vec3(GetWorldMatrix()[0]));
}

glm::vec3 Transform::GetUp() const {
    return glm::normalize(glm::vec3(GetWorldMatrix()[1]));
}

void Transform::Translate(const glm::vec3& translation) {
    position += translation;
    MarkDirty();
}

void Transform::Rotate(const glm::vec3& eulerAngles) {
    rotation = glm::quat(glm::radians(eulerAngles)) * rotation;
    MarkDirty();
}

void Transform::Rotate(const glm::quat& quaternion) {
    rotation = quaternion * rotation;
    MarkDirty();
}

void Transform::RotateAround(const glm::vec3& point, const glm::vec3& axis, float angle) {
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    glm::vec3 direction = position - point;
    glm::vec3 rotatedDirection = glm::vec3(rotationMatrix * glm::vec4(direction, 0.0f));
    position = point + rotatedDirection;
    MarkDirty();
}

void Transform::SetWorldPosition(const glm::vec3& worldPos) {
    if (parent) {
        glm::mat4 parentInverse = glm::inverse(parent->GetWorldMatrix());
        glm::vec4 localPos = parentInverse * glm::vec4(worldPos, 1.0f);
        position = glm::vec3(localPos);
    } else {
        position = worldPos;
    }
    MarkDirty();
}

void Transform::SetWorldRotation(const glm::quat& worldRot) {
    if (parent) {
        glm::quat parentInverse = glm::inverse(parent->GetWorldRotation());
        rotation = parentInverse * worldRot;
    } else {
        rotation = worldRot;
    }
    MarkDirty();
}

void Transform::SetWorldScale(const glm::vec3& worldScl) {
    if (parent) {
        glm::vec3 parentScale = parent->GetWorldScale();
        scale = worldScl / parentScale;
    } else {
        scale = worldScl;
    }
    MarkDirty();
}

glm::vec3 Transform::GetWorldPosition() const {
    return glm::vec3(GetWorldMatrix()[3]);
}

glm::quat Transform::GetWorldRotation() const {
    if (parent) {
        return parent->GetWorldRotation() * rotation;
    }
    return rotation;
}

glm::vec3 Transform::GetWorldScale() const {
    if (parent) {
        return parent->GetWorldScale() * scale;
    }
    return scale;
}

void Transform::LookAt(const glm::vec3& target) {
    LookAt(target, glm::vec3(0, 1, 0));
}

void Transform::LookAt(const glm::vec3& target, const glm::vec3& up) {
    glm::mat4 lookAtMatrix = glm::lookAt(position, target, up);
    rotation = glm::quat_cast(lookAtMatrix);
    MarkDirty();
}

float Transform::GetDistance(const Transform& other) const {
    return glm::length(GetWorldPosition() - other.GetWorldPosition());
}

bool Transform::IsChildOf(const Transform& parentTransform) const {
    Transform* current = parent;
    while (current) {
        if (current == &parentTransform) {
            return true;
        }
        current = current->parent;
    }
    return false;
}

void Transform::SetParent(Transform* newParent) {
    if (parent) {
        parent->RemoveChild(this);
    }
    
    parent = newParent;
    
    if (parent) {
        parent->AddChild(this);
    }
    
    MarkDirty();
}

void Transform::AddChild(Transform* child) {
    if (child && std::find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
    }
}

void Transform::RemoveChild(Transform* child) {
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

void Transform::MarkDirty() {
    isDirty = true;
    localMatrixDirty = true;
    worldMatrixDirty = true;
    PropagateDirty();
}

void Transform::UpdateMatrices() {
    UpdateLocalMatrix();
    UpdateWorldMatrix();
}

void Transform::UpdateLocalMatrix() {
    localMatrix = glm::translate(glm::mat4(1.0f), position) *
                  glm::mat4_cast(rotation) *
                  glm::scale(glm::mat4(1.0f), scale);
    localMatrixDirty = false;
}

void Transform::UpdateWorldMatrix() {
    if (parent) {
        worldMatrix = parent->GetWorldMatrix() * GetLocalMatrix();
    } else {
        worldMatrix = GetLocalMatrix();
    }
    worldMatrixDirty = false;
}

void Transform::PropagateDirty() {
    for (auto child : children) {
        child->worldMatrixDirty = true;
        child->PropagateDirty();
    }
} 