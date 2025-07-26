#include "MapData.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>

MapData::MapData() : modified(false) {
    NewMap();
}

MapData::~MapData() {
}

std::string MapData::GenerateMapId() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);
    
    return "map_" + std::to_string(timestamp) + "_" + std::to_string(dis(gen));
}

void MapData::NewMap() {
    mapId = GenerateMapId();
    mapName = "New Map";
    currentFilepath = "";
    objects.clear();
    modified = false;
    
    // Add default objects
    AddObject("Terrain", "terrain", glm::vec3(0.0f, 0.0f, 0.0f));
    AddObject("Center Cube", "cube", glm::vec3(0.0f, 5.0f, 0.0f));
    AddObject("Grid", "grid", glm::vec3(0.0f, 0.0f, 0.0f));
    
    std::cout << "Created new map: " << mapId << std::endl;
}

bool MapData::SaveMap(const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filepath << std::endl;
            return false;
        }
        
        // Write map header
        file << "CRYPTIC_DUNGEON_MAP" << std::endl;
        file << "VERSION:1.0" << std::endl;
        file << "MAP_ID:" << mapId << std::endl;
        file << "MAP_NAME:" << mapName << std::endl;
        file << "CREATED:" << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
        file << std::endl;
        
        // Write objects
        file << "OBJECTS:" << objects.size() << std::endl;
        for (const auto& obj : objects) {
            file << "OBJECT_START" << std::endl;
            file << "ID:" << obj.id << std::endl;
            file << "NAME:" << obj.name << std::endl;
            file << "TYPE:" << obj.type << std::endl;
            file << "POSITION:" << obj.position.x << "," << obj.position.y << "," << obj.position.z << std::endl;
            file << "ROTATION:" << obj.rotation.x << "," << obj.rotation.y << "," << obj.rotation.z << std::endl;
            file << "SCALE:" << obj.scale.x << "," << obj.scale.y << "," << obj.scale.z << std::endl;
            file << "COLOR:" << obj.color.x << "," << obj.color.y << "," << obj.color.z << std::endl;
            file << "VISIBLE:" << (obj.visible ? "true" : "false") << std::endl;
            file << "OBJECT_END" << std::endl;
        }
        
        file.close();
        currentFilepath = filepath;
        modified = false;
        
        std::cout << "Map saved successfully: " << filepath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving map: " << e.what() << std::endl;
        return false;
    }
}

bool MapData::LoadMap(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for reading: " << filepath << std::endl;
            return false;
        }
        
        std::string line;
        std::getline(file, line);
        if (line != "CRYPTIC_DUNGEON_MAP") {
            std::cerr << "Invalid map file format" << std::endl;
            return false;
        }
        
        // Clear existing objects
        objects.clear();
        
        // Read map header
        while (std::getline(file, line) && line != "OBJECTS:") {
            if (line.substr(0, 8) == "MAP_ID:") {
                mapId = line.substr(8);
            } else if (line.substr(0, 10) == "MAP_NAME:") {
                mapName = line.substr(10);
            }
        }
        
        // Read object count
        if (line.substr(0, 8) == "OBJECTS:") {
            int objectCount = std::stoi(line.substr(8));
            
            // Read objects
            for (int i = 0; i < objectCount; i++) {
                MapObject obj("", "");
                
                while (std::getline(file, line) && line != "OBJECT_END") {
                    if (line == "OBJECT_START") continue;
                    
                    if (line.substr(0, 3) == "ID:") {
                        obj.id = line.substr(3);
                    } else if (line.substr(0, 5) == "NAME:") {
                        obj.name = line.substr(5);
                    } else if (line.substr(0, 5) == "TYPE:") {
                        obj.type = line.substr(5);
                    } else if (line.substr(0, 9) == "POSITION:") {
                        std::string posStr = line.substr(9);
                        size_t comma1 = posStr.find(',');
                        size_t comma2 = posStr.find(',', comma1 + 1);
                        obj.position.x = std::stof(posStr.substr(0, comma1));
                        obj.position.y = std::stof(posStr.substr(comma1 + 1, comma2 - comma1 - 1));
                        obj.position.z = std::stof(posStr.substr(comma2 + 1));
                    } else if (line.substr(0, 9) == "ROTATION:") {
                        std::string rotStr = line.substr(9);
                        size_t comma1 = rotStr.find(',');
                        size_t comma2 = rotStr.find(',', comma1 + 1);
                        obj.rotation.x = std::stof(rotStr.substr(0, comma1));
                        obj.rotation.y = std::stof(rotStr.substr(comma1 + 1, comma2 - comma1 - 1));
                        obj.rotation.z = std::stof(rotStr.substr(comma2 + 1));
                    } else if (line.substr(0, 6) == "SCALE:") {
                        std::string scaleStr = line.substr(6);
                        size_t comma1 = scaleStr.find(',');
                        size_t comma2 = scaleStr.find(',', comma1 + 1);
                        obj.scale.x = std::stof(scaleStr.substr(0, comma1));
                        obj.scale.y = std::stof(scaleStr.substr(comma1 + 1, comma2 - comma1 - 1));
                        obj.scale.z = std::stof(scaleStr.substr(comma2 + 1));
                    } else if (line.substr(0, 6) == "COLOR:") {
                        std::string colorStr = line.substr(6);
                        size_t comma1 = colorStr.find(',');
                        size_t comma2 = colorStr.find(',', comma1 + 1);
                        obj.color.x = std::stof(colorStr.substr(0, comma1));
                        obj.color.y = std::stof(colorStr.substr(comma1 + 1, comma2 - comma1 - 1));
                        obj.color.z = std::stof(colorStr.substr(comma2 + 1));
                    } else if (line.substr(0, 8) == "VISIBLE:") {
                        obj.visible = (line.substr(8) == "true");
                    }
                }
                
                objects.push_back(obj);
            }
        }
        
        file.close();
        currentFilepath = filepath;
        modified = false;
        
        std::cout << "Map loaded successfully: " << filepath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading map: " << e.what() << std::endl;
        return false;
    }
}

void MapData::AddObject(const std::string& name, const std::string& type, const glm::vec3& position) {
    objects.emplace_back(name, type, position);
    modified = true;
    std::cout << "Added object: " << name << " (" << type << ")" << std::endl;
}

void MapData::DeleteObject(const std::string& objectId) {
    auto it = std::find_if(objects.begin(), objects.end(), 
                          [&objectId](const MapObject& obj) { return obj.id == objectId; });
    if (it != objects.end()) {
        std::cout << "Deleted object: " << it->name << std::endl;
        objects.erase(it);
        modified = true;
    }
}

void MapData::DeleteObjectByName(const std::string& name) {
    auto it = std::find_if(objects.begin(), objects.end(), 
                          [&name](const MapObject& obj) { return obj.name == name; });
    if (it != objects.end()) {
        std::cout << "Deleted object: " << it->name << std::endl;
        objects.erase(it);
        modified = true;
    }
}

MapObject* MapData::GetObject(const std::string& objectId) {
    auto it = std::find_if(objects.begin(), objects.end(), 
                          [&objectId](const MapObject& obj) { return obj.id == objectId; });
    return (it != objects.end()) ? &(*it) : nullptr;
}

MapObject* MapData::GetObjectByName(const std::string& name) {
    auto it = std::find_if(objects.begin(), objects.end(), 
                          [&name](const MapObject& obj) { return obj.name == name; });
    return (it != objects.end()) ? &(*it) : nullptr;
}

void MapData::SetObjectPosition(const std::string& objectId, const glm::vec3& position) {
    MapObject* obj = GetObject(objectId);
    if (obj) {
        obj->position = position;
        modified = true;
    }
}

void MapData::SetObjectRotation(const std::string& objectId, const glm::vec3& rotation) {
    MapObject* obj = GetObject(objectId);
    if (obj) {
        obj->rotation = rotation;
        modified = true;
    }
}

void MapData::SetObjectScale(const std::string& objectId, const glm::vec3& scale) {
    MapObject* obj = GetObject(objectId);
    if (obj) {
        obj->scale = scale;
        modified = true;
    }
}

void MapData::SetObjectColor(const std::string& objectId, const glm::vec3& color) {
    MapObject* obj = GetObject(objectId);
    if (obj) {
        obj->color = color;
        modified = true;
    }
}

void MapData::SetObjectVisible(const std::string& objectId, bool visible) {
    MapObject* obj = GetObject(objectId);
    if (obj) {
        obj->visible = visible;
        modified = true;
    }
} 