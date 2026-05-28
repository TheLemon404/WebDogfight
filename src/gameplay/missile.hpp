#pragma once

#include "entity.hpp"
#include "../graphics/types.hpp"
#include "glm/ext/matrix_transform.hpp"

class Missile : public Entity{
    protected:
    Shader* shader = nullptr;
    Mesh* mesh = nullptr;

    public:
    Transform transform = Transform();
    glm::vec3 velocity = glm::vec3(0.0f);

    bool shouldDetonate = false;
    bool detonated = false;

    uint32_t targetNetworkId = 0;
    uint8_t networkId = 0;

    void LoadResources() override;
    void Initialize() override {};
    void Update() override;
    void Draw() override;
    void UnloadResources() override {};

    Missile(const std::string& name, uint8_t networkId) : Entity(name), networkId(networkId) {};
};
