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
#include "MapData.h"
#include "AssetManager.h"

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
        , showAssetBrowser(false)
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
        
        // Initialize camera, scene, map data, and asset manager
        camera = std::make_unique<GraphicalCamera>();
        scene = std::make_unique<GraphicalScene>();
        mapData = std::make_unique<MapData>();
        assetManager = std::make_unique<AssetManager>();
        
        if (!assetManager->Initialize()) {
            std::cerr << "Failed to initialize asset manager" << std::endl;
            return false;
        }
        
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
        if (renderer && mapData) {
            // Render all map objects
            renderer->RenderMapObjects(*mapData, view, projection);
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
                    if (mapData) {
                        mapData->NewMap();
                    }
                }
                if (ImGui::MenuItem("Open", "Ctrl+O")) {
                    // TODO: Open file dialog
                    if (mapData) {
                        mapData->LoadMap("test_map.cdm");
                    }
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    // TODO: Save file dialog
                    if (mapData) {
                        mapData->SaveMap("test_map.cdm");
                    }
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
                if (ImGui::MenuItem("Terrain Editor", nullptr, &showTerrainPanel)) {}
                if (ImGui::MenuItem("Model Editor", nullptr, &showModelPanel)) {}
                if (ImGui::MenuItem("Console", nullptr, &showConsole)) {}
                if (ImGui::MenuItem("Asset Browser", nullptr, &showAssetBrowser)) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Demo Window", nullptr, &showDemoWindow)) {}
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Tools")) {
                if (ImGui::MenuItem("Terrain Editor")) {
                    SetMode(0);
                    showTerrainPanel = true;
                }
                if (ImGui::MenuItem("Model Editor")) {
                    SetMode(1);
                    showModelPanel = true;
                }
                if (ImGui::MenuItem("Camera Mode")) {
                    SetMode(2);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Grid Settings...")) {
                    // TODO: Grid settings dialog
                }
                if (ImGui::MenuItem("Snap Settings...")) {
                    // TODO: Snap settings dialog
                }
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
            ImGui::Text("Selected Object Properties");
            ImGui::Separator();
            
            // Camera properties
            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                glm::vec3 pos = camera->GetPosition();
                float posArray[3] = {pos.x, pos.y, pos.z};
                if (ImGui::DragFloat3("Position", posArray, 0.1f)) {
                    camera->SetPosition(glm::vec3(posArray[0], posArray[1], posArray[2]));
                }
                
                float fov = camera->GetFOV();
                if (ImGui::SliderFloat("FOV", &fov, 30.0f, 120.0f)) {
                    camera->SetFOV(fov);
                }
                
                float aspect = 16.0f / 9.0f;
                if (ImGui::SliderFloat("Aspect Ratio", &aspect, 0.5f, 3.0f)) {
                    camera->SetAspectRatio(aspect);
                }
                
                // Camera controls
                if (ImGui::Button("Reset Camera")) {
                    camera->SetPosition(glm::vec3(0.0f, 50.0f, 50.0f));
                }
            }
            
            // Scene properties
            if (ImGui::CollapsingHeader("Scene")) {
                static float ambientLight[3] = {0.2f, 0.2f, 0.2f};
                ImGui::ColorEdit3("Ambient Light", ambientLight);
                
                static float directionalLight[3] = {1.0f, 1.0f, 1.0f};
                ImGui::ColorEdit3("Directional Light", directionalLight);
                
                static float lightIntensity = 1.0f;
                ImGui::SliderFloat("Light Intensity", &lightIntensity, 0.0f, 2.0f);
                
                static bool showGrid = true;
                ImGui::Checkbox("Show Grid", &showGrid);
                
                static bool showAxes = true;
                ImGui::Checkbox("Show Axes", &showAxes);
            }
            
            // Selected object properties
            if (ImGui::CollapsingHeader("Selected Object")) {
                // TODO: Show properties of selected object
                ImGui::Text("No object selected");
                
                static float objPos[3] = {0.0f, 0.0f, 0.0f};
                ImGui::DragFloat3("Position", objPos, 0.1f);
                
                static float objRot[3] = {0.0f, 0.0f, 0.0f};
                ImGui::DragFloat3("Rotation", objRot, 1.0f);
                
                static float objScale[3] = {1.0f, 1.0f, 1.0f};
                ImGui::DragFloat3("Scale", objScale, 0.1f);
                
                static float objColor[3] = {1.0f, 1.0f, 1.0f};
                ImGui::ColorEdit3("Color", objColor);
                
                static bool objVisible = true;
                ImGui::Checkbox("Visible", &objVisible);
            }
            
            ImGui::End();
        }
    }
    
    void RenderHierarchy() {
        if (!showHierarchy) return;
        
        ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Hierarchy", &showHierarchy)) {
            if (mapData) {
                const auto& objects = mapData->GetAllObjects();
                
                if (ImGui::TreeNode("Scene")) {
                    for (const auto& obj : objects) {
                        ImGui::PushID(obj.id.c_str());
                        
                        bool nodeOpen = ImGui::TreeNode(obj.name.c_str());
                        
                        // Object controls
                        if (nodeOpen) {
                            // Focus button
                            if (ImGui::Button("Focus")) {
                                camera->SetPosition(obj.position + glm::vec3(20.0f, 20.0f, 20.0f));
                            }
                            ImGui::SameLine();
                            
                            // Delete button
                            if (ImGui::Button("Delete")) {
                                mapData->DeleteObject(obj.id);
                            }
                            
                            // Object properties
                            ImGui::Text("Type: %s", obj.type.c_str());
                            ImGui::Text("Position: %.1f, %.1f, %.1f", obj.position.x, obj.position.y, obj.position.z);
                            ImGui::Text("Visible: %s", obj.visible ? "Yes" : "No");
                            
                            ImGui::TreePop();
                        }
                        
                        ImGui::PopID();
                    }
                    
                    ImGui::TreePop();
                }
                
                // Add new object button
                if (ImGui::Button("Add Cube")) {
                    mapData->AddObject("New Cube", "cube", glm::vec3(10.0f, 5.0f, 10.0f));
                }
                ImGui::SameLine();
                if (ImGui::Button("Add Terrain")) {
                    mapData->AddObject("New Terrain", "terrain", glm::vec3(0.0f, 0.0f, 0.0f));
                }
            }
            
            ImGui::End();
        }
    }
    
    void RenderTerrainPanel() {
        if (!showTerrainPanel) return;
        
        ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Terrain Editor", &showTerrainPanel)) {
            ImGui::Text("Terrain Tools");
            ImGui::Separator();
            
            // Tool selection
            static int tool = 0;
            ImGui::Text("Tool:");
            ImGui::RadioButton("Paint", &tool, 0);
            ImGui::RadioButton("Raise/Lower", &tool, 1);
            ImGui::RadioButton("Smooth", &tool, 2);
            ImGui::RadioButton("Flatten", &tool, 3);
            ImGui::RadioButton("Noise", &tool, 4);
            
            ImGui::Separator();
            
            // Brush settings
            ImGui::Text("Brush Settings");
            static int brushSize = 5;
            ImGui::SliderInt("Size", &brushSize, 1, 20);
            
            static float brushStrength = 1.0f;
            ImGui::SliderFloat("Strength", &brushStrength, 0.1f, 5.0f);
            
            static float brushFalloff = 0.5f;
            ImGui::SliderFloat("Falloff", &brushFalloff, 0.0f, 1.0f);
            
            ImGui::Separator();
            
            // Terrain settings
            ImGui::Text("Terrain Settings");
            static float terrainHeight = 0.0f;
            ImGui::SliderFloat("Height", &terrainHeight, -10.0f, 10.0f);
            
            static float terrainScale = 1.0f;
            ImGui::SliderFloat("Scale", &terrainScale, 0.1f, 5.0f);
            
            // Color picker for terrain
            static float terrainColor[3] = {1.0f, 1.0f, 1.0f};
            ImGui::ColorEdit3("Terrain Color", terrainColor);
            
            ImGui::Separator();
            
            // Action buttons
            if (ImGui::Button("Apply Tool", ImVec2(120, 30))) {
                // TODO: Apply terrain tool
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Terrain", ImVec2(120, 30))) {
                // TODO: Reset terrain
            }
            
            ImGui::End();
        }
    }
    
    void RenderModelPanel() {
        if (!showModelPanel) return;
        
        ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Model Editor", &showModelPanel)) {
            ImGui::Text("Model Tools");
            ImGui::Separator();
            
            // Tool selection
            static int tool = 0;
            ImGui::Text("Tool:");
            ImGui::RadioButton("Place", &tool, 0);
            ImGui::RadioButton("Select", &tool, 1);
            ImGui::RadioButton("Move", &tool, 2);
            ImGui::RadioButton("Rotate", &tool, 3);
            ImGui::RadioButton("Scale", &tool, 4);
            ImGui::RadioButton("Delete", &tool, 5);
            
            ImGui::Separator();
            
            // Transform mode
            static int transform = 0;
            ImGui::Text("Transform Mode:");
            ImGui::RadioButton("Local", &transform, 0);
            ImGui::RadioButton("World", &transform, 1);
            
            ImGui::Separator();
            
            // Snapping settings
            ImGui::Text("Snapping");
            static bool snapToGrid = true;
            ImGui::Checkbox("Snap to Grid", &snapToGrid);
            
            static float gridSize = 1.0f;
            ImGui::SliderFloat("Grid Size", &gridSize, 0.1f, 10.0f);
            
            static bool snapToAngle = true;
            ImGui::Checkbox("Snap to Angle", &snapToAngle);
            
            static float angleSnap = 15.0f;
            ImGui::SliderFloat("Angle Snap", &angleSnap, 1.0f, 90.0f);
            
            ImGui::Separator();
            
            // Asset browser
            if (assetManager) {
                ImGui::Text("Asset Browser");
                
                static int selectedCategory = 0;
                const auto& categories = AssetManager::CATEGORIES;
                if (ImGui::BeginCombo("Category", categories[selectedCategory].c_str())) {
                    for (int i = 0; i < categories.size(); i++) {
                        bool isSelected = (selectedCategory == i);
                        if (ImGui::Selectable(categories[i].c_str(), isSelected)) {
                            selectedCategory = i;
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                
                // Show assets in selected category
                if (selectedCategory < categories.size()) {
                    auto categoryAssets = assetManager->GetAssetsByCategory(categories[selectedCategory]);
                    
                    ImGui::BeginChild("AssetList", ImVec2(0, 150), true);
                    for (const auto& asset : categoryAssets) {
                        if (ImGui::Selectable(asset.name.c_str())) {
                            // TODO: Select asset for placement
                        }
                    }
                    ImGui::EndChild();
                }
            }
            
            ImGui::Separator();
            
            // Action buttons
            if (ImGui::Button("Place Selected", ImVec2(120, 30))) {
                // TODO: Place selected asset
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear Selection", ImVec2(120, 30))) {
                // TODO: Clear selection
            }
            
            ImGui::End();
        }
    }
    
    void RenderConsole() {
        if (!showConsole) return;
        
        ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Console", &showConsole)) {
            ImGui::Text("Console Output");
            ImGui::Separator();
            
            // Console output area
            ImGui::BeginChild("ConsoleOutput", ImVec2(0, 220), true);
            ImGui::Text("Cryptic Dungeon Map Editor Console");
            ImGui::Text("Type 'help' for available commands");
            ImGui::Separator();
            ImGui::Text("Available Commands:");
            ImGui::Text("  help - Show this help");
            ImGui::Text("  new - Create new map");
            ImGui::Text("  save <filename> - Save map");
            ImGui::Text("  load <filename> - Load map");
            ImGui::Text("  add <type> <name> - Add object");
            ImGui::Text("  delete <name> - Delete object");
            ImGui::Text("  list - List all objects");
            ImGui::Text("  camera <x> <y> <z> - Set camera position");
            ImGui::Text("  mode <0|1|2> - Set mode (0=Terrain, 1=Model, 2=Camera)");
            ImGui::Text("  clear - Clear console");
            ImGui::Text("  quit - Exit application");
            ImGui::EndChild();
            
            // Command input
            static char commandBuffer[256] = "";
            if (ImGui::InputText("Command", commandBuffer, sizeof(commandBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                ProcessConsoleCommand(commandBuffer);
                commandBuffer[0] = '\0';
            }
            
            ImGui::End();
        }
    }
    
    void ProcessConsoleCommand(const std::string& command) {
        std::string cmd = command;
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
        
        if (cmd == "help") {
            // Help already shown in console
        } else if (cmd == "new") {
            if (mapData) mapData->NewMap();
        } else if (cmd.substr(0, 4) == "save") {
            std::string filename = cmd.substr(5);
            if (mapData && !filename.empty()) mapData->SaveMap(filename);
        } else if (cmd.substr(0, 4) == "load") {
            std::string filename = cmd.substr(5);
            if (mapData && !filename.empty()) mapData->LoadMap(filename);
        } else if (cmd.substr(0, 3) == "add") {
            // TODO: Parse add command
        } else if (cmd.substr(0, 6) == "delete") {
            std::string name = cmd.substr(7);
            if (mapData && !name.empty()) mapData->DeleteObjectByName(name);
        } else if (cmd == "list") {
            if (mapData) {
                const auto& objects = mapData->GetAllObjects();
                for (const auto& obj : objects) {
                    std::cout << "Object: " << obj.name << " (" << obj.type << ")" << std::endl;
                }
            }
        } else if (cmd.substr(0, 6) == "camera") {
            // TODO: Parse camera command
        } else if (cmd.substr(0, 4) == "mode") {
            int mode = std::stoi(cmd.substr(5));
            SetMode(mode);
        } else if (cmd == "clear") {
            // Console will be cleared on next frame
        } else if (cmd == "quit") {
            isRunning = false;
        } else if (!cmd.empty()) {
            std::cout << "Unknown command: " << command << std::endl;
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
    bool showAssetBrowser;
    
    std::unique_ptr<GraphicalCamera> camera;
    std::unique_ptr<GraphicalScene> scene;
    std::unique_ptr<ModernRenderer> renderer;
    std::unique_ptr<MapData> mapData;
    std::unique_ptr<AssetManager> assetManager;
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