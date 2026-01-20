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
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "widget.hpp"
#include <cstddef>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/quaternion.hpp>
#include "glm/geometric.hpp"
#include "glm/common.hpp"
#include "scene_manager.hpp"
#include "../graphics/window.hpp"
#include "../utils/math.hpp"
#include <math.h>
#include <nlohmann/json.hpp>

#define BRAKE_ANGLE_LERP_TIME 1.0

#define YAW_ROTATION 15
#define ROLL_ROTATION 25
#define PITCH_ROTATION 25

#define GRAVITY 250.0f
#define MAGIC_STALL_NUMBER 46.0f

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

    shader = Loader::LoadShaderFromGLSL(resource.description.shaderResourcePath.c_str());
    skeletalMesh = Loader::LoadSkeletalMeshFromGLTF(resource.description.meshResourcePath.c_str());

    transform.position.y = 6000.0;

    exhaustParticles = AircraftExhaustParticleSystem();
    exhaustParticles.LoadResources();

    AudioBackend::LoadSound("resources/audio/engine.wav", engineSound);
}

void Aircraft::Initialize() {
    aimWidget = SceneManager::currentScene->GetWidgetByName("aimWidget");
    mouseWidget = SceneManager::currentScene->GetWidgetByName("mouseWidget");

    skeletalMesh.material.albedo = glm::vec3(0.7f);

    exhaustParticles.Initialize();

    AudioBackend::StartSoundAsset(engineSound, true, 0.3f);
}

void Aircraft::ApplyControlSurfaces(float roll) {
    glm::quat diffQuat = transform.rotation * glm::inverse(targetRotation);
    glm::vec3 eulerAngles = glm::eulerAngles(diffQuat);
    float rollDelta = MathUtils::Clamp<float>(roll, -1.0, 1.0);
    float pitchDelta = MathUtils::Clamp<float>(eulerAngles.x, -1.0, 1.0);
    float yawDelta = MathUtils::Clamp<float>(eulerAngles.y, -1.0, 1.0);

    //testing for flaps
    if(!InputManager::IsKeyPressed(GLFW_KEY_TAB)) {
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingL].SetLocalRotation(glm::vec3(1.0, 0.0, 0.0), resource.settings.flapsMaxAngle * rollDelta);
        skeletalMesh.skeleton.bones[resource.description.boneMappings.wingR].SetLocalRotation(glm::vec3(1.0, 0.0, 0.0), -resource.settings.flapsMaxAngle * rollDelta);
    }

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

    //this ugly one-liner makes for smooth camera rotation
    smoothedMouseDelta = MathUtils::Lerp<glm::vec2>(smoothedMouseDelta, InputManager::mouseDelta / 500.0, Time::deltaTime * 10.0);
    cameraRotationInputValue += InputManager::mouseDelta / 500.0;
    camera.aspect = (float)WindowManager::primaryWindow->width / WindowManager::primaryWindow->height;

    glm::vec3 cameraForward = glm::normalize(camera.target - camera.position);
    glm::vec3 cameraRight = glm::cross(GLOBAL_UP, cameraForward);
    glm::vec3 cameraUp = glm::cross(cameraForward, cameraRight);

    glm::vec3 aircraftForward = glm::normalize(glm::rotate(transform.rotation, GLOBAL_FORWARD));

    //aircraft orientation
    glm::quat extraRotation = glm::identity<glm::quat>();

    if(!InputManager::IsKeyPressed(GLFW_KEY_TAB) && InputManager::mouseHidden){
        uiDiff = MathUtils::Lerp<float>(uiDiff, aimWidget->position.x - mouseWidget->position.x, Time::deltaTime * 10.0f);
        targetRotation = glm::quatLookAt(-cameraForward, GLOBAL_UP);
    }

    if(InputManager::IsKeyPressed(GLFW_KEY_Q)) {
        rollInput -= resource.settings.rollRate * Time::deltaTime;
    }
    else if(InputManager::IsKeyPressed(GLFW_KEY_E)) {
        rollInput += resource.settings.rollRate * Time::deltaTime;
    }
    else {
        rollInput = fmodf(rollInput, 2.0f * 3.141592f);
        rollInput = MathUtils::Lerp<float>(rollInput, 0.0f, Time::deltaTime * 2.0f);
    }

    float rollAngle = MathUtils::Clamp<float>(-uiDiff * resource.settings.rollMagnifier, glm::radians(-90.0f), glm::radians(90.0f));
    extraRotation = glm::angleAxis(rollAngle + rollInput, GLOBAL_FORWARD);

    unrolledRotation = glm::slerp(unrolledRotation, targetRotation, (float)Time::deltaTime);

    //throttle controls
    if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        controls.throttle += resource.settings.throttleIncreaseRate * Time::deltaTime;
    }
    else if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
        controls.throttle -= resource.settings.throttleIncreaseRate * Time::deltaTime;
    }
    controls.throttle = MathUtils::Clamp<float>(controls.throttle, 0.0f, 1.0f);

    AudioBackend::SoundAssetSetPitch(engineSound, controls.throttle);

    glm::vec3 unrotatedForward = glm::normalize(glm::rotate(unrolledRotation, GLOBAL_FORWARD));

    //stalling and thrust logic
    physicsBody.forwardThrust = MathUtils::Lerp<float>(physicsBody.forwardThrust, controls.throttle, Time::deltaTime * 2.0);

    float stallFactor = 1.0 - (MathUtils::Clamp<float>(physicsBody.forwardThrust, 0.0f, resource.settings.throttleCruise) * (1.0f / resource.settings.throttleCruise));

    // --- TODO --- alter this when adding stall rotation logic
    transform.rotation = glm::normalize(unrolledRotation * extraRotation);

    glm::vec3 moveDirection = unrotatedForward * physicsBody.forwardThrust * resource.settings.maxSpeed;
    glm::vec3 moveOffset = MathUtils::Lerp<glm::vec3>(moveDirection, -GLOBAL_UP * GRAVITY, stallFactor) * (float)Time::deltaTime;
    transform.position += moveOffset;

    camera.target = transform.position + GLOBAL_UP * resource.settings.cameraRideHeight;
    camera.position = camera.target + GLOBAL_FORWARD * (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2) ? resource.settings.cameraZoomDistance : resource.settings.cameraDistance);
    glm::vec3 horizontalAxis = MathUtils::RotatePointAroundPoint(camera.position, camera.target, cameraRotationInputValue.y, -GLOBAL_LEFT);
    camera.position = MathUtils::RotatePointAroundPoint(horizontalAxis, camera.target, -cameraRotationInputValue.x, GLOBAL_UP);

    ApplyControlSurfaces(rollAngle - rollInput);
    skeletalMesh.skeleton.UpdateGlobalBoneTransforms();

    exhaustParticles.aircraftPosition = transform.position;
    exhaustParticles.Update();
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

    exhaustParticles.Draw();
}

void Aircraft::UnloadResources()  {
    AudioBackend::EndSoundAsset(engineSound);

    GraphicsBackend::DeleteSkeletalMesh(skeletalMesh);
    GraphicsBackend::DeleteShader(shader);

    exhaustParticles.UnloadResources();
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
    //aircraft aiming ui
    aim = std::make_shared<CircleWidget>("aimWidget");
    aim->color.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    widgets.push_back(aim);

    mouse = std::make_shared<RectWidget>("mouseWidget");
    mouse->rotation = 45.0;
    mouse->scale = glm::vec2(0.02);
    mouse->color.value.a = 0.0;
    mouse->borderColor.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    mouse->cornerColor.value = mouse->borderColor.value;
    widgets.push_back(mouse);

    //demo window ui
    Font font = Font();
    Loader::LoadFontFromTTF("resources/fonts/JetBrainsMono-Medium.ttf", font);
    std::shared_ptr<TextRectWidget> rect = std::make_shared<TextRectWidget>("rect", font);
    rect->SetText("Welcome to the Fox2.io\n"
                "flight controls test!\n\n"
                "Controls:\n"
                "- Shift: Thottle Up\n"
                "- Ctrl: Thottle Down\n"
                "- Q: Roll Left\n"
                "- E: Roll Right\n"
                "- Alt: Free Mouse\n"
                "- Tab: Free Look\n\n"
                "Notes:\n"
                "- Press T to remove\n"
                "  this window\n"
                "  (it may increase FPS)\n"
                "- Stalling is implimented.\n"
                "- Respawn on terrain\n"
                "  or boundary collision.\n\n"
                "Follow development at:\n"
                "- YouTube: @thelemon9300\n"
                "- X: @MichaelTeschner7");
    rect->position = glm::vec2(-0.7, 0.2);
    rect->scale = glm::vec2(0.4, 0.7);
    rect->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    rect->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    widgets.push_back(rect);

    //aircraft stats ui
    stats = std::make_shared<TextRectWidget>("stats", font);
    stats->SetText("FPS: " + std::to_string(1/Time::deltaTime) + "\n");
    stats->scale = glm::vec2(0.4, 0.1);
    stats->position = glm::vec2(0.6, -0.8);
    stats->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    stats->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    widgets.push_back(stats);

    aircraft = std::static_pointer_cast<Aircraft>(SceneManager::currentScene->GetEntityByName("FA-XX"));
}

void AircraftWidgetLayer::UpdateLayer() {
    glm::vec2 targetDelta = glm::vec2(InputManager::mouseDelta.x / (WindowManager::widthFraction * 1000.0),
        #ifdef __EMSCRIPTEN__
        InputManager::mouseDelta.y / 1000.0
        #else
        -InputManager::mouseDelta.y / 1000.0
        #endif
    );

    mouse->position += targetDelta;
    mouse->position *= 0.95f;
    mouse->position = glm::clamp(mouse->position, glm::vec2(-4.0f), glm::vec2(4.0f));

    aim->position = UIAlignmentWithRotation(aircraft->unrolledRotation);
    aim->position.x /= WindowManager::widthFraction;
    glm::vec3 aircraftForwardVector = glm::normalize(glm::rotate(aircraft->transform.rotation, GLOBAL_FORWARD));
    glm::vec3 cameraForward = glm::normalize(SceneManager::activeCamera.target - SceneManager::activeCamera.position);
    float dot = glm::dot(cameraForward, aircraftForwardVector);

    aim->color.value.a = dot;

    stats->SetText("FPS: " + std::to_string(1/Time::deltaTime) + "\n"
        "Throttle: " + std::to_string(aircraft->controls.throttle) + "\n");
}

void AircraftExhaustParticleSystem::LoadResources() {
    mesh = GraphicsBackend::CreateQuad();
    mesh.material.albedo = glm::vec3(0.7f);
    mesh.material.alpha = 0.1f;
    shader = &GraphicsBackend::globalShaders.particles;
}

void AircraftExhaustParticleSystem::Initialize() {
    for(size_t i = 0; i < MAX_PARTICLE_TRANSFORMS; i++) {
        transforms[i] = Transform();
        transforms[i].position = aircraftPosition;
    }
}

void AircraftExhaustParticleSystem::Update() {
    glm::vec3 toCameraDir = glm::normalize(SceneManager::activeCamera.target - SceneManager::activeCamera.position);
    for(size_t i = 0; i < MAX_PARTICLE_TRANSFORMS; i++) {
        if(particleLifetimes[i] <= 0.0) {
            transforms[i].position = aircraftPosition;
            particleLifetimes[i] = particleStartLifetime;
            particleRotations[i] = (float)rand() / 10.0f;
        }
        else {
            particleLifetimes[i] -= Time::deltaTime;
        }

        transforms[i].rotation = glm::quatLookAt(toCameraDir, GLOBAL_UP) * glm::angleAxis(particleRotations[i], GLOBAL_FORWARD);
        float scaleByLifetime = (1.0 - (abs(particleLifetimes[i] - (particleStartLifetime / 2.0f)) / (particleStartLifetime / 2.0))) * 4.0;
        transforms[i].scale = glm::vec3(scaleByLifetime);
    }
}

void AircraftExhaustParticleSystem::Draw() {
    GraphicsBackend::SetBackfaceCulling(false);
    GraphicsBackend::BeginDrawMeshInstanced(mesh, *shader, SceneManager::activeCamera, transforms, MAX_PARTICLE_TRANSFORMS);
    GraphicsBackend::EndDrawMeshInstanced(mesh, MAX_PARTICLE_TRANSFORMS);
    GraphicsBackend::SetBackfaceCulling(true);
}

void AircraftExhaustParticleSystem::UnloadResources() {
    GraphicsBackend::DeleteMesh(mesh);
}
