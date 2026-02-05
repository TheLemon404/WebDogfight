#include "water.hpp"
#include "../graphics/backend.hpp"
#include "scene_manager.hpp"

#define WATER_PLANE_SIZE 100000.0f
#define WATER_LEVEL 3000.0f

void Water::LoadResources() {
    shader = &GraphicsBackend::globalShaders.water;
    mesh = GraphicsBackend::CreateQuad();
}

void Water::Initialize() {
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

    mesh.material.albedo = glm::vec3(0.4f, 0.4f, 0.8f);
    GraphicsBackend::UploadMeshData(mesh.vao, mesh.vbo, mesh.ebo, vertices, indices);
}

void Water::Update() {
}

void Water::Draw() {
    GraphicsBackend::SetBackfaceCulling(false);
    GraphicsBackend::BeginDrawMesh(mesh, *shader, SceneManager::activeCamera, transform, false);
    GraphicsBackend::EndDrawMesh(mesh);
    GraphicsBackend::SetBackfaceCulling(true);
}

void Water::UnloadResources() {
}
