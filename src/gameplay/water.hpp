#pragma once

#include "entity.hpp"
#include "../graphics/types.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Water : public Entity {
    Mesh mesh;
    Shader* shader;
    Transform transform = Transform();

    Texture noiseTexture;

    const json resourceProperties;

    public:
    Water(const std::string& name, const json& resourceProperties) : Entity(name), resourceProperties(resourceProperties) {};

    void LoadResources() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void UnloadResources() override;
};
