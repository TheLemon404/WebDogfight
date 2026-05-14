#include "tracer.hpp"

#include "../graphics/backend.hpp"
#include "../application.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "../utils/math.hpp"
#include <cstddef>

#define MAX_TRACERS 100
#define TRACER_LIFETIME_SECONDS 1.0f

void TracerSystemEntity::UpdateInstanceMeshTransforms() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.BindVertexArray(tracerMesh.vao);
    app->graphicsBackend.BindBuffer(tracerMesh.ibo);
    app->graphicsBackend.BufferSubData(GL_ARRAY_BUFFER, 0, tracerInstances.size() * sizeof(BulletInstanceData), tracerInstances.data());
    app->graphicsBackend.BindBuffer(0);
}

void TracerSystemEntity::SpawnTracer(glm::vec3 start, glm::vec3 end) {
    if(tracerInstances.size() > MAX_TRACERS) {
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
    tracerInstances.push_back({
        .transform = tracerTransform.GetMatrix(),
        .spawnTime = (float)app->clock.currentTime,
    });

    UpdateInstanceMeshTransforms();
}

void TracerSystemEntity::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    tracerMesh = app->graphicsBackend.CreateCube();
    app->graphicsBackend.BindVertexArray(tracerMesh.vao);
    app->graphicsBackend.GenBuffer(tracerMesh.ibo);
    app->graphicsBackend.BindBuffer(tracerMesh.ibo);
    app->graphicsBackend.BufferData(GL_ARRAY_BUFFER, MAX_TRACERS * sizeof(BulletInstanceData), tracerInstances.data(), GL_DYNAMIC_DRAW);
    for(int i = 0; i < 4; i++) {
        app->graphicsBackend.VertexAttribPointer(4 + i, 4, sizeof(BulletInstanceData), (void*)(i * sizeof(glm::vec4)));
        app->graphicsBackend.EnableVertexAttribDivisor(4 + i, 1);
        app->graphicsBackend.EnableVertexAttribArray(4 + i);
    }
    app->graphicsBackend.VertexAttribPointer(8, 1, sizeof(BulletInstanceData), (void*)offsetof(BulletInstanceData, spawnTime));
    app->graphicsBackend.EnableVertexAttribDivisor(8, 1);
    app->graphicsBackend.EnableVertexAttribArray(8);
    app->graphicsBackend.BindBuffer(0);
    app->graphicsBackend.BindVertexArray(0);
    tracerShader = &app->graphicsBackend.globalShaders.tracer;
}

void TracerSystemEntity::Update() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    bool transformsChangedFlag = false;

    for(size_t i = 0; i < tracerInstances.size();) {
        if(app->clock.currentTime - tracerInstances[i].spawnTime > TRACER_LIFETIME_SECONDS) {
            tracerInstances.erase(tracerInstances.begin() + i);
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
    app->graphicsBackend.BeginDrawMeshInstanced(tracerMesh, *tracerShader, app->sceneManager.activeCamera);
    app->graphicsBackend.UploadShaderUniformFloat(*tracerShader, app->clock.currentTime, "uTime");
    app->graphicsBackend.EndDrawMeshInstanced(tracerMesh, tracerInstances.size());
    app->graphicsBackend.SetBackfaceCulling(true);
}

void TracerSystemEntity::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();
    app->graphicsBackend.DeleteMesh(tracerMesh);
}
