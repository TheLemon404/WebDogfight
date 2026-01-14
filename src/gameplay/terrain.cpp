#include "terrain.hpp"
#include "../graphics/backend.hpp"
#include "scene_manager.hpp"
#include <vector>

#define TERRAIN_RESOLUTION 100
#define TERRAIN_SIZE 100
#define GLOBAL_UP_VECTOR {0.0f, 1.0f, 0.0f}

void Terrain::LoadResources() {
    shader = GraphicsBackend::CreateShader("resources/shaders/terrain.glsl");
}

void Terrain::Initialize() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Build vertex positions and uvs
    for (int i = 0; i < TERRAIN_RESOLUTION; ++i) {
        for (int j = 0; j < TERRAIN_RESOLUTION; ++j) {
            float x = static_cast<float>(i) / ((float)TERRAIN_RESOLUTION - 1);
            float z = static_cast<float>(j) / ((float)TERRAIN_RESOLUTION - 1);

            glm::vec3 position = glm::vec3(x * TERRAIN_SIZE, 0, z * TERRAIN_SIZE) * glm::vec3((float)TERRAIN_RESOLUTION, 1, (float)TERRAIN_RESOLUTION);
            position -= glm::vec3((TERRAIN_RESOLUTION * TERRAIN_SIZE) / 2.0, 0, (TERRAIN_RESOLUTION * TERRAIN_SIZE) / 2.0);


            vertices.push_back({
                position,
                GLOBAL_UP_VECTOR,
                {static_cast<float>(i) / (TERRAIN_RESOLUTION - 1), static_cast<float>(j) / (TERRAIN_RESOLUTION - 1)}
            });
        }
    }

    // Build indices: two triangles per quad
    for (int i = 0; i < TERRAIN_RESOLUTION - 1; ++i) {
        for (int j = 0; j < TERRAIN_RESOLUTION - 1; ++j) {
            unsigned int topLeft     = static_cast<unsigned int>(i * TERRAIN_RESOLUTION + j);
            unsigned int topRight    = topLeft + 1;
            unsigned int bottomLeft  = static_cast<unsigned int>((i + 1) * TERRAIN_RESOLUTION + j);
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);

            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(bottomRight);
        }
    }

    mesh = Mesh(0, 0, 0, vertices.size(), indices.size());
    mesh.material.albedo = glm::vec3(0.48f, 0.63f, 0.35f);
    GraphicsBackend::UploadMeshData(mesh.vao, mesh.vbo, mesh.ebo, vertices, indices);
}

void Terrain::Draw() {
    GraphicsBackend::BeginDrawMesh(mesh, shader, SceneManager::activeCamera, transform);
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunDirection, "uSunDirection");
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunColor, "uSunColor");
    GraphicsBackend::UploadShaderUniformInt(shader, TERRAIN_RESOLUTION, "uResolution");
    GraphicsBackend::EndDrawMesh(mesh);
}
