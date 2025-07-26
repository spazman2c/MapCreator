#include "MapEditor.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        MapEditor editor;
        
        if (!editor.Initialize()) {
            std::cerr << "Failed to initialize map editor" << std::endl;
            return -1;
        }
        
        std::cout << "Cryptic Dungeon Map Editor initialized successfully" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  1 - Terrain Editor Mode" << std::endl;
        std::cout << "  2 - Model Placement Mode" << std::endl;
        std::cout << "  3 - Model Edit Mode" << std::endl;
        std::cout << "  4 - Camera Mode" << std::endl;
        std::cout << "  ESC - Exit" << std::endl;
        
        editor.Run();
        
        std::cout << "Map editor shutdown complete" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return -1;
    }
} 