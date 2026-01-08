#include "aircraft.hpp"
#include "../graphics/loader.hpp"
#include "../graphics/backend.hpp"
#include "../io/input.hpp"
#include "../io/time.hpp"
#include "GLFW/glfw3.h"
#include "scene_manager.hpp"
#include "../graphics/window.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void Aircraft::LoadResources() {
    shader = GraphicsBackend::CreateShader("resources/shaders/skeletal.glsl");
    skeletalMesh = Loader::LoadSkeletalMeshFromGLTF("resources/meshes/demo_jet.gltf");
    camera = Camera();
    camera.position = glm::vec3(10.0f, 10.0f, 10.0f);
    camera.target = glm::vec3(0.0f, 0.0f, 0.0f);

    std::cout << "Attemping to read Aircraft Resource JSON file at: " << resourcePath << std::endl;

    std::string resourceFileText = Files::ReadResourceString(resourcePath);
    json JSON = json::parse(resourceFileText);

    resource.description.name = JSON["description"]["name"];
    resource.description.boneMappings.root = JSON["description"]["bone-mappings"]["root"];
    resource.description.boneMappings.brake = JSON["description"]["bone-mappings"]["brake"];
    resource.description.boneMappings.wingL = JSON["description"]["bone-mappings"]["wing.l"];
    resource.description.boneMappings.wingR = JSON["description"]["bone-mappings"]["wing.r"];
    resource.description.boneMappings.tailL = JSON["description"]["bone-mappings"]["tail.l"];
    resource.description.boneMappings.tailR = JSON["description"]["bone-mappings"]["tail.r"];

    std::cout << resource.description.boneMappings.tailL << std::endl;
    std::cout << resource.description.boneMappings.tailR << std::endl;

    resource.settings.flapsMaxAngle = JSON["settings"]["flaps-max-angle"];
    resource.settings.brakeMaxAngle = JSON["settings"]["brake-max-angle"];
    resource.settings.tailMaxAngle = JSON["settings"]["tail-max-angle"];
    resource.settings.rudderMaxAngle = JSON["settings"]["rudder-max-angle"];
    resource.settings.controlSurfaceTweenStep = JSON["settings"]["control-surface-tween-step"];
}

void Aircraft::Update() {
    camera.aspect = (float)WindowManager::primaryWindow->width / WindowManager::primaryWindow->height;
    //camera controls
    glm::vec3 cameraForward = glm::normalize(camera.target - camera.position);
    if(InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
        glm::vec3 cameraRight = glm::cross(glm::vec3(0.0, 1.0, 0.0), cameraForward);
        glm::vec3 horizontalAxis = RotatePointAroundPoint(camera.position, camera.target, InputManager::mouseDeltaY * Time::deltaTime, cameraRight);
        camera.position = RotatePointAroundPoint(horizontalAxis, camera.target, -InputManager::mouseDeltaX * Time::deltaTime, glm::vec3(0.0, 1.0, 0.0));
    }
    camera.position += cameraForward * glm::vec3(InputManager::mouseScrollY);

    //testing for animations
    if(InputManager::IsKeyPressed(GLFW_KEY_A)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailL].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.controlSurfaceTweenStep);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingL].RotateLocal(glm::vec3(1.0, 0.0, 0.0), resource.settings.controlSurfaceTweenStep);
    }
    if(InputManager::IsKeyPressed(GLFW_KEY_D)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailR].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.controlSurfaceTweenStep);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingR].RotateLocal(glm::vec3(1.0, 0.0, 0.0), resource.settings.controlSurfaceTweenStep);
    }
}

void Aircraft::Draw()  {
    GraphicsBackend::BeginDrawSkeletalMesh(skeletalMesh, shader, camera, transform);
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunDirection, "uSunDirection");
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunColor, "uSunColor");
    GraphicsBackend::EndDrawSkeletalMesh(skeletalMesh);
}

void Aircraft::UnloadResources()  {
    GraphicsBackend::DeleteSkeletalMesh(skeletalMesh);
    GraphicsBackend::DeleteShader(shader);
}
