#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

// Forward declarations
class Camera;
class Scene;
class TerrainEditor;
class ModelEditor;
class UI;
class Renderer;

enum class EditorMode {
    TERRAIN,
    MODEL_PLACEMENT,
    MODEL_EDIT,
    CAMERA
};

class MapEditor {
public:
    MapEditor();
    ~MapEditor();
    
    bool Initialize();
    void Run();
    void Shutdown();
    
    // Editor state management
    void SetMode(EditorMode mode);
    EditorMode GetCurrentMode() const { return currentMode; }
    
    // File operations
    bool LoadMap(const std::string& filename);
    bool SaveMap(const std::string& filename);
    bool ExportMap(const std::string& filename);
    
    // Scene management
    Scene* GetScene() { return scene.get(); }
    Camera* GetCamera() { return camera.get(); }
    
    // UI callbacks
    void OnMouseMove(double xpos, double ypos);
    void OnMouseButton(int button, int action, int mods);
    void OnScroll(double xoffset, double yoffset);
    void OnKey(int key, int scancode, int action, int mods);

private:
    bool InitializeGLFW();
    bool InitializeOpenGL();
    bool InitializeImGui();
    void ProcessInput();
    void Update();
    void Render();
    
    // Editor components
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<TerrainEditor> terrainEditor;
    std::unique_ptr<ModelEditor> modelEditor;
    std::unique_ptr<UI> ui;
    std::unique_ptr<Renderer> renderer;
    
    // Window and context
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    
    // Editor state
    EditorMode currentMode;
    bool isRunning;
    
    // Input state
    bool mousePressed[3];
    double lastMouseX, lastMouseY;
    bool firstMouse;
}; 