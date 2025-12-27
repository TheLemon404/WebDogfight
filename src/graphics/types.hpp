#pragma once

#include "../io/files.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cstddef>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

class Transform {
    public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 GetMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
        return model;
    }
};

class Camera {
    public:
    glm::vec3 position;
    glm::vec3 target;
    float near = 0.01f;
    float far = 1000.0f;
    float fov = 60.0f;
    float aspect = 1.0f;

    glm::mat4 GetProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspect, near, far);
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};

struct Material {
    glm::vec3 albedo = glm::vec3(1.0f);
    glm::vec3 shadowColor = glm::vec3(0.3f);
};

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

class Texture {
    public:
    unsigned int id;
    int widht;
    int height;
};

class Mesh {
    public:
    Material material;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int vertexCount;
    unsigned int indexCount;

    Mesh(unsigned int vao, unsigned int vbo, unsigned int ebo, unsigned int vertexCount, unsigned int indexCount) : vao(vao), vbo(vbo), ebo(ebo), vertexCount(vertexCount), indexCount(indexCount) {}
};
