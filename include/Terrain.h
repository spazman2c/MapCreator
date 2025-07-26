#pragma once

#include <vector>
#include <string>

class Terrain {
public:
    Terrain();
    ~Terrain();
    
    // Terrain generation
    void Generate(int width, int height, float scale);
    void LoadHeightmap(const std::string& filename);
    void SaveHeightmap(const std::string& filename);
    
    // Height data
    void SetHeight(int x, int z, float height);
    float GetHeight(int x, int z) const;
    
    // Terrain properties
    void SetScale(float scale);
    float GetScale() const { return scale; }
    
    void SetMaxHeight(float height);
    float GetMaxHeight() const { return maxHeight; }
    
    // Rendering
    void Render();
    
    // Utility
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    
private:
    std::vector<float> heightData;
    int width;
    int height;
    float scale;
    float maxHeight;
}; 