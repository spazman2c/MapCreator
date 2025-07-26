#pragma once

#include <string>
#include <vector>

class Model {
public:
    Model();
    ~Model();
    
    // Model loading
    bool Load(const std::string& path);
    void Unload();
    
    // Rendering
    void Render();
    
    // Model properties
    void SetVisible(bool visible);
    bool IsVisible() const { return visible; }
    
    // Bounding box
    struct BoundingBox {
        float minX, minY, minZ;
        float maxX, maxY, maxZ;
    };
    
    BoundingBox GetBoundingBox() const { return boundingBox; }
    
    // Utility
    const std::string& GetPath() const { return path; }
    bool IsLoaded() const { return loaded; }
    
private:
    std::string path;
    bool loaded;
    bool visible;
    BoundingBox boundingBox;
}; 