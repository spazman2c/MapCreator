#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <random>

// Forward declarations
class GameObject;

struct MapObject {
    std::string id;
    std::string name;
    std::string type; // "terrain", "cube", "grid", etc.
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 color;
    bool visible;
    
    MapObject(const std::string& objName, const std::string& objType, const glm::vec3& pos = glm::vec3(0.0f))
        : id(GenerateUniqueId())
        , name(objName)
        , type(objType)
        , position(pos)
        , rotation(0.0f)
        , scale(1.0f)
        , color(1.0f, 1.0f, 1.0f)
        , visible(true)
    {}
    
private:
    static std::string GenerateUniqueId() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(100000, 999999);
        
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        
        return "obj_" + std::to_string(timestamp) + "_" + std::to_string(dis(gen));
    }
};

class MapData {
public:
    MapData();
    ~MapData();
    
    // Map management
    void NewMap();
    bool SaveMap(const std::string& filepath);
    bool LoadMap(const std::string& filepath);
    std::string GetMapId() const { return mapId; }
    std::string GetMapName() const { return mapName; }
    void SetMapName(const std::string& name) { mapName = name; }
    
    // Object management
    void AddObject(const std::string& name, const std::string& type, const glm::vec3& position = glm::vec3(0.0f));
    void DeleteObject(const std::string& objectId);
    void DeleteObjectByName(const std::string& name);
    MapObject* GetObject(const std::string& objectId);
    MapObject* GetObjectByName(const std::string& name);
    const std::vector<MapObject>& GetAllObjects() const { return objects; }
    
    // Object modification
    void SetObjectPosition(const std::string& objectId, const glm::vec3& position);
    void SetObjectRotation(const std::string& objectId, const glm::vec3& rotation);
    void SetObjectScale(const std::string& objectId, const glm::vec3& scale);
    void SetObjectColor(const std::string& objectId, const glm::vec3& color);
    void SetObjectVisible(const std::string& objectId, bool visible);
    
    // Map state
    bool IsModified() const { return modified; }
    void SetModified(bool mod = true) { modified = mod; }
    void ClearModified() { modified = false; }
    
private:
    std::string GenerateMapId();
    
    std::string mapId;
    std::string mapName;
    std::string currentFilepath;
    std::vector<MapObject> objects;
    bool modified;
}; 