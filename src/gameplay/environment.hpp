#pragma once

#include "../graphics/types.hpp"
#include <glm/glm.hpp>
#include <memory>

struct Environment {
    std::shared_ptr<Skybox> skybox = nullptr;
    glm::vec3 sunDirection = glm::vec3(-1,-1,-1);
    glm::vec3 sunColor;
    glm::vec3 nonCastShadowColor;
};
