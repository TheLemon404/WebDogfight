#include "flares.hpp"

#include "../graphics/backend.hpp"
#include "../application.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "../utils/math.hpp"
#include <cmath>

#define MAX_FLARES 25
#define FLARE_LIFETIME_SECONDS 2.0f
#define PARTICLE_GRAVITY 0.02f

void FlareParticleSystem::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    mesh = app->graphicsBackend.CreateQuad();
    mesh.material.albedo = startAlbedo;
    mesh.material.alpha = alpha;
    shader = &app->graphicsBackend.globalShaders.flares;
}

void FlareParticleSystem::Initialize() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    glm::mat4 transformationMatrices[MAX_FLARE_PARTICLES];
    for(size_t i = 0; i < MAX_FLARE_PARTICLES; i++) {
        transforms[i] = Transform();
        transforms[i].position = leadPosition;
        transformationMatrices[i] = transforms[i].GetMatrix();
    }

    app->graphicsBackend.UploadInstancedMeshTransforms(mesh, transformationMatrices, MAX_FLARE_PARTICLES);
}

void FlareParticleSystem::Update() {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

    glm::vec3 toCameraDir = glm::normalize(app->sceneManager.activeCamera.target - app->sceneManager.activeCamera.position);

    leadVelocity.y -= PARTICLE_GRAVITY * (1.0f - ((app->clock.currentTime - spawnTime) / FLARE_LIFETIME_SECONDS));
    leadVelocity.x *= 0.975f;
    leadVelocity.z *= 0.975f;
    leadPosition += leadVelocity;

    for(size_t i = 0; i < MAX_FLARE_PARTICLES; i++) {

        if(particleLifetimes[i] <= 0.0 && spawnTime + FLARE_LIFETIME_SECONDS > particleStartLifetime + app->clock.currentTime) {
            transforms[i].position = leadPosition;
            particleLifetimes[i] = particleStartLifetime;
            particleRotations[i] = (float)rand() / 10.0f;
        }
        else {
            particleLifetimes[i] = MathUtils::Min(particleLifetimes[i] - (float)app->clock.deltaTime, 0.0f);
        }

        transforms[i].rotation = glm::quatLookAt(toCameraDir, GLOBAL_UP) * glm::angleAxis(particleRotations[i], GLOBAL_FORWARD);
        transforms[i].scale = glm::vec3(scale * (1.0 - (abs(particleLifetimes[i] - (particleStartLifetime)) / particleStartLifetime)));
    }

    glm::mat4 transformationMatrices[MAX_FLARE_PARTICLES];
    for(size_t i = 0; i < MAX_FLARE_PARTICLES; i++) {
        transformationMatrices[i] = transforms[i].GetMatrix();
    }

    app->graphicsBackend.UpdateInstancedMeshTransforms(mesh, transformationMatrices, MAX_FLARE_PARTICLES);
}

void FlareParticleSystem::Draw() {
    FOX2_PROFILE_FUNCTION()

    if(!emitting) return;

    std::unique_ptr<Application>& app = Application::GetInstance();

    if(disableBackfaceCulling) {
        app->graphicsBackend.SetBackfaceCulling(false);
    }
    app->graphicsBackend.BeginDrawMeshInstanced(mesh, *shader, app->sceneManager.activeCamera);
    app->graphicsBackend.EndDrawMeshInstanced(mesh, MAX_FLARE_PARTICLES);
    if(disableBackfaceCulling) {
        app->graphicsBackend.SetBackfaceCulling(true);
    }
}

void FlareParticleSystem::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.DeleteMesh(mesh);
}

void FlareSystemEntity::SpawnFlare(glm::vec3 emitterPosition, glm::quat emitterRotation, glm::vec3 emitterVelocity) {
    FOX2_PROFILE_FUNCTION()

    if(flareParticleInstances.size() > MAX_FLARES) {
        return;
    }

    std::unique_ptr<Application>& app = Application::GetInstance();

    FlareParticleSystem leftParticleSystem = FlareParticleSystem();
    leftParticleSystem.leadPosition = emitterPosition;
    leftParticleSystem.leadVelocity = emitterVelocity / 100.0f + emitterRotation * GLOBAL_LEFT;
    leftParticleSystem.spawnTime = app->clock.currentTime;
    leftParticleSystem.LoadResources();
    leftParticleSystem.Initialize();
    flareParticleInstances.push_back(leftParticleSystem);

    FlareParticleSystem rightParticleSystem = FlareParticleSystem();
    rightParticleSystem.leadPosition = emitterPosition;
    rightParticleSystem.leadVelocity = emitterVelocity / 100.0f - emitterRotation * GLOBAL_LEFT;
    rightParticleSystem.spawnTime = app->clock.currentTime;
    rightParticleSystem.LoadResources();
    rightParticleSystem.Initialize();
    flareParticleInstances.push_back(rightParticleSystem);
}

void FlareSystemEntity::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();
}

void FlareSystemEntity::Update() {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

    for(size_t i = 0; i < flareParticleInstances.size();) {
        if(app->clock.currentTime - flareParticleInstances[i].spawnTime > FLARE_LIFETIME_SECONDS) {
            flareParticleInstances[i].UnloadResources();
            flareParticleInstances.erase(flareParticleInstances.begin() + i);
        }
        else {
            flareParticleInstances[i].Update();
            i++;
        }
    }
}

void FlareSystemEntity::Draw() {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();
    app->graphicsBackend.SetBackfaceCulling(false);
    for(auto& flareParticleSystem : flareParticleInstances) {
        flareParticleSystem.Draw();
    }
    app->graphicsBackend.SetBackfaceCulling(true);
}

void FlareSystemEntity::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    for(auto& flareParticleSystem : flareParticleInstances) {
        flareParticleSystem.UnloadResources();
    }

    flareParticleInstances.clear();
}
