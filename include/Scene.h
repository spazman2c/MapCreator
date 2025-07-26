#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// Forward declarations
class GameObject;
class Terrain;
class Model;
class Light;

class Scene {
public:
    Scene();
    ~Scene();
    
    // Scene management
    void Update(float deltaTime);
    void Render();
    
    // GameObject management
    GameObject* CreateGameObject(const std::string& name = "GameObject");
    void DestroyGameObject(GameObject* obj);
    GameObject* FindGameObject(const std::string& name);
    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return gameObjects; }
    
    // Terrain management
    void SetTerrain(std::unique_ptr<Terrain> terrain);
    Terrain* GetTerrain() { return terrain.get(); }
    
    // Model management
    Model* LoadModel(const std::string& path);
    void UnloadModel(const std::string& path);
    Model* GetModel(const std::string& path);
    
    // Lighting
    void AddLight(std::unique_ptr<Light> light);
    void RemoveLight(Light* light);
    const std::vector<std::unique_ptr<Light>>& GetLights() const { return lights; }
    
    // Scene properties
    void SetAmbientLight(const glm::vec3& ambient);
    glm::vec3 GetAmbientLight() const { return ambientLight; }
    
    // Scene serialization
    bool Save(const std::string& filename);
    bool Load(const std::string& filename);
    
    // Scene statistics
    size_t GetGameObjectCount() const { return gameObjects.size(); }
    size_t GetModelCount() const { return models.size(); }
    size_t GetLightCount() const { return lights.size(); }

private:
    // Scene objects
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::unique_ptr<Terrain> terrain;
    std::unordered_map<std::string, std::unique_ptr<Model>> models;
    std::vector<std::unique_ptr<Light>> lights;
    
    // Scene properties
    glm::vec3 ambientLight;
    
    // Scene hierarchy
    GameObject* rootNode;
}; 