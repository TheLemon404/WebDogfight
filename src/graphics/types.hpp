#pragma once

#include <glm/glm.hpp>

class Shader {
    public:
    unsigned int programID;

    Shader(unsigned int programID) : programID(programID) {}
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

class Mesh {
    public:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int vertexCount;

    Mesh(unsigned int vao, unsigned int vbo, unsigned int ebo, unsigned int vertexCount) : vao(vao), vbo(vbo), ebo(ebo), vertexCount(vertexCount) {}
};
