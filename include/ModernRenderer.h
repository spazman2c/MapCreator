#pragma once

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "MapData.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

class ModernRenderer {
public:
    ModernRenderer();
    ~ModernRenderer();

    bool Initialize();
    void RenderTerrain(const glm::mat4& view, const glm::mat4& projection);
    void RenderCube(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    void RenderGrid(const glm::mat4& view, const glm::mat4& projection);
    void RenderMapObjects(const MapData& mapData, const glm::mat4& view, const glm::mat4& projection);

private:
    void CreateTerrainVAO();
    void CreateCubeVAO();
    void CreateGridVAO();
    
    std::unique_ptr<Shader> basicShader;
    
    // Terrain rendering
    unsigned int terrainVAO, terrainVBO;
    std::vector<Vertex> terrainVertices;
    
    // Cube rendering
    unsigned int cubeVAO, cubeVBO;
    std::vector<Vertex> cubeVertices;
    
    // Grid rendering
    unsigned int gridVAO, gridVBO;
    std::vector<Vertex> gridVertices;
}; 