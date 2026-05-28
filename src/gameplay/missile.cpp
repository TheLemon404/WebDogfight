#include "missile.hpp"

#include "../application.hpp"
#include "../utils/math.hpp"

#define MISSILE_EXPLOSION_SIZE 200.0f


void MissileTrail::GenerateMesh() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    // Create 4 quads in a line along the Z-axis
    // Each quad needs 4 vertices, total = 20 vertices (5 positions × 2 sides)
    // But we can share vertices between quads for efficiency

    // Generate vertices (2 vertices per cross-section, 5 cross-sections for 4 quads)
    for(int i = 0; i < trailResolution; i++) {
        float z = i * trailLength;
        float v = i / 4.0f;  // UV coordinate along trail

        // Left vertex
        vertices.push_back({
            glm::vec3(-trailWidth / 2.0f, 0.0f, z) + targetPosition,      // Position
            {0.0f, 1.0f, 0.0f},                 // Normal (pointing up)
            {0.0f, v}                           // UV
        });

        // Right vertex
        vertices.push_back({
            glm::vec3(trailWidth / 2.0f, 0.0f, z) + targetPosition,       // Position
            {0.0f, 1.0f, 0.0f},                 // Normal (pointing up)
            {1.0f, v}                           // UV
        });
    }

    // Generate indices for 4 quads
    for(unsigned int i = 0; i < 4; i++) {
        unsigned int baseIndex = i * 2;

        // First triangle
        indices.push_back(baseIndex + 0);  // Bottom-left
        indices.push_back(baseIndex + 2);  // Top-left
        indices.push_back(baseIndex + 1);  // Bottom-right

        // Second triangle
        indices.push_back(baseIndex + 1);  // Bottom-right
        indices.push_back(baseIndex + 2);  // Top-left
        indices.push_back(baseIndex + 3);  // Top-right
    }

    app->graphicsBackend.UpdateMeshVertices(mesh, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void MissileTrail::RecomputeMesh() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    vertices[0].position = (targetRotation * glm::vec3(-trailWidth / 2.0f, 0.0f, 0.0f)) + targetPosition;
    vertices[1].position = (targetRotation * glm::vec3(trailWidth / 2.0f, 0.0f, 0.0f)) + targetPosition;

    if(vertexLifetime <= 0.0) {
        for(int i = trailResolution - 1; i >= 1; i--) {
            vertices[(i * 2)].position = vertices[(i - 1) * 2].position;
            vertices[(i * 2) + 1].position = vertices[((i - 1) * 2) + 1].position;
        }
        vertexLifetime = vertexStartLifetime;
    }
    else {
        vertexLifetime -= app->clock.deltaTime;
    }

    app->graphicsBackend.UpdateMeshVertices(mesh, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void MissileTrail::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    shader = &app->graphicsBackend.globalShaders.trails;
    mesh = app->graphicsBackend.CreateQuad();
    mesh.material.albedo = glm::vec3(1.0f);
    mesh.material.shadowColor = glm::vec3(1.0f);
}

void MissileTrail::Initialize() {
    GenerateMesh();
}

void MissileTrail::Update() {
    RecomputeMesh();
}

void MissileTrail::Draw() {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

    Transform t = Transform();
    app->graphicsBackend.SetBackfaceCulling(false);
    app->graphicsBackend.BeginDrawMesh(mesh, *shader, app->sceneManager.activeCamera, t, false);
    app->graphicsBackend.EndDrawMesh(mesh);
    app->graphicsBackend.SetBackfaceCulling(true);
}

void MissileTrail::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.DeleteMesh(mesh);
}


void Missile::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    mesh = &app->graphicsBackend.globalMeshes.heatSeekingMissile;
    shader = &app->graphicsBackend.globalShaders.missile;

    trail.LoadResources();
}

void Missile::Update() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    targetNetworkId = app->networkManager.networkGameState.missileMap[networkId].targetNetworkId;

    float dt = app->clock.currentTime - app->networkManager.networkGameState.lastUpdateTimeStamp;

    NetworkMissile& missileState = app->networkManager.networkGameState.missileMap[networkId];
    glm::vec3 predictedPosition = missileState.position + missileState.velocity * dt;

    transform.position = glm::mix(transform.position, predictedPosition, (float)app->clock.deltaTime * app->networkManager.interpolationFactor);
    transform.rotation = glm::slerp(transform.rotation, missileState.rotation, (float)app->clock.deltaTime * 2.0f);
    velocity = missileState.velocity;
    shouldDetonate = missileState.shouldDetonate;

    std::cout << glm::length(missileState.velocity) << std::endl;

    if(shouldDetonate && !detonated) {
        app->sceneManager.currentScene->GetEntityByName<ExplosionSystemEntity>("explosionSystem")->SpawnExplosion(predictedPosition, MISSILE_EXPLOSION_SIZE, 0.5f);
        detonated = true;
    }

    trail.targetPosition = transform.position;
    trail.targetRotation = transform.rotation;
    trail.Update();
}

void Missile::Draw() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.BeginDrawMesh(*mesh, *shader, app->sceneManager.activeCamera, transform);
    app->graphicsBackend.UploadShaderUniformVec3(*shader, app->sceneManager.currentScene->environment.sunDirection, "uSunDirection");
    app->graphicsBackend.EndDrawMesh(*mesh);

    trail.Draw();
}
