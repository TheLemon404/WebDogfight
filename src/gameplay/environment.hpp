#pragma once

#include <glm/glm.hpp>

struct Environment {
    glm::vec3 sunDirection = glm::vec3(-1,-1,-1);
    glm::vec3 sunColor;
    glm::vec3 nonCastShadowColor;
};
