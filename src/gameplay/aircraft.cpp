#include "aircraft.hpp"
#include "../graphics/loader.hpp"
#include "../graphics/backend.hpp"
#include "../io/input.hpp"
#include "../io/time.hpp"
#include "GLFW/glfw3.h"
#include "glm/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/gtc/constants.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/quaternion.hpp>
#include "glm/geometric.hpp"
#include "glm/common.hpp"
#include "scene.hpp"
#include "scene_manager.hpp"
#include "../graphics/window.hpp"
#include "../utils/math.hpp"
#include "widget.hpp"
#include <math.h>
#include <nlohmann/json.hpp>

#define BRAKE_ANGLE_LERP_TIME 1.0

#define YAW_ROTATION 15
#define ROLL_ROTATION 25
#define PITCH_ROTATION 25

#define GRAVITY 250.0f

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
    resource.settings.throttleIncreaseRate = JSON["settings"]["throttle-increase-rate"];
    resource.settings.throttleCruise = JSON["settings"]["throttle-increase-rate"];
    resource.settings.maxSpeed = JSON["settings"]["max-speed"];
    resource.settings.cameraRideHeight = JSON["settings"]["camera-ride-height"];
    resource.settings.cameraDistance = JSON["settings"]["camera-distance"];
    resource.settings.cameraZoomDistance = JSON["settings"]["camera-zoom-distance"];
    resource.settings.controlSurfaceTweenStep = JSON["settings"]["control-surface-tween-step"];
    resource.settings.rollMagnifier = JSON["settings"]["roll-magnifier"];
    resource.settings.rollRate = JSON["settings"]["roll-rate"];

    shader = GraphicsBackend::CreateShader(resource.description.shaderResourcePath.c_str());
    skeletalMesh = Loader::LoadSkeletalMeshFromGLTF(resource.description.meshResourcePath.c_str());

    transform.position.y = 100.0;
}

void Aircraft::Initialize() {
    aimWidget = SceneManager::currentScene->GetWidgetByName("aimWidget");
    mouseWidget = SceneManager::currentScene->GetWidgetByName("mouseWidget");

    skeletalMesh.material.albedo = glm::vec3(0.7f);
}

void Aircraft::ApplyControlSurfaces() {
    float uiDiff = aimWidget->position.x - mouseWidget->position.x;
    glm::quat diffQuat = transform.rotation * glm::inverse(targetRotation);
    glm::vec3 eulerAngles = glm::eulerAngles(diffQuat);
    float rollDelta = MathUtils::Clamp<float>(uiDiff, -1.0, 1.0);
    float pitchDelta = MathUtils::Clamp<float>(eulerAngles.x, -1.0, 1.0);
    float yawDelta = MathUtils::Clamp<float>(eulerAngles.y, -1.0, 1.0);

    //testing for flaps
    skeletalMesh.skeleton.bones[resource.description.boneMappings.wingL].SetLocalRotation(glm::vec3(1.0, 0.0, 0.0), resource.settings.flapsMaxAngle * rollDelta);
    skeletalMesh.skeleton.bones[resource.description.boneMappings.wingR].SetLocalRotation(glm::vec3(1.0, 0.0, 0.0), -resource.settings.flapsMaxAngle * rollDelta);

    //testing for tail animation
    skeletalMesh.skeleton.bones[resource.description.boneMappings.tailL].SetLocalRotation(glm::vec3(0.0, 1.0, 0.0), (-resource.settings.tailMaxAngle * pitchDelta) + (-resource.settings.rudderMaxAngle * yawDelta));
    skeletalMesh.skeleton.bones[resource.description.boneMappings.tailR].SetLocalRotation(glm::vec3(0.0, 1.0, 0.0), (resource.settings.tailMaxAngle * pitchDelta) + (-resource.settings.rudderMaxAngle * yawDelta));

    if(InputManager::IsKeyPressed(GLFW_KEY_B)) {
        targetBrakeAngle = MathUtils::Lerp<float>(targetBrakeAngle, resource.settings.brakeMaxAngle, Time::deltaTime * BRAKE_ANGLE_LERP_TIME);
    }
    else {
        targetBrakeAngle = MathUtils::Lerp<float>(targetBrakeAngle, 0, Time::deltaTime * BRAKE_ANGLE_LERP_TIME);
    }

    skeletalMesh.skeleton.bones[resource.description.boneMappings.brake].SetLocalRotation(glm::vec3(1.0, 0.0, 0.0), targetBrakeAngle);
}

void Aircraft::Update() {
    //camera controls
    Camera& camera = SceneManager::activeCamera;
    camera.target = transform.position + GLOBAL_UP * resource.settings.cameraRideHeight;
    camera.position = camera.target + GLOBAL_FORWARD * (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2) ? resource.settings.cameraZoomDistance : resource.settings.cameraDistance);
    //this ugly one-liner makes for smooth camera rotation
    cameraRotationInputValue = MathUtils::Lerp<glm::vec2>(cameraRotationInputValue, cameraRotationInputValue + InputManager::mouseDelta * Time::deltaTime * 3.0, Time::deltaTime * 5.0);

    glm::vec3 horizontalAxis = RotatePointAroundPoint(camera.position, camera.target, cameraRotationInputValue.y, -GLOBAL_LEFT);
    camera.aspect = (float)WindowManager::primaryWindow->width / WindowManager::primaryWindow->height;
    camera.position = RotatePointAroundPoint(horizontalAxis, camera.target, -cameraRotationInputValue.x, glm::vec3(0.0, 1.0, 0.0));

    glm::vec3 cameraForward = glm::normalize(camera.target - camera.position);
    glm::vec3 cameraRight = glm::cross(glm::vec3(0.0, 1.0, 0.0), cameraForward);
    glm::vec3 cameraUp = glm::cross(cameraForward, cameraRight);

    glm::vec3 aircraftForward = glm::normalize(glm::rotate(transform.rotation, GLOBAL_FORWARD));

    //aircraft orientation
    glm::quat extraRotation = glm::identity<glm::quat>();
    if(!InputManager::IsKeyPressed(GLFW_KEY_TAB)){
        float uiDiff = aimWidget->position.x - mouseWidget->position.x;
        extraRotation = glm::angleAxis(MathUtils::Clamp<float>(-uiDiff * resource.settings.rollMagnifier, glm::radians(-90.0f), glm::radians(90.0f)), GLOBAL_FORWARD);
    }

    targetRotation = glm::quatLookAt(-cameraForward, GLOBAL_UP);
    unrolledRotation = glm::slerp(unrolledRotation, targetRotation, (float)Time::deltaTime);
    transform.rotation = unrolledRotation * extraRotation;

    //throttle controls
    if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        controls.throttle += resource.settings.throttleIncreaseRate * Time::deltaTime;
    }
    else if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
        controls.throttle -= resource.settings.throttleIncreaseRate * Time::deltaTime;
    }

    controls.throttle = MathUtils::Clamp<float>(controls.throttle, 0.0f, 1.0f);

    glm::vec3 unrotatedForward = glm::normalize(glm::rotate(unrolledRotation, GLOBAL_FORWARD));

    float fallFactor = GRAVITY * MathUtils::Clamp<float>(((1.0f * resource.settings.throttleCruise) - controls.throttle), 0.0, 1.0);
    glm::vec3 moveOffset = ((unrotatedForward * controls.throttle * resource.settings.maxSpeed) + (-GLOBAL_UP * fallFactor)) * Time::deltaTime;
    transform.position += moveOffset;
    camera.position += moveOffset;
    ApplyControlSurfaces();

    skeletalMesh.skeleton.UpdateGlobalBoneTransforms();
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
    float dot = glm::dot(cameraForward, aircraftForwardVector);

    float x = glm::dot(localLeftVector, cameraForward);
    float y = glm::dot(localUpVector, cameraForward);

    return glm::vec2(x, y);
}

void AircraftWidgetLayer::CreateWidgets() {
    aim = std::make_shared<CircleWidget>("aimWidget");
    aim->radius = 0.02f;
    aim->thickness = 0.002f;
    aim->color.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    widgets.push_back(aim);

    mouse = std::make_shared<RectWidget>("mouseWidget");
    mouse->rotation = 45.0;
    mouse->scale = glm::vec2(0.02);
    mouse->color.value.a = 0.0;
    mouse->borderColor.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    mouse->cornerColor.value = mouse->borderColor.value;
    mouse->border = 16;
    widgets.push_back(mouse);

    std::shared_ptr<RectWidget> rect = std::make_shared<RectWidget>("rect");
    rect->position = glm::vec2(-0.7, -0.7);
    rect->scale = glm::vec2(0.3, 0.2);
    rect->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    rect->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    widgets.push_back(rect);

    aircraft = std::static_pointer_cast<Aircraft>(SceneManager::currentScene->GetEntityByName("FA-XX"));
}

void AircraftWidgetLayer::UpdateLayer() {
    mouse->position.x = MathUtils::Clamp<float>(MathUtils::Lerp<double>(mouse->position.x, InputManager::mouseDelta.x / (WindowManager::widthFraction * 50.0), Time::deltaTime * 10.0), -10.0, 10.0);
#ifdef __EMSCRIPTEN__
    mouse->position.y = MathUtils::Clamp<float>(MathUtils::Lerp<double>(mouse->position.y, InputManager::mouseDelta.y / 50.0, Time::deltaTime * 10.0), -10.0, 10.0);
#else
    mouse->position.y = MathUtils::Clamp<float>(MathUtils::Lerp<double>(mouse->position.y, -InputManager::mouseDelta.y / 50.0, Time::deltaTime * 10.0), -10.0, 10.0);
#endif

    aim->position = UIAlignmentWithRotation(aircraft->unrolledRotation);
    aim->position.x /= WindowManager::widthFraction;
    glm::vec3 aircraftForwardVector = glm::normalize(glm::rotate(aircraft->transform.rotation, GLOBAL_FORWARD));
    glm::vec3 cameraForward = glm::normalize(SceneManager::activeCamera.target - SceneManager::activeCamera.position);
    float dot = glm::dot(cameraForward, aircraftForwardVector);

    aim->color.value.a = dot;
}
