#include "Scene.h"
#include "GameObject.h"
#include "Terrain.h"
#include "Model.h"
#include "Light.h"
#include <iostream>

Scene::Scene()
    : rootNode(nullptr)
    , ambientLight(0.2f, 0.2f, 0.2f)
{
    // Create root node
    rootNode = CreateGameObject("Scene Root");
}

Scene::~Scene() {
    // Clean up all game objects
    gameObjects.clear();
}

void Scene::Update(float deltaTime) {
    // Update all game objects
    for (auto& obj : gameObjects) {
        if (obj->IsActive()) {
            obj->Update(deltaTime);
        }
    }
}

void Scene::Render() {
    // Render all game objects
    for (auto& obj : gameObjects) {
        if (obj->IsActive()) {
            obj->Render();
        }
    }
}

GameObject* Scene::CreateGameObject(const std::string& name) {
    auto gameObject = std::make_unique<GameObject>(name);
    GameObject* ptr = gameObject.get();
    gameObjects.push_back(std::move(gameObject));
    return ptr;
}

void Scene::DestroyGameObject(GameObject* obj) {
    if (!obj) return;
    
    // Remove from game objects list
    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(),
            [obj](const std::unique_ptr<GameObject>& go) {
                return go.get() == obj;
            }),
        gameObjects.end()
    );
}

GameObject* Scene::FindGameObject(const std::string& name) {
    for (auto& obj : gameObjects) {
        if (obj->GetName() == name) {
            return obj.get();
        }
    }
    return nullptr;
}

void Scene::SetTerrain(std::unique_ptr<Terrain> newTerrain) {
    terrain = std::move(newTerrain);
}

Model* Scene::LoadModel(const std::string& path) {
    auto it = models.find(path);
    if (it != models.end()) {
        return it->second.get();
    }
    
    // TODO: Implement model loading with Assimp
    auto model = std::make_unique<Model>();
    Model* ptr = model.get();
    models[path] = std::move(model);
    return ptr;
}

void Scene::UnloadModel(const std::string& path) {
    models.erase(path);
}

Model* Scene::GetModel(const std::string& path) {
    auto it = models.find(path);
    if (it != models.end()) {
        return it->second.get();
    }
    return nullptr;
}

void Scene::AddLight(std::unique_ptr<Light> light) {
    lights.push_back(std::move(light));
}

void Scene::RemoveLight(Light* light) {
    if (!light) return;
    
    lights.erase(
        std::remove_if(lights.begin(), lights.end(),
            [light](const std::unique_ptr<Light>& l) {
                return l.get() == light;
            }),
        lights.end()
    );
}

void Scene::SetAmbientLight(const glm::vec3& ambient) {
    ambientLight = ambient;
}

bool Scene::Save(const std::string& filename) {
    // TODO: Implement scene serialization
    std::cout << "Saving scene to: " << filename << std::endl;
    return true;
}

bool Scene::Load(const std::string& filename) {
    // TODO: Implement scene deserialization
    std::cout << "Loading scene from: " << filename << std::endl;
    return true;
} 