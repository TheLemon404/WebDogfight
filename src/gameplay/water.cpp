#include "water.hpp"
#include "../graphics/backend.hpp"
#include "scene_manager.hpp"
#include "../utils/instrumentor.hpp"
#include "aircraft.hpp"
#include "../application.hpp"

#define WATER_PLANE_SIZE 100000.0f
#define WATER_LEVEL 3000.0f

void Water::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    shader = &app->graphicsBackend.globalShaders.water;
    mesh = app->graphicsBackend.CreateQuad();
    noiseTexture = Loader::LoadTextureFromFile("resources/textures/waterNormal.png");
}

void Water::Initialize() {
    FOX2_PROFILE_FUNCTION();

    std::unique_ptr<Application>& app = Application::GetInstance();

    std::vector<Vertex> vertices = {
        {{-WATER_PLANE_SIZE, WATER_LEVEL, -WATER_PLANE_SIZE}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 0
        {{ WATER_PLANE_SIZE, WATER_LEVEL, -WATER_PLANE_SIZE}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // 1
        {{ WATER_PLANE_SIZE, WATER_LEVEL, WATER_PLANE_SIZE}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 2
        {{-WATER_PLANE_SIZE, WATER_LEVEL, WATER_PLANE_SIZE}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // 3
    };

    std::vector<unsigned int> indices = {
        // Front face
        0, 1, 2,
        2, 3, 0,
    };

    mesh.material.albedo = glm::vec3(0.8f, 0.9f, 1.0f);
    app->graphicsBackend.UploadMeshData(mesh.vao, mesh.vbo, mesh.ebo, vertices, indices);
}

void Water::Update() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    for(std::shared_ptr<Aircraft> aircraft : app->sceneManager.currentScene->GetEntitiesByType<Aircraft>()) {
        if(aircraft->transform.position.y < WATER_LEVEL) {
            aircraft->exploded = true;
        }
    }
}

void Water::Draw() {
    FOX2_PROFILE_FUNCTION();
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.SetBackfaceCulling(false);
    app->graphicsBackend.BeginDrawMesh(mesh, *shader, app->sceneManager.activeCamera, transform, false);
    app->graphicsBackend.UploadShaderUniformVec3(*shader, app->sceneManager.currentScene->environment.skybox->horizonColor.value, "uFogColor");
    app->graphicsBackend.EndDrawMesh(mesh);
    app->graphicsBackend.SetBackfaceCulling(true);
}

void Water::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.DeleteMesh(mesh);
    app->graphicsBackend.DeleteTexture(noiseTexture);
}
