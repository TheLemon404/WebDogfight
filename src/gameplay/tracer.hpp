#pragma once

#include "entity.hpp"
#include "../graphics/types.hpp"

#define BULLET_SPEED 10000.0f

struct BulletInstanceData {
    glm::mat4 transform;
    float spawnTime;
};

class TracerSystemEntity : public Entity {
    Mesh tracerMesh;
    Shader* tracerShader;
    std::vector<BulletInstanceData> tracerInstances;

    void UpdateInstanceMeshTransforms();

    public:
    TracerSystemEntity(const std::string& name) : Entity(name) {};

    void SpawnTracer(glm::vec3 start, glm::vec3 end);

    void LoadResources() override;
    void Initialize() override {};
    void Update() override;
    void Draw() override;
    void UnloadResources() override;
};
