#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct AssetInfo {
    std::string name;
    std::string filepath;
    std::string category; // "walls", "furniture", "props", etc.
    std::string type;     // "fbx", "obj", "texture"
    bool loaded;
    
    AssetInfo(const std::string& n, const std::string& fp, const std::string& cat, const std::string& t)
        : name(n), filepath(fp), category(cat), type(t), loaded(false) {}
};

class AssetManager {
public:
    AssetManager();
    ~AssetManager();
    
    // Asset discovery and loading
    bool Initialize();
    void ScanAssetsDirectory();
    bool LoadAsset(const std::string& assetName);
    bool LoadAllAssets();
    
    // Asset querying
    std::vector<AssetInfo> GetAssetsByCategory(const std::string& category);
    std::vector<AssetInfo> GetAssetsByType(const std::string& type);
    AssetInfo* GetAssetInfo(const std::string& assetName);
    const std::vector<AssetInfo>& GetAllAssets() const { return assets; }
    
    // Model loading
    bool LoadModel(const std::string& filepath);
    void RenderModel(const std::string& modelName, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    
    // Texture loading
    bool LoadTexture(const std::string& filepath);
    unsigned int GetTexture(const std::string& textureName);
    
    // Asset categories
    static const std::vector<std::string> CATEGORIES;
    static const std::vector<std::string> ASSET_TYPES;

private:
    void DiscoverAssets();
    std::string GetAssetCategory(const std::string& filename);
    std::string GetAssetType(const std::string& filename);
    
    std::vector<AssetInfo> assets;
    std::map<std::string, unsigned int> loadedTextures;
    std::map<std::string, aiScene*> loadedModels;
    
    Assimp::Importer importer;
    std::string assetsDirectory;
}; 