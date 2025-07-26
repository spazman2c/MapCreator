#pragma once

#include <memory>
#include <vector>
#include <string>

// Forward declarations
class Terrain;
class Camera;
class Scene;

enum class TerrainTool {
    RAISE_LOWER,
    SMOOTH,
    FLATTEN,
    PAINT,
    NOISE
};

enum class TerrainBrush {
    CIRCLE,
    SQUARE,
    SOFT
};

class TerrainEditor {
public:
    TerrainEditor(Scene* scene);
    ~TerrainEditor();
    
    // Tool management
    void SetTool(TerrainTool tool);
    void SetBrush(TerrainBrush brush);
    void SetBrushSize(float size);
    void SetBrushStrength(float strength);
    
    TerrainTool GetCurrentTool() const { return currentTool; }
    TerrainBrush GetCurrentBrush() const { return currentBrush; }
    float GetBrushSize() const { return brushSize; }
    float GetBrushStrength() const { return brushStrength; }
    
    // Terrain editing
    void OnMouseDown(int button, double x, double y);
    void OnMouseMove(double x, double y);
    void OnMouseUp(int button);
    void OnScroll(double delta);
    
    // Terrain operations
    void RaiseLower(float delta, double x, double y);
    void Smooth(double x, double y);
    void Flatten(double x, double y);
    void Paint(double x, double y, int textureLayer);
    void ApplyNoise(double x, double y);
    
    // Terrain generation
    void GenerateTerrain(int width, int height, float scale);
    void LoadHeightmap(const std::string& filename);
    void SaveHeightmap(const std::string& filename);
    
    // Terrain properties
    void SetTerrainScale(float scale);
    void SetTerrainHeight(float height);
    void SetTextureScale(float scale);
    
    float GetTerrainScale() const { return terrainScale; }
    float GetTerrainHeight() const { return terrainHeight; }
    float GetTextureScale() const { return textureScale; }
    
    // Brush preview
    void DrawBrushPreview(double x, double y);
    
    // Undo/Redo
    void Undo();
    void Redo();
    bool CanUndo() const;
    bool CanRedo() const;

private:
    // Helper functions
    glm::vec3 GetTerrainPoint(double mouseX, double mouseY);
    bool IsPointInTerrain(const glm::vec3& point);
    void ApplyBrushEffect(double x, double y, std::function<void(int, int, float)> effect);
    float GetBrushFalloff(float distance, float radius);
    
    // Terrain editing state
    TerrainTool currentTool;
    TerrainBrush currentBrush;
    float brushSize;
    float brushStrength;
    bool isEditing;
    
    // Terrain properties
    float terrainScale;
    float terrainHeight;
    float textureScale;
    
    // Scene reference
    Scene* scene;
    Terrain* terrain;
    Camera* camera;
    
    // Undo/Redo system
    struct TerrainState {
        std::vector<float> heightData;
        std::vector<glm::vec4> textureData;
    };
    
    std::vector<TerrainState> undoStack;
    std::vector<TerrainState> redoStack;
    size_t maxUndoSteps;
}; 