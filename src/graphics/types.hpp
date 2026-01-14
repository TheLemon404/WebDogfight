#pragma once

#include "../io/files.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <cstddef>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


class Transform {
    public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::identity<glm::quat>();
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 GetMatrix() const {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);
        return translation * glm::toMat4(rotation) * sca;
    }

    void RotateLocal(glm::vec3 localAxis, float localAngle) {
        glm::quat delta = glm::angleAxis(glm::radians(localAngle), glm::normalize(localAxis));
        rotation = rotation * delta;
        rotation = glm::normalize(rotation);
    }
};

class Color {
    public:
    glm::vec4 value = glm::vec4(1.0f);
};

class Camera {
    public:
    glm::vec3 position = glm::vec3(1.0f);
    glm::vec3 target = glm::vec3(0.0f);
    float near = 0.01f;
    float far = 10000.0f;
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
    Shader() : programID(0) {}
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    //we only need to allow vertices to be effected by a single bone
    unsigned int boneID;
};

class Texture {
    public:
    unsigned int id;
    int widht;
    int height;
};

class Bone {
    public:
    glm::quat restingRotation;

    unsigned int id;
    std::string name;
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation;
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 inverseBindMatrix = glm::identity<glm::mat4>();
    int parentID = -1;

    void RotateLocal(glm::vec3 localAxis, float localAngle) {
        glm::vec3 globalAxis = rotation * localAxis;
        rotation = glm::angleAxis(localAngle, globalAxis) * rotation;
    }

    void SetLocalRotation(glm::vec3 localAxis, float localAngle) {
        glm::vec3 globalAxis = rotation * localAxis;
        rotation = glm::angleAxis(glm::radians(localAngle), globalAxis) * restingRotation;
    }

    glm::mat4 GetGlobalTransform(std::vector<Bone>& skeletalArray) {
        glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);

        if(parentID != -1) {
            for(size_t i = 0; i < skeletalArray.size(); i++){
                if(skeletalArray[i].id == parentID){
                    return skeletalArray[i].GetGlobalTransform(skeletalArray) * localTransform;
                }
            }
        }

        return localTransform;
    }
};

class Skeleton {
    public:
    std::vector<Bone> bones;
};

class Mesh {
    public:
    Material material;

    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;
    unsigned int vertexCount = 0;
    unsigned int indexCount = 0;

    Mesh() {}
    Mesh(unsigned int vao, unsigned int vbo, unsigned int ebo, unsigned int vertexCount, unsigned int indexCount) : vao(vao), vbo(vbo), ebo(ebo), vertexCount(vertexCount), indexCount(indexCount) {}
};

class SkeletalMesh : public Mesh {
    public:
    Skeleton skeleton;

    SkeletalMesh() {}
    SkeletalMesh(unsigned int vao, unsigned int vbo, unsigned int ebo, unsigned int vertexCount, unsigned int indexCount) : Mesh(vao, vbo, ebo, vertexCount, indexCount) {}
};

struct Skybox {
    Mesh mesh;
    Shader shader;

    public:
    Color skyColor;
    Color horizonColor;

    Skybox();
    ~Skybox();
};
