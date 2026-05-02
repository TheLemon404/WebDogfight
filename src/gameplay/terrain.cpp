#include "terrain.hpp"
#include "../graphics/backend.hpp"
#include "../graphics/loader.hpp"
#include "../utils/instrumentor.hpp"
#include "aircraft.hpp"
#include "scene_manager.hpp"
#include <vector>
#include "../application.hpp"

#define MAX_AIRCRAFT_HEIGHT 30000.0f

#define HEIGHT_CONSTANT 10000.0f

#define TERRAIN_RESOLUTION 100
#define GLOBAL_UP_VECTOR {0.0f, 1.0f, 0.0f}

void Terrain::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    resource.assets.shader = resourceProperties["assets"]["shader"];
    resource.assets.heightmap = resourceProperties["assets"]["heightmap"];
    resource.assets.discolorationmap = resourceProperties["assets"]["discolorationmap"];
    resource.settings.heightFactor = resourceProperties["settings"]["heightFactor"];
    resource.settings.topColor = {resourceProperties["settings"]["top-color"][0], resourceProperties["settings"]["top-color"][1], resourceProperties["settings"]["top-color"][2]};
    resource.settings.middleColor = {resourceProperties["settings"]["middle-color"][0], resourceProperties["settings"]["middle-color"][1], resourceProperties["settings"]["middle-color"][2]};
    resource.settings.slopeColor = {resourceProperties["settings"]["slope-color"][0], resourceProperties["settings"]["slope-color"][1], resourceProperties["settings"]["slope-color"][2]};
    resource.settings.baseColor = {resourceProperties["settings"]["base-color"][0], resourceProperties["settings"]["base-color"][1], resourceProperties["settings"]["base-color"][2]};

    shader = &app->graphicsBackend.globalShaders.terrain;
    heightMap = Loader::LoadTextureFromFile(resource.assets.heightmap.c_str());
    discolorationMap = Loader::LoadTextureFromFile(resource.assets.discolorationmap.c_str());
}

void Terrain::Initialize() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Build vertex positions and uvs
    for (int i = 0; i < TERRAIN_RESOLUTION; ++i) {
        for (int j = 0; j < TERRAIN_RESOLUTION; ++j) {
            float x = static_cast<float>(i) / ((float)TERRAIN_RESOLUTION - 1);
            float z = static_cast<float>(j) / ((float)TERRAIN_RESOLUTION - 1);

            glm::vec2 uv = glm::vec2(x, z);
            glm::ivec2 texturePixelUV = glm::ivec2(floor(uv.x * heightMap.width), floor(uv.y * heightMap.height));
            int pixelIndex = (texturePixelUV.y * heightMap.width + texturePixelUV.x) * heightMap.channels;
            unsigned int pixelValue = (unsigned int)heightMap.data[pixelIndex];

            float height = (pixelValue / 255.0f) * HEIGHT_CONSTANT * resource.settings.heightFactor;
            glm::vec3 position = glm::vec3(x * TERRAIN_SIZE, height, z * TERRAIN_SIZE);
            position -= glm::vec3((TERRAIN_SIZE) / 2.0, 0, (TERRAIN_SIZE) / 2.0);

            vertices.push_back({
                position,
                GLOBAL_UP_VECTOR,
                {static_cast<float>(i) / (TERRAIN_RESOLUTION - 1), static_cast<float>(j) / (TERRAIN_RESOLUTION - 1)}
            });
        }
    }

    for(int i = 0; i < TERRAIN_RESOLUTION; i++) {
        for(int j = 0; j < TERRAIN_RESOLUTION; j++) {
            glm::vec3 position = vertices[i * TERRAIN_RESOLUTION + j].position;
            glm::vec3 leftPosition = i + 1 < TERRAIN_RESOLUTION ? vertices[(i + 1) * TERRAIN_RESOLUTION + j].position : position;
            glm::vec3 upPosition = j + 1 < TERRAIN_RESOLUTION ? vertices[i * TERRAIN_RESOLUTION + j + 1].position : position;

            glm::vec3 normal = glm::normalize(glm::cross(upPosition - position, leftPosition - position));
            vertices[i * TERRAIN_RESOLUTION + j].normal = normal;
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
    app->graphicsBackend.UploadMeshData(mesh.vao, mesh.vbo, mesh.ebo, vertices, indices);
}

void Terrain::Update() {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

    for(std::shared_ptr<Aircraft> aircraft : app->sceneManager.currentScene->GetEntitiesByType<Aircraft>()) {
        glm::vec2 uv = glm::vec2(aircraft->transform.position.x, aircraft->transform.position.z) + glm::vec2(TERRAIN_SIZE / 2.0);
        uv /= glm::vec2(TERRAIN_SIZE);

        glm::ivec2 texturePixelUV = glm::ivec2(floor(uv.x * heightMap.width), floor(uv.y * heightMap.height));
        int pixelIndex = (texturePixelUV.y * heightMap.width + texturePixelUV.x) * heightMap.channels;
        unsigned int pixelValue = (unsigned int)heightMap.data[pixelIndex];
        float height = (pixelValue / 255.0f) * HEIGHT_CONSTANT * resource.settings.heightFactor;

        //boundaries and terrain collision
        if(aircraft->transform.position.y < height) {
            aircraft->exploded = true;
        }
        else if(aircraft->transform.position.y > MAX_AIRCRAFT_HEIGHT) {
            aircraft->transform.position.y = 6000.0;
        }

        if(uv.x > 1.0f || uv.x < 0.0f) {
            aircraft->transform.position.x = 0.0f;
        }

        if(uv.y > 1.0f || uv.y < 0.0f) {
            aircraft->transform.position.z = 0.0f;
        }

    }
}

void Terrain::Draw() {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.BeginDrawMesh(mesh, *shader, app->sceneManager.activeCamera, transform, false);
    app->graphicsBackend.UploadShaderUniformVec3(*shader, app->sceneManager.currentScene->environment.sunDirection, "uSunDirection");
    app->graphicsBackend.UseTextureSlot(heightMap, 0);
    app->graphicsBackend.UploadShaderUniformInt(*shader, 0, "uHeightmap");
    app->graphicsBackend.UseTextureSlot(discolorationMap, 1);
    app->graphicsBackend.UploadShaderUniformInt(*shader, 1, "uDiscolorationMap");
    app->graphicsBackend.UploadShaderUniformVec3(*shader, app->sceneManager.currentScene->environment.skybox->horizonColor.value, "uFogColor");
    app->graphicsBackend.UploadShaderUniformVec3(*shader, resource.settings.topColor, "uTopColor");
    app->graphicsBackend.UploadShaderUniformVec3(*shader, resource.settings.middleColor, "uMiddleColor");
    app->graphicsBackend.UploadShaderUniformVec3(*shader, resource.settings.slopeColor, "uSlopeColor");
    app->graphicsBackend.UploadShaderUniformVec3(*shader, resource.settings.baseColor, "uBaseColor");
    app->graphicsBackend.EndDrawMesh(mesh);
    app->graphicsBackend.ResetTextureSlots();
}

void Terrain::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.DeleteMesh(mesh);
    app->graphicsBackend.DeleteTexture(heightMap);
}
