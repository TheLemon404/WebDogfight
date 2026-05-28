#pragma once

#include "entity.hpp"
#include "../graphics/types.hpp"
#include "glm/ext/matrix_transform.hpp"

class MissileTrail {
    Mesh mesh;
    Shader* shader;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float vertexStartLifetime = 0.1f;
    float trailWidth = 10.0f;
    float vertexLifetime = vertexStartLifetime;

    void GenerateMesh();
    void RecomputeMesh();

    public:
    glm::vec3 targetPosition = glm::vec3(0.0f);
    glm::quat targetRotation = glm::identity<glm::quat>();
    float gForce = 0.0f;

    unsigned int trailResolution = 15;
    float trailLength = 100.0f;

    void LoadResources();
    void Initialize();
    void Update();
    void Draw();
    void UnloadResources();
};

class Missile : public Entity{
    protected:
    MissileTrail trail;

    Shader* shader = nullptr;
    Mesh* mesh = nullptr;

    public:
    Transform transform = Transform();
    glm::vec3 velocity = glm::vec3(0.0f);

    bool shouldDetonate = false;
    bool detonated = false;

    uint32_t launcherNetworkId = 0;
    uint32_t targetNetworkId = 0;
    uint8_t networkId = 0;

    void LoadResources() override;

    void Initialize() override;

    void Update() override;
    void Draw() override;
    void UnloadResources() override {
        trail.UnloadResources();
    };

    Missile(const std::string& name, uint8_t networkId) : Entity(name), networkId(networkId) {};
};
