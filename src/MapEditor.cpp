#include "MapEditor.h"
#include "Camera.h"
#include "Scene.h"
#include "TerrainEditor.h"
#include "ModelEditor.h"
#include "UI.h"
#include "Renderer.h"
#include <iostream>
#include <stdexcept>

MapEditor::MapEditor() 
    : window(nullptr)
    , windowWidth(1920)
    , windowHeight(1080)
    , currentMode(EditorMode::CAMERA)
    , isRunning(false)
    , firstMouse(true)
{
    for (int i = 0; i < 3; ++i) {
        mousePressed[i] = false;
    }
}

MapEditor::~MapEditor() {
    Shutdown();
}

bool MapEditor::Initialize() {
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
    
    // Initialize editor components
    camera = std::make_unique<Camera>();
    scene = std::make_unique<Scene>();
    terrainEditor = std::make_unique<TerrainEditor>(scene.get());
    modelEditor = std::make_unique<ModelEditor>(scene.get());
    ui = std::make_unique<UI>(this);
    renderer = std::make_unique<Renderer>();
    
    // Set up camera
    camera->SetPosition(glm::vec3(0, 10, 10));
    camera->SetTarget(glm::vec3(0, 0, 0));
    camera->SetUp(glm::vec3(0, 1, 0));
    camera->SetAspectRatio((float)windowWidth / (float)windowHeight);
    
    isRunning = true;
    return true;
}

bool MapEditor::InitializeGLFW() {
    if (!glfwInit()) {
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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
        MapEditor* editor = static_cast<MapEditor*>(glfwGetWindowUserPointer(w));
        editor->OnMouseMove(x, y);
    });
    
    glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
        MapEditor* editor = static_cast<MapEditor*>(glfwGetWindowUserPointer(w));
        editor->OnMouseButton(button, action, mods);
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow* w, double xoffset, double yoffset) {
        MapEditor* editor = static_cast<MapEditor*>(glfwGetWindowUserPointer(w));
        editor->OnScroll(xoffset, yoffset);
    });
    
    glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        MapEditor* editor = static_cast<MapEditor*>(glfwGetWindowUserPointer(w));
        editor->OnKey(key, scancode, action, mods);
    });
    
    return true;
}

bool MapEditor::InitializeOpenGL() {
    // Initialize OpenGL extensions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return false;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return true;
}

bool MapEditor::InitializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    
    return true;
}

void MapEditor::Run() {
    while (isRunning && !glfwWindowShouldClose(window)) {
        ProcessInput();
        Update();
        Render();
        
        glfwPollEvents();
    }
}

void MapEditor::Shutdown() {
    if (ui) ui.reset();
    if (modelEditor) modelEditor.reset();
    if (terrainEditor) terrainEditor.reset();
    if (renderer) renderer.reset();
    if (scene) scene.reset();
    if (camera) camera.reset();
    
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    
    glfwTerminate();
}

void MapEditor::SetMode(EditorMode mode) {
    currentMode = mode;
    
    // Update input handling based on mode
    switch (mode) {
        case EditorMode::TERRAIN:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case EditorMode::MODEL_PLACEMENT:
        case EditorMode::MODEL_EDIT:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case EditorMode::CAMERA:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
    }
}

void MapEditor::ProcessInput() {
    if (currentMode == EditorMode::CAMERA) {
        // Camera controls
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera->SetPosition(camera->GetPosition() + camera->GetFront() * 0.1f);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera->SetPosition(camera->GetPosition() - camera->GetFront() * 0.1f);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera->SetPosition(camera->GetPosition() - camera->GetRight() * 0.1f);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera->SetPosition(camera->GetPosition() + camera->GetRight() * 0.1f);
    }
}

void MapEditor::Update() {
    // Update scene
    if (scene) {
        scene->Update(0.016f); // Fixed timestep for now
    }
    
    // Update UI
    if (ui) {
        ui->Update();
    }
}

void MapEditor::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
    // Render scene
    if (renderer && scene && camera) {
        renderer->Render(scene.get(), camera.get());
    }
    
    // Render UI
    if (ui) {
        ui->Render();
    }
    
    glfwSwapBuffers(window);
}

void MapEditor::OnMouseMove(double xpos, double ypos) {
    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }
    
    double xoffset = xpos - lastMouseX;
    double yoffset = lastMouseY - ypos;
    
    lastMouseX = xpos;
    lastMouseY = ypos;
    
    // Handle input based on current mode
    switch (currentMode) {
        case EditorMode::TERRAIN:
            if (terrainEditor) {
                terrainEditor->OnMouseMove(xpos, ypos);
            }
            break;
        case EditorMode::MODEL_PLACEMENT:
        case EditorMode::MODEL_EDIT:
            if (modelEditor) {
                modelEditor->OnMouseMove(xpos, ypos);
            }
            break;
        case EditorMode::CAMERA:
            if (camera) {
                camera->Orbit(xoffset * 0.1f, yoffset * 0.1f);
            }
            break;
    }
}

void MapEditor::OnMouseButton(int button, int action, int mods) {
    if (button >= 0 && button < 3) {
        mousePressed[button] = (action == GLFW_PRESS);
    }
    
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    switch (currentMode) {
        case EditorMode::TERRAIN:
            if (terrainEditor) {
                if (action == GLFW_PRESS) {
                    terrainEditor->OnMouseDown(button, xpos, ypos);
                } else if (action == GLFW_RELEASE) {
                    terrainEditor->OnMouseUp(button);
                }
            }
            break;
        case EditorMode::MODEL_PLACEMENT:
        case EditorMode::MODEL_EDIT:
            if (modelEditor) {
                if (action == GLFW_PRESS) {
                    modelEditor->OnMouseDown(button, xpos, ypos);
                } else if (action == GLFW_RELEASE) {
                    modelEditor->OnMouseUp(button);
                }
            }
            break;
    }
}

void MapEditor::OnScroll(double xoffset, double yoffset) {
    if (camera) {
        camera->Zoom(yoffset);
    }
    
    switch (currentMode) {
        case EditorMode::TERRAIN:
            if (terrainEditor) {
                terrainEditor->OnScroll(yoffset);
            }
            break;
    }
}

void MapEditor::OnKey(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                isRunning = false;
                break;
            case GLFW_KEY_1:
                SetMode(EditorMode::TERRAIN);
                break;
            case GLFW_KEY_2:
                SetMode(EditorMode::MODEL_PLACEMENT);
                break;
            case GLFW_KEY_3:
                SetMode(EditorMode::MODEL_EDIT);
                break;
            case GLFW_KEY_4:
                SetMode(EditorMode::CAMERA);
                break;
        }
    }
    
    if (modelEditor) {
        modelEditor->OnKey(key, action);
    }
}

bool MapEditor::LoadMap(const std::string& filename) {
    if (scene) {
        return scene->Load(filename);
    }
    return false;
}

bool MapEditor::SaveMap(const std::string& filename) {
    if (scene) {
        return scene->Save(filename);
    }
    return false;
}

bool MapEditor::ExportMap(const std::string& filename) {
    // Implementation for exporting to game format
    return SaveMap(filename);
} 