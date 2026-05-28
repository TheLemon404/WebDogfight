#include "missile.hpp"

#include "../application.hpp"

#define MISSILE_EXPLOSION_SIZE 200.0f

void Missile::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    mesh = &app->graphicsBackend.globalMeshes.heatSeekingMissile;
    shader = &app->graphicsBackend.globalShaders.missile;
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

    if(shouldDetonate && !detonated) {
        app->sceneManager.currentScene->GetEntityByName<ExplosionSystemEntity>("explosionSystem")->SpawnExplosion(transform.position, MISSILE_EXPLOSION_SIZE, 0.5f);
        detonated = true;
    }
}

void Missile::Draw() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.BeginDrawMesh(*mesh, *shader, app->sceneManager.activeCamera, transform);
    app->graphicsBackend.UploadShaderUniformVec3(*shader, app->sceneManager.currentScene->environment.sunDirection, "uSunDirection");
    app->graphicsBackend.EndDrawMesh(*mesh);
}
