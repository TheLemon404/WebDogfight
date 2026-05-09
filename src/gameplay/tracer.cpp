#include "tracer.hpp"

#include "../graphics/backend.hpp"
#include "../application.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "../utils/math.hpp"

#define MAX_TRACERS 100
#define TRACER_LIFETIME_SECONDS 1.0f

void TracerSystemEntity::UpdateInstanceMeshTransforms() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    std::vector<glm::mat4> tracerMatrices;
    for(size_t i = 0; i < tracerTransforms.size(); i++) {
        tracerMatrices.push_back(tracerTransforms[i].GetMatrix());
    }

    app->graphicsBackend.UpdateInstancedMeshTransforms(tracerMesh, tracerMatrices.data(), tracerMatrices.size());
}

void TracerSystemEntity::SpawnTracer(glm::vec3 start, glm::vec3 end) {
    if(tracerTransforms.size() > MAX_TRACERS) {
        return;
    }

    std::unique_ptr<Application>& app = Application::GetInstance();

    Transform tracerTransform;
    tracerTransform.position = (start + end) / 2.0f;
    glm::vec3 diffVector = end - start;
    float length = glm::length(diffVector);
    glm::vec3 targetVector = diffVector / length;
    tracerTransform.rotation = glm::quatLookAt(targetVector, GLOBAL_UP);
    tracerTransform.scale = glm::vec3(0.5f, 0.5f, length / 2.0f);
    tracerTransforms.push_back(tracerTransform);
    tracerSpawnTimes.push_back(app->clock.currentTime);

    UpdateInstanceMeshTransforms();
}

void TracerSystemEntity::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    tracerMesh = app->graphicsBackend.CreateCube();
    app->graphicsBackend.UploadInstancedMeshTransforms(tracerMesh, nullptr, MAX_TRACERS);
    tracerShader = &app->graphicsBackend.globalShaders.particles;
}

void TracerSystemEntity::Update() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    bool transformsChangedFlag = false;

    for(size_t i = 0; i < tracerSpawnTimes.size();) {
        if(app->clock.currentTime - tracerSpawnTimes[i] > TRACER_LIFETIME_SECONDS) {
            tracerSpawnTimes.erase(tracerSpawnTimes.begin() + i);
            tracerTransforms.erase(tracerTransforms.begin() + i);
            transformsChangedFlag = true;
        }
        else {
            i++;
        }
    }

    if(transformsChangedFlag) {
        UpdateInstanceMeshTransforms();
    }
}

void TracerSystemEntity::Draw() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    app->graphicsBackend.SetBackfaceCulling(false);
    app->graphicsBackend.BeginDrawMeshInstanced(tracerMesh, *tracerShader, app->sceneManager.activeCamera, tracerTransforms.data(), tracerTransforms.size());
    app->graphicsBackend.EndDrawMeshInstanced(tracerMesh, tracerTransforms.size());
    app->graphicsBackend.SetBackfaceCulling(true);
}

void TracerSystemEntity::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    app->graphicsBackend.DeleteMesh(tracerMesh);
}
