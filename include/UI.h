#pragma once

#include <memory>

// Forward declarations
class MapEditor;

class UI {
public:
    UI(MapEditor* editor);
    ~UI();
    
    void Update();
    void Render();
    
    // Panel management
    void ShowMainMenuBar();
    void ShowToolbar();
    void ShowViewport();
    void ShowProperties();
    void ShowHierarchy();
    void ShowTerrainPanel();
    void ShowModelPanel();
    void ShowConsole();
    
    // UI state
    bool IsViewportFocused() const { return viewportFocused; }
    bool IsViewportHovered() const { return viewportHovered; }
    
    // File dialogs
    void ShowFileDialog();
    void ShowSaveDialog();
    void ShowExportDialog();

private:
    void SetupDocking();
    void RenderMainMenuBar();
    void RenderToolbar();
    void RenderViewport();
    void RenderProperties();
    void RenderHierarchy();
    void RenderTerrainPanel();
    void RenderModelPanel();
    void RenderConsole();
    
    // UI state
    MapEditor* editor;
    bool viewportFocused;
    bool viewportHovered;
    
    // Panel visibility
    bool showProperties;
    bool showHierarchy;
    bool showTerrainPanel;
    bool showModelPanel;
    bool showConsole;
    
    // File dialog state
    bool showFileDialog;
    bool showSaveDialog;
    bool showExportDialog;
    char filePath[256];
}; 