#pragma once

#include "entity.hpp"
#include "../graphics/types.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define TERRAIN_SIZE 100000

struct TerrainAssets {
    std::string shader;
    std::string heightmap;
    std::string discolorationmap;
};

struct TerrainSettings {
    float heightFactor;
    glm::vec3 topColor;
    glm::vec3 middleColor;
    glm::vec3 slopeColor;
    glm::vec3 baseColor;
};

struct TerrainResource {
    TerrainAssets assets;
    TerrainSettings settings;
};

class Terrain : public Entity {
    Mesh mesh;
    Shader* shader;
    Transform transform = Transform();
    TerrainResource resource;
    Texture heightMap;
    Texture discolorationMap;

    const json resourceProperties;

    public:
    Texture& GetHeightMap() { return heightMap; }

    Terrain(const std::string& name, const json& resourceProperties) : Entity(name), resourceProperties(resourceProperties) {};

    void LoadResources() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void UnloadResources() override;
};
