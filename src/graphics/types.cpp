#include "types.hpp"
#include "backend.hpp"
#include "glm/matrix.hpp"
#include "loader.hpp"
#include "../utils/math.hpp"
#include "../gameplay/scene_manager.hpp"
#include "../utils/instrumentor.hpp"
#include "window.hpp"
#include "../application.hpp"

void Skeleton::UpdateGlobalBoneTransforms()  {
    cachedGlobalBoneTransforms.resize(bones.size());

    for(size_t i = 0; i < bones.size(); i++) {
        glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), bones[i].position) * glm::toMat4(bones[i].rotation) * glm::scale(glm::mat4(1.0f), bones[i].scale);

        if(bones[i].parentID != -1) {
            cachedGlobalBoneTransforms[i] = cachedGlobalBoneTransforms[bones[i].parentID] * localTransform;
        }
        else {
            cachedGlobalBoneTransforms[i] = localTransform;
        }
    }
}

Skybox::Skybox(Color skyColor, Color horizonColor) : skyColor(skyColor), horizonColor(horizonColor) {
    std::unique_ptr<Application>& app = Application::GetInstance();

    mesh = app->graphicsBackend.CreateSphere(app->sceneManager.activeCamera.near + 8.0f, true);
    shader = &app->graphicsBackend.globalShaders.skybox;
}

Skybox::Skybox(const json& resourceProperties) {
    std::unique_ptr<Application>& app = Application::GetInstance();

    skyColor.value = {
        resourceProperties["sky-color"][0],
        resourceProperties["sky-color"][1],
        resourceProperties["sky-color"][2],
        resourceProperties["sky-color"][3],
    };

    horizonColor.value = {
        resourceProperties["horizon-color"][0],
        resourceProperties["horizon-color"][1],
        resourceProperties["horizon-color"][2],
        resourceProperties["horizon-color"][3],
    };

    mesh = app->graphicsBackend.CreateSphere(app->sceneManager.activeCamera.near + 8.0f);
    shader = &app->graphicsBackend.globalShaders.skybox;
}

Skybox::~Skybox() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.DeleteMesh(mesh);
}
