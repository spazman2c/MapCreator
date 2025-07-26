#define GLM_ENABLE_EXPERIMENTAL
#define GL_SILENCE_DEPRECATION
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <cmath>

// ImGui includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Modern OpenGL includes
#include "ModernRenderer.h"

// Simple camera class for the graphical editor
class GraphicalCamera {
public:
    GraphicalCamera() 
        : position(0.0f, 40.0f, 40.0f) // Camera above and at an angle
        , target(0.0f, 5.0f, 0.0f)    // Look at the cube
        , up(0.0f, 1.0f, 0.0f)
        , fov(45.0f)
        , aspectRatio(16.0f / 9.0f)
        , nearPlane(0.1f)
        , farPlane(1000.0f)
        , distance(56.57f)
        , yaw(-45.0f)
        , pitch(-45.0f)
    {
    }
    
    void SetPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 GetPosition() const { return position; }
    
    void Orbit(float deltaX, float deltaY) {
        yaw += deltaX;
        pitch += deltaY;
        
        // Constrain pitch
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        
        // Calculate new position
        float x = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        float y = distance * sin(glm::radians(pitch));
        float z = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        
        position = target + glm::vec3(x, y, z);
    }
    
    void Zoom(float delta) {
        distance += delta;
        if (distance < 0.1f) distance = 0.1f;
        
        // Recalculate position
        float x = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        float y = distance * sin(glm::radians(pitch));
        float z = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        
        position = target + glm::vec3(x, y, z);
    }
    
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position, target, up);
    }
    
    glm::mat4 GetProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
    
    void SetAspectRatio(float aspect) { aspectRatio = aspect; }
    void SetFOV(float f) { fov = f; }
    float GetFOV() const { return fov; }

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    float distance;
    float yaw;
    float pitch;
};

// Main graphical map editor with modern OpenGL
class GraphicalMapEditor {
public:
    GraphicalMapEditor() 
        : window(nullptr)
        , windowWidth(1920)
        , windowHeight(1080)
        , currentMode(0) // 0=Terrain, 1=Model, 2=Camera
        , isRunning(false)
        , firstMouse(true)
        , lastMouseX(0.0)
        , lastMouseY(0.0)
        , showDemoWindow(false)
        , showProperties(true)
        , showHierarchy(true)
        , showTerrainPanel(false)
        , showModelPanel(false)
        , showConsole(false)
    {
    }
    
    ~GraphicalMapEditor() {
        Shutdown();
    }
    
    bool Initialize() {
        std::cout << "Initializing Graphical Cryptic Dungeon Map Editor..." << std::endl;
        
        if (!InitializeGLFW()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        if (!InitializeOpenGL()) {
            std::cerr << "Failed to initialize OpenGL" << std::endl;
            return false;
        }
        
        if (!InitializeImGui()) {
            std::cerr << "Failed to initialize ImGui" << std::endl;
            return false;
        }
        
        // Initialize modern renderer
        renderer = std::make_unique<ModernRenderer>();
        if (!renderer->Initialize()) {
            std::cerr << "Failed to initialize modern renderer" << std::endl;
            return false;
        }
        
        // Initialize camera and scene
        camera = std::make_unique<GraphicalCamera>();
        scene = std::make_unique<GraphicalScene>();
        
        std::cout << "Cryptic Dungeon Map Editor initialized successfully" << std::endl;
        return true;
    }
    
    void Run() {
        std::cout << "Starting graphical map editor..." << std::endl;
        isRunning = true;
        
        while (!glfwWindowShouldClose(window) && isRunning) {
            glfwShowWindow(window);
            
            ProcessInput();
            Update();
            Render();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
        std::cout << "Map editor shutdown complete" << std::endl;
    }
    
    void Shutdown() {
        if (window) {
            glfwDestroyWindow(window);
            window = nullptr;
        }
        glfwTerminate();
    }
    
    void SetMode(int mode) {
        currentMode = mode;
        std::cout << "Switched to ";
        switch (mode) {
            case 0: std::cout << "Terrain"; break;
            case 1: std::cout << "Model"; break;
            case 2: std::cout << "Camera"; break;
        }
        std::cout << " mode" << std::endl;
    }
    
    int GetCurrentMode() const { return currentMode; }

private:
    bool InitializeGLFW() {
        if (!glfwInit()) {
            return false;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
        window = glfwCreateWindow(windowWidth, windowHeight, "Cryptic Dungeon Map Editor", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            return false;
        }
        
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        
        // Set up callbacks
        glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
            static_cast<GraphicalMapEditor*>(glfwGetWindowUserPointer(w))->OnMouseMove(x, y);
        });
        
        glfwSetScrollCallback(window, [](GLFWwindow* w, double xoffset, double yoffset) {
            (void)xoffset; // Unused parameter
            static_cast<GraphicalMapEditor*>(glfwGetWindowUserPointer(w))->OnScroll(yoffset);
        });
        
        glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
            (void)scancode; (void)mods; // Unused parameters
            static_cast<GraphicalMapEditor*>(glfwGetWindowUserPointer(w))->OnKey(key, scancode, action, mods);
        });
        
        return true;
    }
    
    bool InitializeOpenGL() {
        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        
        // Set clear color
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        
        return true;
    }
    
    bool InitializeImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        ImGui::StyleColorsDark();
        
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        
        return true;
    }
    
    void ProcessInput() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            SetMode(2); // Switch to Camera mode
        }
    }
    
    void Update() {
        // Update logic here
    }
    
    void Render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set up viewport
        glViewport(0, 0, windowWidth, windowHeight);
        
        // Get matrices
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = camera->GetProjectionMatrix();
        
        // Render scene with modern OpenGL
        if (renderer) {
            // Render terrain
            renderer->RenderTerrain(view, projection);
            
            // Render cube at center
            glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 0.0f));
            renderer->RenderCube(cubeModel, view, projection);
            
            // Render grid
            renderer->RenderGrid(view, projection);
        }
        
        // Render ImGui UI
        RenderImGui();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    void RenderImGui() {
        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Render main menu bar
        RenderMainMenuBar();
        
        // Render toolbar
        RenderToolbar();
        
        // Render panels
        RenderProperties();
        RenderHierarchy();
        RenderTerrainPanel();
        RenderModelPanel();
        RenderConsole();
        
        // Render demo window (optional)
        if (showDemoWindow) {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }
        
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    void RenderMainMenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New", "Ctrl+N")) {
                    // TODO: New map
                }
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    // TODO: Open map
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    // TODO: Save map
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    isRunning = false;
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                    // TODO: Undo
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                    // TODO: Redo
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "Ctrl+X")) {
                    // TODO: Cut
                }
                if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                    // TODO: Copy
                }
                if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                    // TODO: Paste
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Properties", nullptr, &showProperties)) {}
                if (ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy)) {}
                if (ImGui::MenuItem("Terrain Panel", nullptr, &showTerrainPanel)) {}
                if (ImGui::MenuItem("Model Panel", nullptr, &showModelPanel)) {}
                if (ImGui::MenuItem("Console", nullptr, &showConsole)) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Demo Window", nullptr, &showDemoWindow)) {}
                ImGui::EndMenu();
            }
            
            ImGui::EndMainMenuBar();
        }
    }
    
    void RenderToolbar() {
        if (ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
            ImGui::SetWindowPos(ImVec2(0, 20), ImGuiCond_Always);
            ImGui::SetWindowSize(ImVec2(windowWidth, 40), ImGuiCond_Always);
            
            if (ImGui::Button("Terrain", ImVec2(80, 30))) SetMode(0);
            ImGui::SameLine();
            if (ImGui::Button("Model", ImVec2(80, 30))) SetMode(1);
            ImGui::SameLine();
            if (ImGui::Button("Camera", ImVec2(80, 30))) SetMode(2);
            
            ImGui::End();
        }
    }
    
    void RenderProperties() {
        if (!showProperties) return;
        
        ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Properties", &showProperties)) {
            if (ImGui::CollapsingHeader("Camera")) {
                float fov = camera->GetFOV();
                if (ImGui::SliderFloat("FOV", &fov, 10.0f, 120.0f)) {
                    camera->SetFOV(fov);
                }
                
                float aspect = 16.0f / 9.0f;
                if (ImGui::SliderFloat("Aspect Ratio", &aspect, 0.5f, 3.0f)) {
                    camera->SetAspectRatio(aspect);
                }
            }
            
            if (ImGui::CollapsingHeader("Scene")) {
                ImGui::Text("Ambient Light");
                static float ambient[3] = {0.2f, 0.2f, 0.2f};
                ImGui::ColorEdit3("##Ambient", ambient);
            }
            
            ImGui::End();
        }
    }
    
    void RenderHierarchy() {
        if (!showHierarchy) return;
        
        ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Hierarchy", &showHierarchy)) {
            if (ImGui::TreeNode("Scene")) {
                if (ImGui::TreeNode("Terrain")) {
                    if (ImGui::Button("Focus##Terrain")) {
                        // Focus camera on terrain
                        camera->SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
                    }
                    ImGui::TreePop();
                }
                
                if (ImGui::TreeNode("Cube")) {
                    if (ImGui::Button("Focus##Cube")) {
                        // Focus camera on cube
                        camera->SetPosition(glm::vec3(20.0f, 20.0f, 20.0f));
                    }
                    ImGui::TreePop();
                }
                
                ImGui::TreePop();
            }
            
            ImGui::End();
        }
    }
    
    void RenderTerrainPanel() {
        if (!showTerrainPanel) return;
        
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Terrain Editor", &showTerrainPanel)) {
            ImGui::Text("Terrain Tools");
            
            static int tool = 0;
            ImGui::RadioButton("Paint", &tool, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Raise/Lower", &tool, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Smooth", &tool, 2);
            
            static int brush = 0;
            ImGui::Text("Brush Size");
            ImGui::SliderInt("##BrushSize", &brush, 1, 10);
            
            static float strength = 1.0f;
            ImGui::Text("Strength");
            ImGui::SliderFloat("##Strength", &strength, 0.1f, 5.0f);
            
            ImGui::End();
        }
    }
    
    void RenderModelPanel() {
        if (!showModelPanel) return;
        
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Model Editor", &showModelPanel)) {
            ImGui::Text("Model Tools");
            
            static int tool = 0;
            ImGui::RadioButton("Place", &tool, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Select", &tool, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Delete", &tool, 2);
            
            static int transform = 0;
            ImGui::Text("Transform Mode");
            ImGui::RadioButton("Translate", &transform, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Rotate", &transform, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Scale", &transform, 2);
            
            ImGui::End();
        }
    }
    
    void RenderConsole() {
        if (!showConsole) return;
        
        ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Console", &showConsole)) {
            ImGui::Text("Console Output");
            ImGui::Separator();
            ImGui::Text("Ready for commands...");
            
            static char command[256] = "";
            if (ImGui::InputText("Command", command, sizeof(command), ImGuiInputTextFlags_EnterReturnsTrue)) {
                // Process command
                ImGui::SetKeyboardFocusHere(-1);
            }
            
            ImGui::End();
        }
    }
    
    void OnMouseMove(double xpos, double ypos) {
        if (currentMode == 2) { // Camera mode
            if (firstMouse) {
                lastMouseX = xpos;
                lastMouseY = ypos;
                firstMouse = false;
            }
            
            float xoffset = xpos - lastMouseX;
            float yoffset = lastMouseY - ypos;
            
            lastMouseX = xpos;
            lastMouseY = ypos;
            
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                camera->Orbit(xoffset * 0.1f, yoffset * 0.1f);
            }
        }
    }
    
    void OnScroll(double yoffset) {
        if (currentMode == 2) { // Camera mode
            camera->Zoom(-yoffset * 2.0f);
        }
    }
    
    void OnKey(int key, int scancode, int action, int mods) {
        (void)scancode; (void)mods; // Unused parameters
        
        if (action == GLFW_PRESS) {
            std::cout << "Key pressed: " << key << std::endl;
            
            switch (key) {
                case GLFW_KEY_1:
                    SetMode(0);
                    break;
                case GLFW_KEY_2:
                    SetMode(1);
                    break;
                case GLFW_KEY_3:
                    SetMode(2);
                    break;
                case GLFW_KEY_Q:
                    isRunning = false;
                    break;
            }
        }
    }
    
    // Simple scene class (placeholder)
    class GraphicalScene {
    public:
        GraphicalScene() {
            std::cout << "Graphical scene initialized" << std::endl;
        }
    };
    
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    int currentMode;
    bool isRunning;
    bool firstMouse;
    double lastMouseX, lastMouseY;
    
    // UI state
    bool showDemoWindow;
    bool showProperties;
    bool showHierarchy;
    bool showTerrainPanel;
    bool showModelPanel;
    bool showConsole;
    
    std::unique_ptr<GraphicalCamera> camera;
    std::unique_ptr<GraphicalScene> scene;
    std::unique_ptr<ModernRenderer> renderer;
};

int main() {
    std::cout << "Initializing Graphical Cryptic Dungeon Map Editor..." << std::endl;
    
    GraphicalMapEditor editor;
    
    if (!editor.Initialize()) {
        std::cerr << "Failed to initialize map editor" << std::endl;
        return -1;
    }
    
    std::cout << "Controls:" << std::endl;
    std::cout << "  1 - Terrain Editor Mode" << std::endl;
    std::cout << "  2 - Model Placement Mode" << std::endl;
    std::cout << "  3 - Camera Mode (mouse to orbit, scroll to zoom)" << std::endl;
    std::cout << "  ESC - Switch to Camera Mode" << std::endl;
    std::cout << "  Q - Exit" << std::endl;
    
    editor.Run();
    
    return 0;
} 