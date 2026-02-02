#pragma once

#include "entity.hpp"
#include "../graphics/types.hpp"

struct TerrainAssets {
    std::string shader;
    std::string heightmap;
    std::string discolorationmap;
};

struct TerrainResource {
    TerrainAssets assets;
};

class Terrain : public Entity {
    Mesh mesh;
    Shader* shader;
    Transform transform = Transform();
    TerrainResource resource;
    Texture heightMap;
    Texture discolorationMap;

    const std::string terrainResourcePath;

    public:
    Terrain(const std::string& name, const std::string& resourcePath) : Entity(name), terrainResourcePath(resourcePath) {};

    void LoadResources() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void UnloadResources() override;
};
