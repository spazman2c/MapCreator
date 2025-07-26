#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeindex>

// Forward declarations
class Component;
class Transform;
class Model;
class Light;

class GameObject {
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();
    
    // Basic properties
    void SetName(const std::string& name);
    const std::string& GetName() const { return name; }
    
    void SetActive(bool active);
    bool IsActive() const { return isActive; }
    
    // Transform
    Transform* GetTransform() { return transform.get(); }
    const Transform* GetTransform() const { return transform.get(); }
    
    // Component management
    template<typename T>
    T* AddComponent() {
        auto component = std::make_unique<T>(this);
        T* ptr = component.get();
        components[typeid(T)] = std::move(component);
        return ptr;
    }
    
    template<typename T>
    T* GetComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }
    
    template<typename T>
    const T* GetComponent() const {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return static_cast<const T*>(it->second.get());
        }
        return nullptr;
    }
    
    template<typename T>
    void RemoveComponent() {
        components.erase(typeid(T));
    }
    
    // Hierarchy
    void SetParent(GameObject* parent);
    GameObject* GetParent() const { return parent; }
    
    void AddChild(GameObject* child);
    void RemoveChild(GameObject* child);
    const std::vector<GameObject*>& GetChildren() const { return children; }
    
    // Lifecycle
    void Start();
    void Update(float deltaTime);
    void Render();
    void Destroy();
    
    // Utility
    GameObject* FindChild(const std::string& name);
    GameObject* FindChildRecursive(const std::string& name);
    
    // Serialization
    void Serialize(std::ostream& stream);
    void Deserialize(std::istream& stream);

private:
    std::string name;
    bool isActive;
    
    std::unique_ptr<Transform> transform;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
    
    GameObject* parent;
    std::vector<GameObject*> children;
    
    bool started;
    bool destroyed;
};

// Base Component class
class Component {
public:
    Component(GameObject* owner);
    virtual ~Component() = default;
    
    GameObject* GetGameObject() { return gameObject; }
    const GameObject* GetGameObject() const { return gameObject; }
    
    virtual void Start() {}
    virtual void Update(float deltaTime) {}
    virtual void Render() {}
    
    virtual void Serialize(std::ostream& stream) {}
    virtual void Deserialize(std::istream& stream) {}

protected:
    GameObject* gameObject;
}; 