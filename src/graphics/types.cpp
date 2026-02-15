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

void CloudsVolume::LoadResources() {
    boundsMesh = GraphicsBackend::CreateCube();
    boundsMesh.material.albedo = glm::vec3(0.0f);
    shader = &GraphicsBackend::globalShaders.clouds;
}

void CloudsVolume::Initialize() {
    boundsMesh.material.albedo = glm::vec3(0.58f, 0.75f, 0.80f);
}

void CloudsVolume::Draw() {
    FOX2_PROFILE_FUNCTION()

    GraphicsBackend::SetDepthMask(false);
    GraphicsBackend::SetBackfaceCulling(false);
    GraphicsBackend::BeginDrawMesh(boundsMesh, *shader, SceneManager::activeCamera, transform, true, true);
    GraphicsBackend::UploadShaderUniformVec2(*shader, glm::vec2(WindowManager::primaryWindow->width, WindowManager::primaryWindow->height), "uScreenResolution");
    GraphicsBackend::UploadShaderUniformMat4(*shader, SceneManager::activeCamera.GetViewMatrix(), "uView");
    GraphicsBackend::UploadShaderUniformVec3(*shader, SceneManager::currentScene->environment.sunDirection, "uSunDirection");
    GraphicsBackend::UploadShaderUniformFloat(*shader, boundsMesh.material.alpha, "uAlpha");
    GraphicsBackend::UploadShaderUniformVec3(*shader, boundsMesh.material.albedo, "uAlbedo");
    GraphicsBackend::EndDrawMesh(boundsMesh);
    GraphicsBackend::SetBackfaceCulling(true);
    GraphicsBackend::SetDepthMask(true);
}

void CloudsVolume::UnloadResources() {
    GraphicsBackend::DeleteMesh(boundsMesh);
}
