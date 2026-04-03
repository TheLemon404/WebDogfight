#pragma once

#include "../graphics/types.hpp"
#include "entity.hpp"
#include <string.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class CloudsVolume : public Entity {
    Mesh boundsMesh;
    Shader* shader;

    public:
    Transform transform = Transform();
    Color color = Color();

    const json resourceProperties;

    CloudsVolume(const std::string& name, const json& resourceProperties) : Entity(name), resourceProperties(resourceProperties) {};

    void LoadResources();
    void Initialize();
    void Update() {};
    void Draw();
    void UnloadResources();
};
