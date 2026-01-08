#pragma once

#include "entity.hpp"

class Terrain : public Entity {
    public:
    Terrain(const std::string& name) : Entity(name) {};

    void LoadResources() override {};
    void Initialize() override;
    void Update() override {};
    void Draw() override;
    void UnloadResources() override {};
};
