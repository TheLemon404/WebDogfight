#include "aircraft.hpp"
#include "../graphics/loader.hpp"
#include "../graphics/backend.hpp"
#include "../io/input.hpp"
#include "../io/time.hpp"
#include "GLFW/glfw3.h"
#include "glm/common.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/quaternion.hpp>
#include "glm/geometric.hpp"
#include "scene.hpp"
#include "scene_manager.hpp"
#include "../graphics/window.hpp"
#include "../utils/math.hpp"
#include "widget.hpp"
#include <math.h>
#include <nlohmann/json.hpp>

#define YAW_ROTATION 15
#define ROLL_ROTATION 25
#define PITCH_ROTATION 25

using json = nlohmann::json;

void Aircraft::LoadResources() {
    SceneManager::activeCamera.position = glm::vec3(10.0f, 10.0f, 10.0f);
    SceneManager::activeCamera.target = glm::vec3(0.0f, 0.0f, 0.0f);

    std::cout << "Attemping to read Aircraft Resource JSON file at: " << resourcePath << std::endl;

    std::string resourceFileText = Files::ReadResourceString(resourcePath);
    json JSON = json::parse(resourceFileText);

    resource.description.name = JSON["description"]["name"];
    resource.description.shaderResourcePath = JSON["description"]["shader-resource-path"];
    resource.description.meshResourcePath = JSON["description"]["mesh-resource-path"];
    resource.description.boneMappings.root = JSON["description"]["bone-mappings"]["root"];
    resource.description.boneMappings.brake = JSON["description"]["bone-mappings"]["brake"];
    resource.description.boneMappings.wingL = JSON["description"]["bone-mappings"]["wing.l"];
    resource.description.boneMappings.wingR = JSON["description"]["bone-mappings"]["wing.r"];
    resource.description.boneMappings.tailL = JSON["description"]["bone-mappings"]["tail.l"];
    resource.description.boneMappings.tailR = JSON["description"]["bone-mappings"]["tail.r"];
    resource.description.boneMappings.rudderL = JSON["description"]["bone-mappings"]["rudder.l"];
    resource.description.boneMappings.rudderR = JSON["description"]["bone-mappings"]["rudder.r"];

    resource.settings.flapsMaxAngle = JSON["settings"]["flaps-max-angle"];
    resource.settings.brakeMaxAngle = JSON["settings"]["brake-max-angle"];
    resource.settings.tailMaxAngle = JSON["settings"]["tail-max-angle"];
    resource.settings.rudderMaxAngle = JSON["settings"]["rudder-max-angle"];
    resource.settings.maxSpeed = JSON["settings"]["max-speed"];
    resource.settings.cameraRideHeight = JSON["settings"]["camera-ride-height"];
    resource.settings.cameraLagDistance = JSON["settings"]["camera-lag-distance"];
    resource.settings.controlSurfaceTweenStep = JSON["settings"]["control-surface-tween-step"];
    resource.settings.rollMagnifier = JSON["settings"]["roll-magnifier"];

    shader = GraphicsBackend::CreateShader(resource.description.shaderResourcePath.c_str());
    skeletalMesh = Loader::LoadSkeletalMeshFromGLTF(resource.description.meshResourcePath.c_str());

    transform.position.y = 10.0;
}

void Aircraft::Initialize() {
    aimWidget = SceneManager::currentScene->GetWidgetByName("aimWidget");
    mouseWidget = SceneManager::currentScene->GetWidgetByName("mouseWidget");

    skeletalMesh.material.albedo = glm::vec3(0.7f);
}

void Aircraft::ApplyControlSurfaces() {
    //testing for flaps
    if(InputManager::IsKeyJustPressed(GLFW_KEY_Q)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingL].RotateLocal(glm::vec3(1.0, 0.0, 0.0), resource.settings.flapsMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingR].RotateLocal(glm::vec3(1.0, 0.0, 0.0), -resource.settings.flapsMaxAngle);
    }
    else if(InputManager::IsKeyJustReleased(GLFW_KEY_Q)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingL].RotateLocal(glm::vec3(1.0, 0.0, 0.0), -resource.settings.flapsMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingR].RotateLocal(glm::vec3(1.0, 0.0, 0.0), resource.settings.flapsMaxAngle);
    }
    if(InputManager::IsKeyJustPressed(GLFW_KEY_E)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingL].RotateLocal(glm::vec3(1.0, 0.0, 0.0), -resource.settings.flapsMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingR].RotateLocal(glm::vec3(1.0, 0.0, 0.0), resource.settings.flapsMaxAngle);
    }
    else if(InputManager::IsKeyJustReleased(GLFW_KEY_E)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingL].RotateLocal(glm::vec3(1.0, 0.0, 0.0), resource.settings.flapsMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingR].RotateLocal(glm::vec3(1.0, 0.0, 0.0), -resource.settings.flapsMaxAngle);
    }

    //testing for tail animation
    if(InputManager::IsKeyJustPressed(GLFW_KEY_W)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailL].RotateLocal(glm::vec3(0.0, 1.0, 0.0), -resource.settings.tailMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailR].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.tailMaxAngle);
    }
    else if(InputManager::IsKeyJustReleased(GLFW_KEY_W)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailL].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.tailMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailR].RotateLocal(glm::vec3(0.0, 1.0, 0.0), -resource.settings.tailMaxAngle);
    }
    if(InputManager::IsKeyJustPressed(GLFW_KEY_S)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailL].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.tailMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailR].RotateLocal(glm::vec3(0.0, 1.0, 0.0), -resource.settings.tailMaxAngle);
    }
    else if(InputManager::IsKeyJustReleased(GLFW_KEY_S)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailL].RotateLocal(glm::vec3(0.0, 1.0, 0.0), -resource.settings.tailMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.tailR].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.tailMaxAngle);
    }

    //testing for rudder animations
    if(InputManager::IsKeyJustPressed(GLFW_KEY_A)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.rudderL].RotateLocal(glm::vec3(0.0, 1.0, 0.0), -resource.settings.rudderMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.rudderR].RotateLocal(glm::vec3(0.0, 1.0, 0.0), -resource.settings.rudderMaxAngle);
    }
    else if(InputManager::IsKeyJustReleased(GLFW_KEY_A)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.rudderL].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.rudderMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.rudderR].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.rudderMaxAngle);
    }
    if(InputManager::IsKeyJustPressed(GLFW_KEY_D)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.rudderL].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.rudderMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.rudderR].RotateLocal(glm::vec3(0.0, 1.0, 0.0), resource.settings.rudderMaxAngle);
    }
    else if(InputManager::IsKeyJustReleased(GLFW_KEY_D)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.rudderL].RotateLocal(glm::vec3(0.0, 1.0, 0.0), -resource.settings.rudderMaxAngle);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.rudderR].RotateLocal(glm::vec3(0.0, 1.0, 0.0), -resource.settings.rudderMaxAngle);
    }

    if(InputManager::IsKeyJustPressed(GLFW_KEY_B)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.brake].RotateLocal(glm::vec3(1.0, 0.0, 0.0), resource.settings.brakeMaxAngle);
    }
    else if(InputManager::IsKeyJustReleased(GLFW_KEY_B)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.brake].RotateLocal(glm::vec3(1.0, 0.0, 0.0), -resource.settings.brakeMaxAngle);
    }
}

void Aircraft::Update() {
    //camera controls
    Camera& camera = SceneManager::activeCamera;
    glm::vec3 cameraForward = glm::normalize(camera.target - camera.position);
    glm::vec3 cameraRight = glm::cross(glm::vec3(0.0, 1.0, 0.0), cameraForward);
    glm::vec3 cameraUp = glm::cross(cameraForward, cameraRight);
    glm::vec3 horizontalAxis = RotatePointAroundPoint(camera.position, camera.target, InputManager::mouseDelta.y * Time::deltaTime, cameraRight);
    camera.target = transform.position + cameraUp * resource.settings.cameraRideHeight;
    camera.aspect = (float)WindowManager::primaryWindow->width / WindowManager::primaryWindow->height;
    camera.position = RotatePointAroundPoint(horizontalAxis, camera.target, -InputManager::mouseDelta.x * Time::deltaTime, glm::vec3(0.0, 1.0, 0.0));
    camera.position += cameraForward * glm::vec3(InputManager::mouseScroll.y);
    glm::vec3 aircraftForward = glm::normalize(glm::rotate(transform.rotation, GLOBAL_FORWARD));

    //aircraft orientation
    if(!InputManager::IsKeyPressed(GLFW_KEY_TAB)){
        float uiDiff = aimWidget->position.x - mouseWidget->position.x;
        targetRotation.rotation = glm::angleAxis(-uiDiff * resource.settings.rollMagnifier, GLOBAL_FORWARD);
        targetRotation.rotation = glm::quatLookAt(-cameraForward, GLOBAL_UP) * targetRotation.rotation;
    }

    transform.rotation = glm::slerp(transform.rotation, targetRotation.rotation, (float)Time::deltaTime);

    //throttle controls
    if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        controls.throttle += 0.0001f;
    }
    else if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
        controls.throttle -= 0.0001f;
    }
    controls.throttle = MathUtils::Clamp<float>(controls.throttle, 0.0f, 1.0f);
    transform.position += aircraftForward * controls.throttle * resource.settings.maxSpeed;
    camera.position += aircraftForward * controls.throttle * resource.settings.maxSpeed;
    ApplyControlSurfaces();
}

void Aircraft::Draw()  {
    GraphicsBackend::BeginDrawSkeletalMesh(skeletalMesh, shader, SceneManager::activeCamera, transform);
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunDirection, "uSunDirection");
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunColor, "uSunColor");
    GraphicsBackend::EndDrawSkeletalMesh(skeletalMesh);

    if(GraphicsBackend::debugMode){
        Transform t = Transform();
        t.position = transform.position;
        t.rotation = transform.rotation;
        t.scale = glm::vec3(10.0);
        GraphicsBackend::DrawDebugCube(SceneManager::activeCamera, t);
    }
}

void Aircraft::UnloadResources()  {
    GraphicsBackend::DeleteSkeletalMesh(skeletalMesh);
    GraphicsBackend::DeleteShader(shader);
}

glm::vec2 AircraftWidgetLayer::UIAlignmentWithRotation(glm::quat rotation) {
    glm::vec3 aircraftForwardVector = glm::normalize(glm::rotate(rotation, GLOBAL_FORWARD));
    glm::vec3 aircraftUpVector = glm::normalize(glm::rotate(rotation, GLOBAL_UP));
    glm::vec3 aircraftLeftVector = glm::normalize(glm::rotate(rotation, GLOBAL_LEFT));

    glm::vec3 localLeftVector = glm::normalize(glm::rotate(aircraftForwardVector, glm::radians(90.0f), aircraftUpVector));
    glm::vec3 localUpVector = glm::normalize(glm::rotate(aircraftForwardVector, glm::radians(90.0f), aircraftLeftVector));
    glm::vec3 cameraForward = glm::normalize(SceneManager::activeCamera.target - SceneManager::activeCamera.position);
    float x = glm::dot(localLeftVector, cameraForward);
    float y = glm::dot(localUpVector, cameraForward);
    return glm::vec2(x, y);
}

void AircraftWidgetLayer::CreateWidgets() {
    aim = std::make_shared<CircleWidget>("aimWidget");
    aim->radius = 0.05f;
    aim->thickness = 0.005f;
    aim->color.value = glm::vec4(0.0, 1.0, 0.0, 1.0);
    widgets.push_back(aim);

    mouse = std::make_shared<CircleWidget>("mouseWidget");
    mouse->radius = 0.025f;
    mouse->thickness = 0.005f;
    mouse->color.value = glm::vec4(0.0, 1.0, 0.0, 1.0);
    widgets.push_back(mouse);

    aircraft = std::static_pointer_cast<Aircraft>(SceneManager::currentScene->GetEntityByName("FA-XX"));
}

void AircraftWidgetLayer::UpdateLayer() {
    mouse->position.x = InputManager::mouseDelta.x / 25.0f;
    mouse->position.y = -InputManager::mouseDelta.y / 25.0f;

    aim->position = UIAlignmentWithRotation(aircraft->transform.rotation);
}
