# Collect source files
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# Build the graphical version with ImGui, Shader, ModernRenderer, and MapData
set(SOURCES GraphicalMapEditor.cpp Shader.cpp ModernRenderer.cpp MapData.cpp ${IMGUI_SOURCES})

# Create executable
add_executable(GraphicalMapEditor ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(GraphicalMapEditor 
    OpenGL::GL
)

# Link optional libraries if found
if(glfw3_FOUND)
    target_link_libraries(GraphicalMapEditor glfw)
endif()

if(glm_FOUND)
    target_link_libraries(GraphicalMapEditor glm::glm)
endif()

if(assimp_FOUND)
    target_link_libraries(GraphicalMapEditor assimp::assimp)
endif()

# Set properties
set_target_properties(GraphicalMapEditor PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
) 