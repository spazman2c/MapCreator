# Cryptic Dungeon Map Editor

A comprehensive 3D map editor for the Cryptic Dungeon game engine, built with modern C++ and OpenGL. This editor provides powerful tools for terrain editing, model placement, and scene management.

## Features

### 🏔️ Terrain Editor
- **Height Editing**: Raise, lower, smooth, and flatten terrain
- **Brush System**: Multiple brush types (circle, square, soft) with customizable size and strength
- **Texture Painting**: Paint different textures onto terrain surfaces
- **Noise Generation**: Procedural terrain generation with noise algorithms
- **Undo/Redo**: Full undo/redo system for terrain modifications

### 🏗️ Model Editor
- **Model Placement**: Place 3D models with precise positioning
- **Transform Tools**: Move, rotate, and scale models with intuitive controls
- **Selection System**: Multi-select and group objects
- **Snapping**: Grid and angle snapping for precise placement
- **Model Library**: Organize and manage your model collection

### 🎥 Camera Controls
- **Orbit Camera**: Smooth orbital camera with mouse controls
- **Pan & Zoom**: Intuitive navigation with keyboard and mouse
- **Focus System**: Quickly focus on selected objects
- **Multiple Views**: Support for perspective and orthographic projections

### 🖥️ User Interface
- **Dockable Panels**: Fully customizable interface with dockable panels
- **Real-time Properties**: Edit object properties in real-time
- **Hierarchy View**: Scene hierarchy with object management
- **Console**: Built-in console for debugging and commands
- **File Management**: Load, save, and export map files

## System Requirements

- **Operating System**: macOS 10.15 or later
- **Compiler**: Clang/LLVM with C++17 support
- **Graphics**: OpenGL 4.1 or later
- **Memory**: 8GB RAM minimum, 16GB recommended
- **Storage**: 2GB free space

## Dependencies

- **GLFW**: Window management and input handling
- **OpenGL**: Graphics rendering
- **GLM**: Mathematics library
- **Assimp**: 3D model loading
- **ImGui**: User interface
- **GLAD**: OpenGL extension loading

## Installation

### Prerequisites

1. Install Xcode Command Line Tools:
```bash
xcode-select --install
```

2. Install Homebrew (if not already installed):
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

3. Install dependencies:
```bash
brew install cmake glfw glm assimp
```

### Building from Source

1. Clone the repository:
```bash
git clone https://github.com/yourusername/CrypticDungeon.git
cd CrypticDungeon
```

2. Create build directory and compile:
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

3. Run the editor:
```bash
./MapEditor
```

## Usage

### Basic Controls

| Key | Action |
|-----|--------|
| `1` | Switch to Terrain Editor |
| `2` | Switch to Model Placement |
| `3` | Switch to Model Edit |
| `4` | Switch to Camera Mode |
| `W/A/S/D` | Camera movement (in camera mode) |
| `Mouse` | Orbit camera |
| `Scroll` | Zoom camera |
| `ESC` | Exit application |

### Terrain Editing

1. **Select Terrain Tool**: Press `1` or use the toolbar
2. **Choose Brush**: Select brush type and size in the Terrain Panel
3. **Edit Terrain**: Click and drag to modify terrain height
4. **Adjust Settings**: Use the Properties panel to fine-tune brush parameters

### Model Placement

1. **Switch to Placement Mode**: Press `2` or use the toolbar
2. **Select Model**: Choose from the Model Library panel
3. **Place Model**: Click in the viewport to place the model
4. **Transform**: Use the transform gizmo to position and orient the model

### Camera Navigation

1. **Switch to Camera Mode**: Press `4` or use the toolbar
2. **Orbit**: Hold left mouse button and drag to orbit around the target
3. **Pan**: Hold middle mouse button and drag to pan the view
4. **Zoom**: Use mouse scroll wheel to zoom in/out

## File Formats

### Supported Input Formats
- **Models**: OBJ, FBX, DAE, 3DS, BLEND
- **Textures**: PNG, JPG, TGA, BMP
- **Heightmaps**: PNG, RAW, TIFF

### Export Formats
- **Map Files**: Custom binary format (.map)
- **Heightmaps**: PNG, RAW
- **Scene Files**: JSON, XML

## Project Structure

```
CrypticDungeon/
├── include/           # Header files
│   ├── MapEditor.h    # Main editor class
│   ├── Camera.h       # Camera system
│   ├── Scene.h        # Scene management
│   ├── TerrainEditor.h # Terrain editing
│   ├── ModelEditor.h  # Model editing
│   └── UI.h          # User interface
├── src/              # Source files
│   ├── main.cpp      # Application entry point
│   ├── MapEditor.cpp # Main editor implementation
│   ├── Camera.cpp    # Camera implementation
│   ├── Scene.cpp     # Scene implementation
│   ├── TerrainEditor.cpp # Terrain editor
│   ├── ModelEditor.cpp # Model editor
│   └── UI.cpp        # UI implementation
├── assets/           # Default assets
│   ├── models/       # 3D models
│   ├── textures/     # Textures
│   └── shaders/      # GLSL shaders
├── CMakeLists.txt    # CMake configuration
├── package.json      # Project metadata
└── README.md         # This file
```

## Development

### Adding New Features

1. **Create Header**: Add class declaration in `include/`
2. **Implement**: Add implementation in `src/`
3. **Update CMake**: Add new files to `CMakeLists.txt`
4. **Test**: Build and test the new feature

### Code Style

- Use modern C++17 features
- Follow RAII principles
- Use smart pointers for memory management
- Keep functions small and focused
- Add comprehensive comments for complex logic

### Debugging

- Use the built-in console for debugging output
- Enable debug builds with `cmake -DCMAKE_BUILD_TYPE=Debug`
- Use Xcode for advanced debugging on macOS

## Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Make your changes and test thoroughly
4. Commit your changes: `git commit -m 'Add feature'`
5. Push to the branch: `git push origin feature-name`
6. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For support and questions:
- Create an issue on GitHub
- Check the documentation in the `docs/` folder
- Join our Discord community

## Roadmap

### Version 1.1
- [ ] Advanced terrain generation algorithms
- [ ] Vegetation system
- [ ] Water and fluid simulation
- [ ] Lighting system improvements

### Version 1.2
- [ ] Multiplayer collaboration
- [ ] Plugin system
- [ ] Advanced material editor
- [ ] Animation support

### Version 1.3
- [ ] VR support
- [ ] Real-time rendering improvements
- [ ] Advanced particle systems
- [ ] AI-assisted editing tools 