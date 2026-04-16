#pragma once

#include "../graphics/types.hpp"
#include "entity.hpp"
#include <glm/glm.hpp>
#include <vector>

class Explosion {
    Mesh* mesh;
    Shader* shader;
    float lifetime = 1.0f;
    Transform transform = Transform();
    float spawnTimestamp = 0.0f;
    float duration = 0.0f;
    float spawnScale = 0.0f;
    float t = 0.0f;

    public:
    Explosion(glm::vec3 position, float scale, float duration);

    void Update();
    void Draw();
};

class ExplosionSystemEntity : public Entity {
    std::vector<Explosion*> activeExplosions;
    public:
    ExplosionSystemEntity(const std::string& name) : Entity(name) {};

    void SpawnExplosion(glm::vec3 position, float scale, float duration);

    void LoadResources() override {};
    void Initialize() override {};
    void Update() override;
    void Draw() override;
    void UnloadResources() override {};
};
