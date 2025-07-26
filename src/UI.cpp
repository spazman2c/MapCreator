#include "UI.h"
#include "MapEditor.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

UI::UI(MapEditor* editor)
    : editor(editor)
    , viewportFocused(false)
    , viewportHovered(false)
    , showProperties(true)
    , showHierarchy(true)
    , showTerrainPanel(false)
    , showModelPanel(false)
    , showConsole(false)
    , showFileDialog(false)
    , showSaveDialog(false)
    , showExportDialog(false)
{
    strcpy(filePath, "");
}

UI::~UI() {
}

void UI::Update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    SetupDocking();
    
    RenderMainMenuBar();
    RenderToolbar();
    RenderViewport();
    RenderProperties();
    RenderHierarchy();
    RenderTerrainPanel();
    RenderModelPanel();
    RenderConsole();
}

void UI::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void UI::SetupDocking() {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void UI::RenderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                // TODO: Implement new map
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                showFileDialog = true;
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                showSaveDialog = true;
            }
            if (ImGui::MenuItem("Save As...")) {
                showSaveDialog = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Export", "Ctrl+E")) {
                showExportDialog = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                // TODO: Exit application
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                // TODO: Implement undo
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                // TODO: Implement redo
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X")) {
                // TODO: Implement cut
            }
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                // TODO: Implement copy
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                // TODO: Implement paste
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Properties", nullptr, &showProperties)) {}
            if (ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy)) {}
            if (ImGui::MenuItem("Terrain Panel", nullptr, &showTerrainPanel)) {}
            if (ImGui::MenuItem("Model Panel", nullptr, &showModelPanel)) {}
            if (ImGui::MenuItem("Console", nullptr, &showConsole)) {}
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Terrain Editor")) {
                editor->SetMode(EditorMode::TERRAIN);
            }
            if (ImGui::MenuItem("Model Placement")) {
                editor->SetMode(EditorMode::MODEL_PLACEMENT);
            }
            if (ImGui::MenuItem("Model Edit")) {
                editor->SetMode(EditorMode::MODEL_EDIT);
            }
            if (ImGui::MenuItem("Camera")) {
                editor->SetMode(EditorMode::CAMERA);
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void UI::RenderToolbar() {
    if (ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        if (ImGui::Button("Terrain")) {
            editor->SetMode(EditorMode::TERRAIN);
        }
        ImGui::SameLine();
        if (ImGui::Button("Place")) {
            editor->SetMode(EditorMode::MODEL_PLACEMENT);
        }
        ImGui::SameLine();
        if (ImGui::Button("Edit")) {
            editor->SetMode(EditorMode::MODEL_EDIT);
        }
        ImGui::SameLine();
        if (ImGui::Button("Camera")) {
            editor->SetMode(EditorMode::CAMERA);
        }
        
        ImGui::Separator();
        
        // Current mode display
        const char* modeNames[] = {"Terrain", "Place", "Edit", "Camera"};
        int currentMode = static_cast<int>(editor->GetCurrentMode());
        ImGui::Text("Mode: %s", modeNames[currentMode]);
        
        ImGui::End();
    }
}

void UI::RenderViewport() {
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImVec2 viewportPos = ImGui::GetCursorScreenPos();
    
    // Store viewport state
    viewportFocused = ImGui::IsWindowFocused();
    viewportHovered = ImGui::IsWindowHovered();
    
    // Render viewport content here
    ImGui::GetWindowDrawList()->AddRectFilled(
        viewportPos,
        ImVec2(viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y),
        IM_COL32(50, 50, 50, 255)
    );
    
    // Add viewport controls overlay
    if (ImGui::IsWindowHovered()) {
        ImGui::SetTooltip("Viewport - Use mouse to navigate");
    }
    
    ImGui::End();
}

void UI::RenderProperties() {
    if (!showProperties) return;
    
    ImGui::Begin("Properties", &showProperties);
    
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        static float fov = 45.0f;
        if (ImGui::SliderFloat("FOV", &fov, 10.0f, 120.0f)) {
            // TODO: Update camera FOV
        }
        
        static float nearPlane = 0.1f;
        if (ImGui::SliderFloat("Near Plane", &nearPlane, 0.01f, 10.0f)) {
            // TODO: Update camera near plane
        }
        
        static float farPlane = 1000.0f;
        if (ImGui::SliderFloat("Far Plane", &farPlane, 100.0f, 10000.0f)) {
            // TODO: Update camera far plane
        }
    }
    
    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
        static glm::vec3 ambientLight(0.2f, 0.2f, 0.2f);
        if (ImGui::ColorEdit3("Ambient Light", &ambientLight[0])) {
            // TODO: Update scene ambient light
        }
    }
    
    ImGui::End();
}

void UI::RenderHierarchy() {
    if (!showHierarchy) return;
    
    ImGui::Begin("Hierarchy", &showHierarchy);
    
    if (ImGui::TreeNode("Scene Root")) {
        // TODO: Render scene hierarchy
        ImGui::Text("Game Objects: %zu", 0); // Placeholder
        
        if (ImGui::TreeNode("Terrain")) {
            ImGui::Text("Terrain Object");
            ImGui::TreePop();
        }
        
        if (ImGui::TreeNode("Models")) {
            ImGui::Text("No models placed");
            ImGui::TreePop();
        }
        
        if (ImGui::TreeNode("Lights")) {
            ImGui::Text("No lights placed");
            ImGui::TreePop();
        }
        
        ImGui::TreePop();
    }
    
    ImGui::End();
}

void UI::RenderTerrainPanel() {
    if (!showTerrainPanel) return;
    
    ImGui::Begin("Terrain Editor", &showTerrainPanel);
    
    if (ImGui::CollapsingHeader("Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
        static int currentTool = 0;
        const char* tools[] = {"Raise/Lower", "Smooth", "Flatten", "Paint", "Noise"};
        if (ImGui::Combo("Tool", &currentTool, tools, IM_ARRAYSIZE(tools))) {
            // TODO: Set terrain tool
        }
        
        static int currentBrush = 0;
        const char* brushes[] = {"Circle", "Square", "Soft"};
        if (ImGui::Combo("Brush", &currentBrush, brushes, IM_ARRAYSIZE(brushes))) {
            // TODO: Set terrain brush
        }
    }
    
    if (ImGui::CollapsingHeader("Brush Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        static float brushSize = 10.0f;
        if (ImGui::SliderFloat("Size", &brushSize, 1.0f, 100.0f)) {
            // TODO: Update brush size
        }
        
        static float brushStrength = 0.5f;
        if (ImGui::SliderFloat("Strength", &brushStrength, 0.0f, 1.0f)) {
            // TODO: Update brush strength
        }
    }
    
    if (ImGui::CollapsingHeader("Terrain Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        static float terrainScale = 1.0f;
        if (ImGui::SliderFloat("Scale", &terrainScale, 0.1f, 10.0f)) {
            // TODO: Update terrain scale
        }
        
        static float terrainHeight = 100.0f;
        if (ImGui::SliderFloat("Height", &terrainHeight, 1.0f, 1000.0f)) {
            // TODO: Update terrain height
        }
    }
    
    ImGui::End();
}

void UI::RenderModelPanel() {
    if (!showModelPanel) return;
    
    ImGui::Begin("Model Editor", &showModelPanel);
    
    if (ImGui::CollapsingHeader("Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
        static int currentTool = 0;
        const char* tools[] = {"Place", "Select", "Move", "Rotate", "Scale", "Delete"};
        if (ImGui::Combo("Tool", &currentTool, tools, IM_ARRAYSIZE(tools))) {
            // TODO: Set model tool
        }
        
        static int transformMode = 0;
        const char* modes[] = {"Local", "World"};
        if (ImGui::Combo("Transform Mode", &transformMode, modes, IM_ARRAYSIZE(modes))) {
            // TODO: Set transform mode
        }
    }
    
    if (ImGui::CollapsingHeader("Snapping", ImGuiTreeNodeFlags_DefaultOpen)) {
        static bool snapToGrid = true;
        if (ImGui::Checkbox("Snap to Grid", &snapToGrid)) {
            // TODO: Update snap to grid
        }
        
        static float gridSize = 1.0f;
        if (ImGui::SliderFloat("Grid Size", &gridSize, 0.1f, 10.0f)) {
            // TODO: Update grid size
        }
        
        static bool snapToAngle = true;
        if (ImGui::Checkbox("Snap to Angle", &snapToAngle)) {
            // TODO: Update snap to angle
        }
        
        static float angleSnap = 15.0f;
        if (ImGui::SliderFloat("Angle Snap", &angleSnap, 1.0f, 90.0f)) {
            // TODO: Update angle snap
        }
    }
    
    if (ImGui::CollapsingHeader("Model Library", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Available Models:");
        // TODO: List available models
        
        if (ImGui::Button("Add Model")) {
            // TODO: Open file dialog to add model
        }
    }
    
    ImGui::End();
}

void UI::RenderConsole() {
    if (!showConsole) return;
    
    ImGui::Begin("Console", &showConsole);
    
    // Console output
    ImGui::BeginChild("ConsoleOutput", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
    ImGui::Text("Map Editor Console");
    ImGui::Text("Ready for commands...");
    ImGui::EndChild();
    
    // Console input
    static char inputBuffer[256] = "";
    if (ImGui::InputText("Command", inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        // TODO: Process console command
        inputBuffer[0] = '\0';
    }
    
    ImGui::End();
} 