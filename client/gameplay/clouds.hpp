#pragma once

#include "../graphics/types.hpp"
#include "entity.hpp"
#include <string.h>

class CloudsVolume : public Entity {
    Mesh boundsMesh;
    Shader* shader;

    public:
    Transform transform = Transform();
    Color color = Color();

    CloudsVolume(const std::string& name) : Entity(name) {};

    void LoadResources();
    void Initialize();
    void Update() {};
    void Draw();
    void UnloadResources();
};
