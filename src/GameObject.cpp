#include "GameObject.h"
#include "Transform.h"
#include <iostream>

GameObject::GameObject(const std::string& name)
    : name(name)
    , isActive(true)
    , parent(nullptr)
    , started(false)
    , destroyed(false)
{
    transform = std::make_unique<Transform>();
}

GameObject::~GameObject() {
    Destroy();
}

void GameObject::SetName(const std::string& newName) {
    name = newName;
}

void GameObject::SetActive(bool active) {
    isActive = active;
}

void GameObject::SetParent(GameObject* newParent) {
    if (parent) {
        parent->RemoveChild(this);
    }
    
    parent = newParent;
    
    if (parent) {
        parent->AddChild(this);
    }
}

void GameObject::AddChild(GameObject* child) {
    if (child && std::find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
    }
}

void GameObject::RemoveChild(GameObject* child) {
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

void GameObject::Start() {
    if (started || destroyed) return;
    
    started = true;
    
    // Start all components
    for (auto& component : components) {
        component.second->Start();
    }
    
    // Start all children
    for (auto child : children) {
        child->Start();
    }
}

void GameObject::Update(float deltaTime) {
    if (!isActive || destroyed) return;
    
    // Update all components
    for (auto& component : components) {
        component.second->Update(deltaTime);
    }
    
    // Update all children
    for (auto child : children) {
        child->Update(deltaTime);
    }
}

void GameObject::Render() {
    if (!isActive || destroyed) return;
    
    // Render all components
    for (auto& component : components) {
        component.second->Render();
    }
    
    // Render all children
    for (auto child : children) {
        child->Render();
    }
}

void GameObject::Destroy() {
    if (destroyed) return;
    
    destroyed = true;
    
    // Destroy all children
    for (auto child : children) {
        child->Destroy();
    }
    children.clear();
    
    // Clear components
    components.clear();
}

GameObject* GameObject::FindChild(const std::string& childName) {
    for (auto child : children) {
        if (child->GetName() == childName) {
            return child;
        }
    }
    return nullptr;
}

GameObject* GameObject::FindChildRecursive(const std::string& childName) {
    // Check direct children first
    auto child = FindChild(childName);
    if (child) return child;
    
    // Recursively search children
    for (auto child : children) {
        auto result = child->FindChildRecursive(childName);
        if (result) return result;
    }
    
    return nullptr;
}

void GameObject::Serialize(std::ostream& stream) {
    // TODO: Implement serialization
}

void GameObject::Deserialize(std::istream& stream) {
    // TODO: Implement deserialization
}

// Component implementation
Component::Component(GameObject* owner)
    : gameObject(owner)
{
} 