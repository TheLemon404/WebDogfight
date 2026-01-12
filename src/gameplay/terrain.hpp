#pragma once

#include "entity.hpp"
#include "../graphics/types.hpp"

class Terrain : public Entity {
    Mesh mesh;
    Shader shader;
    Transform transform = Transform();

    public:
    Terrain(const std::string& name) : Entity(name) {};

    void LoadResources() override;
    void Initialize() override;
    void Update() override {};
    void Draw() override;
    void UnloadResources() override {};
};
