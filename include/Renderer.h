#pragma once

// Forward declarations
class Scene;
class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    // Initialization
    bool Initialize();
    void Shutdown();
    
    // Rendering
    void Render(Scene* scene, Camera* camera);
    void RenderScene(Scene* scene, Camera* camera);
    void RenderTerrain(Scene* scene, Camera* camera);
    void RenderModels(Scene* scene, Camera* camera);
    void RenderLights(Scene* scene, Camera* camera);
    
    // Render settings
    void SetWireframe(bool wireframe);
    bool IsWireframe() const { return wireframe; }
    
    void SetShowBoundingBoxes(bool show);
    bool ShowBoundingBoxes() const { return showBoundingBoxes; }
    
    void SetShowGrid(bool show);
    bool ShowGrid() const { return showGrid; }
    
    // Utility
    void Clear();
    void SwapBuffers();
    
private:
    bool wireframe;
    bool showBoundingBoxes;
    bool showGrid;
    bool initialized;
}; 