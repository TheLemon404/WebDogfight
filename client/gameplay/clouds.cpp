#include "clouds.hpp"
#include "../graphics/backend.hpp"
#include "../graphics/window.hpp"
#include "../utils/instrumentor.hpp"
#include "scene_manager.hpp"

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
