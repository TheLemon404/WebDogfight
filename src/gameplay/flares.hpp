#pragma once

#include "entity.hpp"
#include "../graphics/types.hpp"

#define MAX_FLARE_PARTICLES 15

class FlareParticleSystem : public ParticleSystem {
    float particleLifetimes[MAX_FLARE_PARTICLES];
    float particleRotations[MAX_FLARE_PARTICLES];
    float particleStartLifetime = 1.0f;

    public:
    float spawnTime = 0.0f;

    float scale = 4.0f;
    glm::vec3 leadVelocity = glm::vec3(0.0f);
    glm::vec3 leadPosition = glm::vec3(0.0f);

    bool emitting = true;
    bool disableBackfaceCulling = true;

    void LoadResources() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void UnloadResources() override;

    FlareParticleSystem() : ParticleSystem(MAX_FLARE_PARTICLES) {
        for(size_t i = 0; i < MAX_FLARE_PARTICLES; i++){
            particleLifetimes[i] = particleStartLifetime * ((float)i / (MAX_FLARE_PARTICLES - 1.0));
            particleRotations[i] = (float)rand() / 10.0f;
        }
    }
};

class FlareSystemEntity : public Entity {
    std::vector<FlareParticleSystem> flareParticleInstances;

    public:
    FlareSystemEntity(const std::string& name) : Entity(name) {}

    void SpawnFlare(glm::vec3 emitterPosition, glm::quat emitterRotation, glm::vec3 emitterVelocity);

    void LoadResources() override;
    void Initialize() override {};
    void Update() override;
    void Draw() override;
    void UnloadResources() override;
};
