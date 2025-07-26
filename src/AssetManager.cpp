#include "AssetManager.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

// Static member initialization
const std::vector<std::string> AssetManager::CATEGORIES = {
    "walls", "furniture", "props", "stairs", "tables", "torches", "pillars", "floors", "trunks"
};

const std::vector<std::string> AssetManager::ASSET_TYPES = {
    "fbx", "obj", "texture", "png", "jpg"
};

AssetManager::AssetManager() : assetsDirectory("../../assets") {
}

AssetManager::~AssetManager() {
    // Clean up loaded models
    for (auto& pair : loadedModels) {
        if (pair.second) {
            // Assimp handles cleanup automatically
        }
    }
}

bool AssetManager::Initialize() {
    try {
        ScanAssetsDirectory();
        std::cout << "AssetManager initialized with " << assets.size() << " assets" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize AssetManager: " << e.what() << std::endl;
        return false;
    }
}

void AssetManager::ScanAssetsDirectory() {
    assets.clear();
    
    try {
        std::filesystem::path assetsPath(assetsDirectory);
        if (!std::filesystem::exists(assetsPath)) {
            std::cerr << "Assets directory not found: " << assetsDirectory << std::endl;
            return;
        }
        
        // Scan subdirectories
        for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string filepath = entry.path().string();
                std::string extension = entry.path().extension().string();
                
                // Convert extension to lowercase
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                // Skip non-asset files
                if (extension != ".fbx" && extension != ".obj" && extension != ".png" && extension != ".jpg") {
                    continue;
                }
                
                // Get category and type
                std::string category = GetAssetCategory(filename);
                std::string type = GetAssetType(filename);
                
                // Create asset info
                std::string assetName = entry.path().stem().string();
                assets.emplace_back(assetName, filepath, category, type);
            }
        }
        
        std::cout << "Discovered " << assets.size() << " assets" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error scanning assets directory: " << e.what() << std::endl;
    }
}

std::string AssetManager::GetAssetCategory(const std::string& filename) {
    std::string lowerFilename = filename;
    std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);
    
    if (lowerFilename.find("wall") != std::string::npos) return "walls";
    if (lowerFilename.find("table") != std::string::npos) return "furniture";
    if (lowerFilename.find("torch") != std::string::npos) return "props";
    if (lowerFilename.find("stairs") != std::string::npos) return "stairs";
    if (lowerFilename.find("pillar") != std::string::npos) return "pillars";
    if (lowerFilename.find("floor") != std::string::npos) return "floors";
    if (lowerFilename.find("trunk") != std::string::npos) return "trunks";
    if (lowerFilename.find("shelf") != std::string::npos) return "furniture";
    if (lowerFilename.find("key") != std::string::npos) return "props";
    if (lowerFilename.find("sword") != std::string::npos) return "props";
    if (lowerFilename.find("plate") != std::string::npos) return "props";
    if (lowerFilename.find("keg") != std::string::npos) return "props";
    if (lowerFilename.find("rubble") != std::string::npos) return "props";
    if (lowerFilename.find("stool") != std::string::npos) return "furniture";
    
    return "misc";
}

std::string AssetManager::GetAssetType(const std::string& filename) {
    std::string lowerFilename = filename;
    std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);
    
    if (lowerFilename.find(".fbx") != std::string::npos) return "fbx";
    if (lowerFilename.find(".obj") != std::string::npos) return "obj";
    if (lowerFilename.find(".png") != std::string::npos) return "texture";
    if (lowerFilename.find(".jpg") != std::string::npos) return "texture";
    
    return "unknown";
}

bool AssetManager::LoadAsset(const std::string& assetName) {
    AssetInfo* asset = GetAssetInfo(assetName);
    if (!asset) {
        std::cerr << "Asset not found: " << assetName << std::endl;
        return false;
    }
    
    if (asset->loaded) {
        return true; // Already loaded
    }
    
    if (asset->type == "fbx" || asset->type == "obj") {
        return LoadModel(asset->filepath);
    } else if (asset->type == "texture") {
        return LoadTexture(asset->filepath);
    }
    
    return false;
}

bool AssetManager::LoadAllAssets() {
    bool success = true;
    for (auto& asset : assets) {
        if (!LoadAsset(asset.name)) {
            success = false;
        }
    }
    return success;
}

std::vector<AssetInfo> AssetManager::GetAssetsByCategory(const std::string& category) {
    std::vector<AssetInfo> result;
    for (const auto& asset : assets) {
        if (asset.category == category) {
            result.push_back(asset);
        }
    }
    return result;
}

std::vector<AssetInfo> AssetManager::GetAssetsByType(const std::string& type) {
    std::vector<AssetInfo> result;
    for (const auto& asset : assets) {
        if (asset.type == type) {
            result.push_back(asset);
        }
    }
    return result;
}

AssetInfo* AssetManager::GetAssetInfo(const std::string& assetName) {
    for (auto& asset : assets) {
        if (asset.name == assetName) {
            return &asset;
        }
    }
    return nullptr;
}

bool AssetManager::LoadModel(const std::string& filepath) {
    try {
        const aiScene* scene = importer.ReadFile(filepath, 
            aiProcess_Triangulate | 
            aiProcess_GenNormals | 
            aiProcess_CalcTangentSpace);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Failed to load model: " << filepath << std::endl;
            return false;
        }
        
        // Store the loaded scene
        std::string modelName = std::filesystem::path(filepath).stem().string();
        loadedModels[modelName] = const_cast<aiScene*>(scene);
        
        std::cout << "Loaded model: " << modelName << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        return false;
    }
}

void AssetManager::RenderModel(const std::string& modelName, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    (void)model; (void)view; (void)projection; // Unused parameters for now
    
    auto it = loadedModels.find(modelName);
    if (it == loadedModels.end()) {
        return;
    }
    
    const aiScene* scene = it->second;
    if (!scene) return;
    
    // For now, just render the first mesh as a simple cube placeholder
    // TODO: Implement proper mesh rendering with VAOs/VBOs
    if (scene->mNumMeshes > 0) {
        const aiMesh* mesh = scene->mMeshes[0];
        
        // Render as a simple bounding box for now
        // This will be replaced with proper mesh rendering
        std::cout << "Rendering model: " << modelName << " with " << mesh->mNumVertices << " vertices" << std::endl;
    }
}

bool AssetManager::LoadTexture(const std::string& filepath) {
    // TODO: Implement texture loading with stb_image or similar
    std::cout << "Texture loading not yet implemented: " << filepath << std::endl;
    return false;
}

unsigned int AssetManager::GetTexture(const std::string& textureName) {
    auto it = loadedTextures.find(textureName);
    if (it != loadedTextures.end()) {
        return it->second;
    }
    return 0;
} 