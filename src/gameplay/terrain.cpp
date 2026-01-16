#include "terrain.hpp"
#include "../graphics/backend.hpp"
#include "../graphics/loader.hpp"
#include "entity.hpp"
#include "aircraft.hpp"
#include "scene_manager.hpp"
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define HEIGHT_CONSTANT 10000.0f

#define TERRAIN_RESOLUTION 100
#define TERRAIN_SIZE 100000
#define GLOBAL_UP_VECTOR {0.0f, 1.0f, 0.0f}

void Terrain::LoadResources() {
    std::cout << "Attemping to read Terrain Resource JSON file at: " << terrainResourcePath << std::endl;
    std::string resourceFileText = Files::ReadResourceString(terrainResourcePath);
    json JSON = json::parse(resourceFileText);
    resource.assets.shader = JSON["assets"]["shader"];
    resource.assets.heightmap = JSON["assets"]["heightmap"];

    shader = GraphicsBackend::CreateShader(resource.assets.shader);
    heightMap = Loader::LoadTextureFromFile(resource.assets.heightmap.c_str());
}

void Terrain::Initialize() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Build vertex positions and uvs
    for (int i = 0; i < TERRAIN_RESOLUTION; ++i) {
        for (int j = 0; j < TERRAIN_RESOLUTION; ++j) {
            float x = static_cast<float>(i) / ((float)TERRAIN_RESOLUTION - 1);
            float z = static_cast<float>(j) / ((float)TERRAIN_RESOLUTION - 1);

            glm::vec3 position = glm::vec3(x * TERRAIN_SIZE, 0, z * TERRAIN_SIZE);
            position -= glm::vec3((TERRAIN_SIZE) / 2.0, 0, (TERRAIN_SIZE) / 2.0);


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

void Terrain::Update() {
    for(std::shared_ptr<Aircraft> aircraft : SceneManager::currentScene->GetEntitiesByType<Aircraft>()) {
        glm::vec2 uv = glm::vec2(aircraft->transform.position.x, aircraft->transform.position.z) + glm::vec2(TERRAIN_SIZE / 2.0);
        uv /= glm::vec2(TERRAIN_SIZE);

        glm::ivec2 texturePixelUV = glm::ivec2(floor(uv.x * heightMap.width), floor(uv.y * heightMap.height));
        int pixelIndex = (texturePixelUV.y * heightMap.width + texturePixelUV.x) * heightMap.channels;
        unsigned int pixelValue = (unsigned int)heightMap.data[pixelIndex];
        float height = (pixelValue / 255.0f) * HEIGHT_CONSTANT;

        if(aircraft->transform.position.y <= height) {
            aircraft->transform.position.y += 6000.0;
        }
    }
}

void Terrain::Draw() {
    GraphicsBackend::BeginDrawMesh(mesh, shader, SceneManager::activeCamera, transform);
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunDirection, "uSunDirection");
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunColor, "uSunColor");
    GraphicsBackend::UploadShaderUniformInt(shader, TERRAIN_RESOLUTION, "uResolution");
    GraphicsBackend::UseTextureSlot(heightMap, 0);
    GraphicsBackend::UploadShaderUniformInt(shader, 0, "uHeightmap");
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.skybox->horizonColor.value, "uFogColor");
    GraphicsBackend::EndDrawMesh(mesh);
    GraphicsBackend::ResetTextureSlots();
}

void Terrain::UnloadResources() {
    GraphicsBackend::DeleteMesh(mesh);
    GraphicsBackend::DeleteShader(shader);
    GraphicsBackend::DeleteTexture(heightMap);
}
