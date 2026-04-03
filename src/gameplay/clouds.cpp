#include "clouds.hpp"
#include "../graphics/backend.hpp"
#include "../graphics/window.hpp"
#include "../utils/instrumentor.hpp"
#include "scene_manager.hpp"
#include "../application.hpp"

void CloudsVolume::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    transform.position = {
        resourceProperties["position"][0],
        resourceProperties["position"][1],
        resourceProperties["position"][2]
    };

    transform.scale = {
        resourceProperties["scale"][0],
        resourceProperties["scale"][1],
        resourceProperties["scale"][2]
    };

    boundsMesh = app->graphicsBackend.CreateCube();
    boundsMesh.material.albedo = glm::vec3(0.0f);
    shader = &app->graphicsBackend.globalShaders.clouds;
}

void CloudsVolume::Initialize() {
    boundsMesh.material.albedo = glm::vec3(0.58f, 0.75f, 0.80f);
}

void CloudsVolume::Draw() {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.SetDepthMask(false);
    app->graphicsBackend.SetBackfaceCulling(false);
    app->graphicsBackend.BeginDrawMesh(boundsMesh, *shader, app->sceneManager.activeCamera, transform, true, true);
    app->graphicsBackend.UploadShaderUniformVec2(*shader, glm::vec2(app->windowManager.primaryWindow->width, app->windowManager.primaryWindow->height), "uScreenResolution");
    app->graphicsBackend.UploadShaderUniformMat4(*shader, app->sceneManager.activeCamera.GetViewMatrix(), "uView");
    app->graphicsBackend.UploadShaderUniformFloat(*shader, boundsMesh.material.alpha, "uAlpha");
    app->graphicsBackend.UploadShaderUniformVec3(*shader, boundsMesh.material.albedo, "uAlbedo");
    app->graphicsBackend.UseTexture3DSlot(app->graphicsBackend.globalTextures.noiseTexture3D, 0);
    app->graphicsBackend.UploadShaderUniformInt(*shader, 0, "noiseTexture");
    app->graphicsBackend.EndDrawMesh(boundsMesh);
    app->graphicsBackend.ResetTextureSlots();
    app->graphicsBackend.SetBackfaceCulling(true);
    app->graphicsBackend.SetDepthMask(true);
}

void CloudsVolume::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.DeleteMesh(boundsMesh);
}
