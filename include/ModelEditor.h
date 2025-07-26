#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// Forward declarations
class GameObject;
class Model;
class Camera;
class Scene;

enum class ModelTool {
    PLACE,
    SELECT,
    MOVE,
    ROTATE,
    SCALE,
    DELETE
};

enum class TransformMode {
    LOCAL,
    WORLD
};

class ModelEditor {
public:
    ModelEditor(Scene* scene);
    ~ModelEditor();
    
    // Tool management
    void SetTool(ModelTool tool);
    void SetTransformMode(TransformMode mode);
    
    ModelTool GetCurrentTool() const { return currentTool; }
    TransformMode GetTransformMode() const { return transformMode; }
    
    // Model placement and editing
    void OnMouseDown(int button, double x, double y);
    void OnMouseMove(double x, double y);
    void OnMouseUp(int button);
    void OnKey(int key, int action);
    
    // Model operations
    GameObject* PlaceModel(const std::string& modelPath, double x, double y);
    void SelectModel(GameObject* obj);
    void DeselectAll();
    void DeleteSelected();
    
    // Transform operations
    void MoveSelected(const glm::vec3& delta);
    void RotateSelected(const glm::vec3& delta);
    void ScaleSelected(const glm::vec3& delta);
    void SetSelectedTransform(const glm::mat4& transform);
    
    // Selection
    GameObject* GetSelectedObject() const { return selectedObject; }
    const std::vector<GameObject*>& GetSelectedObjects() const { return selectedObjects; }
    void AddToSelection(GameObject* obj);
    void RemoveFromSelection(GameObject* obj);
    
    // Model library
    void LoadModelLibrary(const std::string& directory);
    void AddModelToLibrary(const std::string& path, const std::string& name);
    void RemoveModelFromLibrary(const std::string& name);
    const std::vector<std::string>& GetModelLibrary() const { return modelLibrary; }
    
    // Transform gizmo
    void DrawTransformGizmo();
    bool IsGizmoHovered() const;
    void HandleGizmoInteraction(double x, double y);
    
    // Snapping
    void SetSnapToGrid(bool enabled);
    void SetGridSize(float size);
    void SetSnapToAngle(bool enabled);
    void SetAngleSnap(float angle);
    
    bool GetSnapToGrid() const { return snapToGrid; }
    float GetGridSize() const { return gridSize; }
    bool GetSnapToAngle() const { return snapToAngle; }
    float GetAngleSnap() const { return angleSnap; }
    
    // Duplication
    void DuplicateSelected();
    void CloneSelected();
    
    // Grouping
    void GroupSelected();
    void UngroupSelected();
    
    // Undo/Redo
    void Undo();
    void Redo();
    bool CanUndo() const;
    bool CanRedo() const;

private:
    // Helper functions
    GameObject* PickObject(double x, double y);
    glm::vec3 GetWorldPosition(double x, double y);
    glm::vec3 SnapToGrid(const glm::vec3& position);
    float SnapToAngle(float angle);
    void UpdateTransformGizmo();
    
    // Model editing state
    ModelTool currentTool;
    TransformMode transformMode;
    bool isEditing;
    
    // Selection state
    GameObject* selectedObject;
    std::vector<GameObject*> selectedObjects;
    
    // Transform gizmo
    struct Gizmo {
        glm::mat4 transform;
        bool isVisible;
        bool isDragging;
        int activeAxis; // 0=X, 1=Y, 2=Z, 3=XY, 4=YZ, 5=XZ
    } gizmo;
    
    // Snapping settings
    bool snapToGrid;
    float gridSize;
    bool snapToAngle;
    float angleSnap;
    
    // Model library
    std::vector<std::string> modelLibrary;
    std::unordered_map<std::string, std::string> modelPaths; // name -> path
    
    // Scene reference
    Scene* scene;
    Camera* camera;
    
    // Undo/Redo system
    struct EditorState {
        std::vector<GameObject*> selectedObjects;
        std::vector<glm::mat4> transforms;
    };
    
    std::vector<EditorState> undoStack;
    std::vector<EditorState> redoStack;
    size_t maxUndoSteps;
}; 