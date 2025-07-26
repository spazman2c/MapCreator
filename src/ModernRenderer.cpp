#define GL_SILENCE_DEPRECATION
#include "ModernRenderer.h"
#include <iostream>

// Include OpenGL extension headers for VAO support
#include <OpenGL/gl3.h>

ModernRenderer::ModernRenderer() 
    : terrainVAO(0), terrainVBO(0)
    , cubeVAO(0), cubeVBO(0)
    , gridVAO(0), gridVBO(0) {
}

ModernRenderer::~ModernRenderer() {
    if (terrainVAO) glDeleteVertexArrays(1, &terrainVAO);
    if (terrainVBO) glDeleteBuffers(1, &terrainVBO);
    if (cubeVAO) glDeleteVertexArrays(1, &cubeVAO);
    if (cubeVBO) glDeleteBuffers(1, &cubeVBO);
    if (gridVAO) glDeleteVertexArrays(1, &gridVAO);
    if (gridVBO) glDeleteBuffers(1, &gridVBO);
}

bool ModernRenderer::Initialize() {
    try {
        // Create and compile shader with embedded source
        const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

        const char* fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

        // Compile shaders manually
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        
        // Create shader program
        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        
        // Clean up shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        // Create a simple shader wrapper
        basicShader = std::make_unique<Shader>();
        basicShader->ID = shaderProgram;
        
        // Create VAOs and VBOs
        CreateTerrainVAO();
        CreateCubeVAO();
        CreateGridVAO();
        
        std::cout << "Modern renderer initialized successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to initialize modern renderer: " << e.what() << std::endl;
        return false;
    }
}

void ModernRenderer::CreateTerrainVAO() {
    // Create a large white terrain square
    float size = 200.0f;
    
    terrainVertices = {
        // Position (x, y, z), Color (r, g, b)
        {{-size, 0.0f, -size}, {1.0f, 1.0f, 1.0f}}, // White
        {{ size, 0.0f, -size}, {1.0f, 1.0f, 1.0f}},
        {{ size, 0.0f,  size}, {1.0f, 1.0f, 1.0f}},
        {{-size, 0.0f,  size}, {1.0f, 1.0f, 1.0f}}
    };
    
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    
    glBindVertexArray(terrainVAO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(Vertex), terrainVertices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void ModernRenderer::CreateCubeVAO() {
    float s = 5.0f; // Large cube size
    
    cubeVertices = {
        // Front face (red)
        {{-s, -s,  s}, {1.0f, 0.0f, 0.0f}},
        {{ s, -s,  s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        {{-s, -s,  s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        {{-s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        
        // Back face (red)
        {{-s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s, -s}, {1.0f, 0.0f, 0.0f}},
        {{-s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s, -s}, {1.0f, 0.0f, 0.0f}},
        {{-s,  s, -s}, {1.0f, 0.0f, 0.0f}},
        
        // Left face (red)
        {{-s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{-s,  s, -s}, {1.0f, 0.0f, 0.0f}},
        {{-s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        {{-s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{-s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        {{-s, -s,  s}, {1.0f, 0.0f, 0.0f}},
        
        // Right face (red)
        {{ s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        {{ s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        {{ s, -s,  s}, {1.0f, 0.0f, 0.0f}},
        
        // Top face (red)
        {{-s,  s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        {{-s,  s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        {{-s,  s,  s}, {1.0f, 0.0f, 0.0f}},
        
        // Bottom face (red)
        {{-s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s, -s,  s}, {1.0f, 0.0f, 0.0f}},
        {{-s, -s, -s}, {1.0f, 0.0f, 0.0f}},
        {{ s, -s,  s}, {1.0f, 0.0f, 0.0f}},
        {{-s, -s,  s}, {1.0f, 0.0f, 0.0f}}
    };
    
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(Vertex), cubeVertices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void ModernRenderer::CreateGridVAO() {
    // Create a simple grid
    for (int i = -10; i <= 10; i++) {
        // X lines
        gridVertices.push_back({{(float)i, 0.0f, -10.0f}, {0.5f, 0.5f, 0.5f}});
        gridVertices.push_back({{(float)i, 0.0f,  10.0f}, {0.5f, 0.5f, 0.5f}});
        
        // Z lines
        gridVertices.push_back({{-10.0f, 0.0f, (float)i}, {0.5f, 0.5f, 0.5f}});
        gridVertices.push_back({{ 10.0f, 0.0f, (float)i}, {0.5f, 0.5f, 0.5f}});
    }
    
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(Vertex), gridVertices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void ModernRenderer::RenderTerrain(const glm::mat4& view, const glm::mat4& projection) {
    basicShader->use();
    basicShader->setMat4("view", view);
    basicShader->setMat4("projection", projection);
    basicShader->setMat4("model", glm::mat4(1.0f));
    
    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void ModernRenderer::RenderCube(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    basicShader->use();
    basicShader->setMat4("view", view);
    basicShader->setMat4("projection", projection);
    basicShader->setMat4("model", model);
    
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size());
    glBindVertexArray(0);
}

void ModernRenderer::RenderGrid(const glm::mat4& view, const glm::mat4& projection) {
    basicShader->use();
    basicShader->setMat4("view", view);
    basicShader->setMat4("projection", projection);
    basicShader->setMat4("model", glm::mat4(1.0f));
    
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, gridVertices.size());
    glBindVertexArray(0);
} 