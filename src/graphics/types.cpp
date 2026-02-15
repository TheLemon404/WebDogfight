#include "types.hpp"
#include "backend.hpp"
#include "glm/matrix.hpp"
#include "loader.hpp"
#include "../utils/math.hpp"
#include "../gameplay/scene_manager.hpp"
#include "../utils/instrumentor.hpp"
#include "window.hpp"

void Skeleton::UpdateGlobalBoneTransforms()  {
    cachedGlobalBoneTransforms.resize(bones.size());

    for(size_t i = 0; i < bones.size(); i++) {
        glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), bones[i].position) * glm::toMat4(bones[i].rotation) * glm::scale(glm::mat4(1.0f), bones[i].scale);

        if(bones[i].parentID != -1) {
            cachedGlobalBoneTransforms[i] = cachedGlobalBoneTransforms[bones[i].parentID] * localTransform;
        }
        else {
            cachedGlobalBoneTransforms[i] = localTransform;
        }
    }
}

Skybox::Skybox() {
    mesh = Loader::LoadMeshFromGLTF("resources/meshes/sphere.gltf");
    shader = &GraphicsBackend::globalShaders.skybox;
    skyColor.value = glm::vec4(0.68f, 0.85f, 0.90f, 1.0);
    horizonColor.value = glm::vec4(0.9f, 1.0f, 1.0f, 1.0f);
}

Skybox::~Skybox() {
    GraphicsBackend::DeleteMesh(mesh);
}
