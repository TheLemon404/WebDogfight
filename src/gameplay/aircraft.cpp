#include "aircraft.hpp"

#include "../networking/network_manager.hpp"
#include "../graphics/loader.hpp"
#include "../graphics/backend.hpp"
#include "../io/input.hpp"
#include "../io/time.hpp"
#include "GLFW/glfw3.h"
#include "glm/common.hpp"
#include "glm/detail/qualifier.hpp"
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
#include "../utils/instrumentor.hpp"
#include "scene_manager.hpp"
#include "../graphics/window.hpp"
#include "../utils/math.hpp"
#include "../utils/misc.hpp"
#include <math.h>
#include <nlohmann/json.hpp>

#include "../application.hpp"

#define BRAKE_ANGLE_LERP_TIME 1.0

#define YAW_ROTATION 15
#define ROLL_ROTATION 25
#define PITCH_ROTATION 25

#define GRAVITY 17000.0f
#define DRAG_COEFFICIENT 50.0f
#define GFORCE_COEFFICIENT 0.005f
#define GFORCE_BODY_THRESHOLD 7
#define GFORCE_TRAIL_THRESHOLD 9

using json = nlohmann::json;

glm::vec2 AircraftWidgetLayer::UIAlignmentWithRotation(glm::quat rotation) {
    std::unique_ptr<Application>& app = Application::GetInstance();

    glm::vec3 aircraftForwardVector = glm::normalize(glm::rotate(rotation, GLOBAL_FORWARD));
    glm::vec3 aircraftUpVector = glm::normalize(glm::rotate(rotation, GLOBAL_UP));
    glm::vec3 aircraftLeftVector = glm::normalize(glm::rotate(rotation, GLOBAL_LEFT));

    glm::vec3 cameraForward = glm::normalize(app->sceneManager.activeCamera.target - app->sceneManager.activeCamera.position);
    float dot = glm::dot(cameraForward, aircraftForwardVector);

    float x = glm::dot(aircraftLeftVector, cameraForward);
    float y = glm::dot(-aircraftUpVector, cameraForward);

    return glm::vec2(x, y);
}

void AircraftWidgetLayer::CreateWidgets() {
    //aircraft aiming ui
    aim = std::make_shared<CircleWidget>("aimWidget");
    aim->color.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    aim->moveWithAspectRatio = true;
    widgets.push_back(aim);

    mouse = std::make_shared<RectWidget>("mouseWidget");
    mouse->rotation = 45.0;
    mouse->scale = glm::vec2(0.02);
    mouse->color.value.a = 0.0;
    mouse->borderColor.value = glm::vec4(0.0);
    mouse->cornerLength = 7;
    mouse->cornerColor.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    widgets.push_back(mouse);

    //demo window ui

    std::unique_ptr<Application>& app = Application::GetInstance();

    std::shared_ptr<TextRectWidget> rect = std::make_shared<TextRectWidget>("rect", app->graphicsBackend.globalFonts.defaultFont);
    rect->SetText(
                "Controls:\n"
                "- Shift: Thottle Up\n"
                "- Ctrl: Thottle Down\n"
                "- Q: Roll Left\n"
                "- E: Roll Right\n"
                "- Alt: Free Mouse\n"
                "- Tab: Free Look\n\n"
                "Notes:\n"
                "- Respawn on terrain\n"
                "  or boundary collision.");
    rect->position = glm::vec2(-0.7, 0.2);
    rect->moveWithAspectRatio = true;
    rect->scale = glm::vec2(0.4, 0.35);
    rect->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    rect->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    widgets.push_back(rect);

    //aircraft stats ui
    stats = std::make_shared<TextRectWidget>("stats",app->graphicsBackend.globalFonts.defaultFont);
    stats->moveWithAspectRatio = true;
    stats->scale = glm::vec2(0.4, 0.16);
    stats->position = glm::vec2(0.6, -0.6);
    stats->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    stats->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    widgets.push_back(stats);
}

void AircraftWidgetLayer::UpdateLayer() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    FOX2_PROFILE_FUNCTION()
    glm::vec2 targetDelta = glm::vec2(
        InputManager::mouseDelta.x / app->windowManager.primaryWindow->width * app->windowManager.primaryWindow->aspect,
        -InputManager::mouseDelta.y / app->windowManager.primaryWindow->height
    );

    mouse->position += targetDelta / app->clock.deltaTime * .004f;
    mouse->position *= 0.85f;
    mouse->position = glm::clamp(mouse->position, glm::vec2(-4.0f), glm::vec2(4.0f));

    aim->position = UIAlignmentWithRotation(aircraftProps.unrolledRotation);
    aim->position.x /= app->windowManager.primaryWindow->aspect;
    glm::vec3 aircraftForwardVector = glm::normalize(glm::rotate(aircraftProps.transform.rotation, GLOBAL_FORWARD));
    glm::vec3 cameraForward = glm::normalize(app->sceneManager.activeCamera.target - app->sceneManager.activeCamera.position);
    float dot = glm::dot(cameraForward, aircraftForwardVector);

    aim->color.value.a = dot;

    stats->SetText("FPS: " + MiscUtils::Truncate(std::to_string(1/app->clock.deltaTime), 4) + "\n"
        "Throttle: " + MiscUtils::Truncate(std::to_string(aircraftProps.throttle), 4) + "\n"
        "Speed: " + MiscUtils::Truncate(std::to_string(glm::length(aircraftProps.velocity)), 4) + "m/s\n"
        "Altitude: " + MiscUtils::Truncate(std::to_string(aircraftProps.transform.position.y), 4) + "m\n"
        "G-Force: " + MiscUtils::Truncate(std::to_string(glm::length(aircraftProps.gForce)), 4) + "gs\n");
}

Aircraft::Aircraft(const std::string& name, const std::string& aircraftResourcePath, uint32_t networkId) : Entity(name), resourcePath(aircraftResourcePath), networkId(networkId) {
    std::unique_ptr<Application>& app = Application::GetInstance();
    if(app->networkManager.localClientId == networkId) {
        std::lock_guard<std::mutex> lock(app->networkManager.pendingStateChangeMutex);
        app->networkManager.networkGameState.clientStates[networkId].inGame = true;
    }
}

void Aircraft::LoadResources() {
    std::string resourceFileText = Files::ReadResourceString(resourcePath);
    std::cout << "successfully loaded Aircraft Resource JSON file at: " << resourcePath << std::endl;
    json JSON = json::parse(resourceFileText);

    resource.description.name = JSON["description"]["name"];
    resource.description.shaderResourcePath = JSON["description"]["shader-resource-path"];
    resource.description.meshResourcePath = JSON["description"]["mesh-resource-path"];
    resource.description.boneMappings.root = JSON["description"]["bone-mappings"]["root"];
    resource.description.boneMappings.burner = JSON["description"]["bone-mappings"]["burner"];
    resource.description.boneMappings.brake = JSON["description"]["bone-mappings"]["brake"];
    resource.description.boneMappings.wingL = JSON["description"]["bone-mappings"]["wing.l"];
    resource.description.boneMappings.wingR = JSON["description"]["bone-mappings"]["wing.r"];
    resource.description.boneMappings.tailL = JSON["description"]["bone-mappings"]["tail.l"];
    resource.description.boneMappings.tailR = JSON["description"]["bone-mappings"]["tail.r"];
    resource.description.boneMappings.rudderL = JSON["description"]["bone-mappings"]["rudder.l"];
    resource.description.boneMappings.rudderR = JSON["description"]["bone-mappings"]["rudder.r"];
    resource.description.boneMappings.pressureVorticesL = JSON["description"]["bone-mappings"]["pressureVortices.l"];
    resource.description.boneMappings.pressureVorticesR = JSON["description"]["bone-mappings"]["pressureVortices.r"];

    resource.settings.flapsMaxAngle = JSON["settings"]["flaps-max-angle"];
    resource.settings.brakeMaxAngle = JSON["settings"]["brake-max-angle"];
    resource.settings.tailMaxAngle = JSON["settings"]["tail-max-angle"];
    resource.settings.rudderMaxAngle = JSON["settings"]["rudder-max-angle"];
    resource.settings.throttleIncreaseRate = JSON["settings"]["throttle-increase-rate"];
    resource.settings.throttleCruise = JSON["settings"]["throttle-increase-rate"];
    resource.settings.maxThrust = JSON["settings"]["max-thrust"];
    resource.settings.maxTurnRate = JSON["settings"]["max-turn-rate"];
    resource.settings.terminalLiftSpeed = JSON["settings"]["terminal-lift-speed"];
    resource.settings.cameraRideHeight = JSON["settings"]["camera-ride-height"];
    resource.settings.cameraDistance = JSON["settings"]["camera-distance"];
    resource.settings.cameraZoomDistance = JSON["settings"]["camera-zoom-distance"];
    resource.settings.controlSurfaceTweenStep = JSON["settings"]["control-surface-tween-step"];
    resource.settings.rollMagnifier = JSON["settings"]["roll-magnifier"];
    resource.settings.rollRate = JSON["settings"]["roll-rate"];
    resource.settings.wingTipL = { JSON["settings"]["wing-tip-l"][0], JSON["settings"]["wing-tip-l"][1], JSON["settings"]["wing-tip-l"][2] };
    resource.settings.wingTipR = { JSON["settings"]["wing-tip-r"][0], JSON["settings"]["wing-tip-r"][1], JSON["settings"]["wing-tip-r"][2] };

    shader = Loader::LoadShaderFromGLSL(resource.description.shaderResourcePath.c_str());
    skeletalMesh = Loader::LoadSkeletalMeshFromGLTF(resource.description.meshResourcePath.c_str());

    transform.position.y = 6000.0;

    exhaustParticles.LoadResources();
    leftTrails.LoadResources();
    rightTrails.LoadResources();

    std::unique_ptr<Application>& app = Application::GetInstance();

    if(networkId == app->networkManager.localClientId) {
        Loader::LoadSoundFromFile("resources/audio/engine.wav", engineSound);

        aircraftWidgetLayer = std::make_unique<AircraftWidgetLayer>();
        aircraftWidgetLayer->CreateWidgets();
        aircraftWidgetLayer->LoadResources();
    }
}

void Aircraft::Initialize() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->sceneManager.activeCamera.position = glm::vec3(10.0f, 10.0f, 10.0f);
    app->sceneManager.activeCamera.target = glm::vec3(0.0f, 0.0f, 0.0f);

    skeletalMesh.material.shadowColor = glm::vec3(0.8f);

    exhaustParticles.Initialize();

    leftTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipL);
    rightTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipR);
    leftTrails.Initialize();
    rightTrails.Initialize();

    if(networkId == app->networkManager.localClientId) {
        app->audioBackend.StartSoundAsset(engineSound, true, 0.3f);
    }

    skeletalMesh.skeleton.UpdateGlobalBoneTransforms();

    if(aircraftWidgetLayer) {
        aimWidget = aircraftWidgetLayer->GetWidgetByName("aimWidget");
        mouseWidget = aircraftWidgetLayer->GetWidgetByName("mouseWidget");
        aircraftWidgetLayer->Initialize();
    }
}

void Aircraft::ApplyControlSurfaces(float roll) {
    std::unique_ptr<Application>& app = Application::GetInstance();

    glm::quat diffQuat = transform.rotation * glm::inverse(targetRotation);
    glm::vec3 eulerAngles = glm::eulerAngles(diffQuat);
    float rollDelta = MathUtils::Clamp<float>(roll, -1.0, 1.0);
    #ifdef __EMSCRIPTEN__
    float pitchDelta = MathUtils::Clamp<float>(-eulerAngles.x, -1.0, 1.0);
    #else
    float pitchDelta = MathUtils::Clamp<float>(eulerAngles.x, -1.0, 1.0);
    #endif
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
        targetBrakeAngle = MathUtils::Lerp<float>(targetBrakeAngle, resource.settings.brakeMaxAngle, app->clock.deltaTime * BRAKE_ANGLE_LERP_TIME);
    }
    else {
        targetBrakeAngle = MathUtils::Lerp<float>(targetBrakeAngle, 0, app->clock.deltaTime * BRAKE_ANGLE_LERP_TIME);
    }

    skeletalMesh.skeleton.bones[resource.description.boneMappings.brake].SetLocalRotation(glm::vec3(1.0, 0.0, 0.0), targetBrakeAngle);

    skeletalMesh.skeleton.bones[resource.description.boneMappings.burner].scale.y = pow(controls.throttle, 15);

    float pressureScale = MathUtils::Max<float>(MathUtils::Min<float>(gForce - GFORCE_BODY_THRESHOLD, 0.0f), 1.0f);
    skeletalMesh.skeleton.bones[resource.description.boneMappings.pressureVorticesL].scale = glm::vec3(pressureScale);
    skeletalMesh.skeleton.bones[resource.description.boneMappings.pressureVorticesR].scale = glm::vec3(pressureScale);
}

void Aircraft::Update() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    if(networkId == app->networkManager.localClientId) {
        FOX2_PROFILE_FUNCTION();
        Camera& camera = app->sceneManager.activeCamera;

        glm::vec3 cameraForward;
        {
            FOX2_PROFILE_SCOPE("Camera Movement")
            //camera controls
            glm::vec2 targetDelta = glm::vec2(
                InputManager::mouseDelta.x / app->windowManager.primaryWindow->width * app->windowManager.primaryWindow->aspect,
                InputManager::mouseDelta.y / app->windowManager.primaryWindow->height
            );
            //this ugly one-liner makes for smooth camera rotation
            cameraRotationInputValue += targetDelta * app->clock.deltaTime * 100.0f;
            cameraRotationInputValue.y = MathUtils::Clamp<float>(cameraRotationInputValue.y, (-PI/2) + 0.00001f , (PI/2) - 0.00001f);
            camera.aspect = static_cast<float>(app->windowManager.primaryWindow->width) / app->windowManager.primaryWindow->height;
            cameraForward = glm::normalize(camera.target - camera.position);
            glm::vec3 cameraRight = glm::cross(GLOBAL_UP, cameraForward);
            glm::vec3 cameraUp = glm::cross(cameraForward, cameraRight);
        }

        glm::vec3 aircraftForward;
        glm::vec3 aircraftUp;
        glm::quat extraRotation;
        glm::vec3 unrotatedForward;
        float rollAngle;

        float terminalLiftFactor = MathUtils::Clamp<float>(!std::isnan(speed) ? (speed / resource.settings.terminalLiftSpeed) : 0.0f, 0.0f, 1.0f);

        {
            FOX2_PROFILE_SCOPE("Aircraft Orientation")
            aircraftForward = glm::normalize(glm::rotate(transform.rotation, GLOBAL_FORWARD));
            aircraftUp = glm::normalize(glm::rotate(transform.rotation, GLOBAL_UP));
            extraRotation = glm::identity<glm::quat>();
            if(!InputManager::IsKeyPressed(GLFW_KEY_TAB) && InputManager::mouseHidden){
                uiDiff = MathUtils::Lerp<float>(uiDiff, aimWidget->position.x - mouseWidget->position.x, app->clock.deltaTime * 10.0f);
                targetRotation = glm::quatLookAt(-cameraForward, GLOBAL_UP);
            }
            if(InputManager::IsKeyPressed(GLFW_KEY_Q)) {
                rollInput -= resource.settings.rollRate * app->clock.deltaTime;
                restingRollRotation = 0.0f;
            }
            else if(InputManager::IsKeyPressed(GLFW_KEY_E)) {
                rollInput += resource.settings.rollRate * app->clock.deltaTime;
                restingRollRotation = 2.0f * PI;
            }
            else {
                rollInput = fmodf(rollInput, 2.0 * PI);
                rollInput = MathUtils::Lerp<float>(rollInput, restingRollRotation, app->clock.deltaTime * 2.0f);
            }
            rollAngle = MathUtils::Clamp<float>(-uiDiff * resource.settings.rollMagnifier, glm::radians(-90.0f), glm::radians(90.0f));
            extraRotation = glm::angleAxis(rollAngle + rollInput, GLOBAL_FORWARD);
            unrolledRotation = glm::slerp(unrolledRotation, targetRotation, (float)app->clock.deltaTime);
            unrotatedForward = glm::normalize(glm::rotate(unrolledRotation, GLOBAL_FORWARD));

            glm::vec3 velocityChange = (velocity - lastVelocity) / app->clock.deltaTime;
            float lateralAcceleration = glm::length(velocityChange - glm::dot(velocityChange, unrotatedForward) * unrotatedForward);

            gForce = MathUtils::Lerp<float>(gForce, (lateralAcceleration * GFORCE_COEFFICIENT) + 1.0f, app->clock.deltaTime * 5.0f);

            lastVelocity = velocity;
            lastRotation = unrolledRotation;
        }
        {
            FOX2_PROFILE_SCOPE("Throttle Controls and Audio")
            if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
                controls.throttle += resource.settings.throttleIncreaseRate * app->clock.deltaTime;
            }
            else if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
                controls.throttle -= resource.settings.throttleIncreaseRate * app->clock.deltaTime;
            }
            controls.throttle = MathUtils::Clamp<float>(controls.throttle, 0.0f, 1.0f);

            app->audioBackend.SoundAssetSetPitch(engineSound, controls.throttle);
        }
        {
            FOX2_PROFILE_SCOPE("Stalling and Thrust Logic")
            transform.rotation = glm::normalize(unrolledRotation * extraRotation);
            glm::vec3 thrust = unrotatedForward * controls.throttle * resource.settings.maxThrust;
            glm::vec3 brake = (-thrust / 2.0f) * (targetBrakeAngle / resource.settings.brakeMaxAngle);
            glm::vec3 gravity = -GLOBAL_UP * GRAVITY;
            glm::vec3 lift = -gravity * terminalLiftFactor * (1.0f - glm::abs(glm::dot(aircraftForward, GLOBAL_UP)));

            glm::vec3 acceleration = thrust + gravity + lift + brake - (velocity * DRAG_COEFFICIENT);
            velocity += acceleration * (float)app->clock.deltaTime;

            speed = glm::length(velocity);

            lastPosition = transform.position;
            transform.position += velocity * (float)app->clock.deltaTime;
        }
        {
            FOX2_PROFILE_SCOPE("More Camera Controls")
            camera.target = transform.position + GLOBAL_UP * resource.settings.cameraRideHeight;
            camera.position = camera.target + GLOBAL_FORWARD * (InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2) ? resource.settings.cameraZoomDistance : resource.settings.cameraDistance);
            glm::vec3 horizontalAxis = MathUtils::RotatePointAroundPoint(camera.position, camera.target, cameraRotationInputValue.y, -GLOBAL_LEFT);
            camera.position = MathUtils::RotatePointAroundPoint(horizontalAxis, camera.target, -cameraRotationInputValue.x, GLOBAL_UP);
        }
        {
            FOX2_PROFILE_SCOPE("Animations")
            ApplyControlSurfaces(rollAngle);
            skeletalMesh.skeleton.UpdateGlobalBoneTransforms();
        }
        {
            FOX2_PROFILE_SCOPE("Particles")
            exhaustParticles.aircraftPosition = transform.position;
            exhaustParticles.Update();
            leftTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipL);
            leftTrails.aircraftRotation = transform.rotation;
            leftTrails.gForce = gForce;
            leftTrails.Update();
            rightTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipR);
            rightTrails.aircraftRotation = transform.rotation;
            rightTrails.gForce = gForce;
            rightTrails.Update();
        }

        app->networkManager.networkGameState.clientStates[networkId].position = transform.position;
        app->networkManager.networkGameState.clientStates[networkId].rotation = transform.rotation;
    }
    else {
        if(app->networkManager.networkGameState.clientStates.contains(networkId)) {

            ClientState& clientState = app->networkManager.networkGameState.clientStates[networkId];
            transform.position = clientState.position;
            transform.rotation =  clientState.rotation;
        }
        else {
            // --- TODO --- delete unrecognized entities
            std::cout << "should be deleted" << std::endl;
        }
    }

    {
        FOX2_PROFILE_SCOPE("Particles")
        exhaustParticles.aircraftPosition = transform.position;
        exhaustParticles.Update();
        leftTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipL);
        leftTrails.aircraftRotation = transform.rotation;
        leftTrails.gForce = gForce;
        leftTrails.Update();
        rightTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipR);
        rightTrails.aircraftRotation = transform.rotation;
        rightTrails.gForce = gForce;
        rightTrails.Update();
    }

    if(aircraftWidgetLayer) {
        aircraftWidgetLayer->aircraftProps.transform = transform;
        aircraftWidgetLayer->aircraftProps.unrolledRotation = unrolledRotation;
        aircraftWidgetLayer->aircraftProps.velocity = velocity;
        aircraftWidgetLayer->aircraftProps.throttle = controls.throttle;
        aircraftWidgetLayer->aircraftProps.gForce = gForce;
        aircraftWidgetLayer->Update();
        aircraftWidgetLayer->UpdateLayer();
    }
}

void Aircraft::Draw()  {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

    //Note: here we only animate the local client's aircraft. Other clients' aircraft are static.
    app->graphicsBackend.BeginDrawSkeletalMesh(skeletalMesh, shader, app->sceneManager.activeCamera, transform);
    app->graphicsBackend.UploadShaderUniformVec3(shader, app->sceneManager.currentScene->environment.sunDirection, "uSunDirection");
    app->graphicsBackend.UploadShaderUniformVec3(shader, app->sceneManager.activeCamera.position, "uCameraPosition");
    app->graphicsBackend.UploadShaderUniformInt(shader, 0, "uAlbedoTexture");
    app->graphicsBackend.UseTextureSlot(skeletalMesh.textureMap["albedo"], 0);
    app->graphicsBackend.UploadShaderUniformInt(shader, 1, "uRoughnessTexture");
    app->graphicsBackend.UseTextureSlot(skeletalMesh.textureMap["roughness"], 1);
    app->graphicsBackend.UploadShaderUniformInt(shader, 2, "uEmmissionTexture");
    app->graphicsBackend.UseTextureSlot(skeletalMesh.textureMap["emmission"], 2);
    app->graphicsBackend.EndDrawSkeletalMesh(skeletalMesh);
    app->graphicsBackend.ResetTextureSlots();

    if(app->graphicsBackend.debugMode){
        Transform t = Transform();
        t.position = transform.position;
        t.rotation = transform.rotation;
        t.scale = glm::vec3(10.0);
        app->graphicsBackend.DrawDebugCube(app->sceneManager.activeCamera, COLOR_BLUE, t);

        if(networkId == app->networkManager.localClientId) {
            Transform l = Transform();
            l.position = app->networkManager.lagPosition;
            l.rotation = app->networkManager.lagRotation;
            l.scale = glm::vec3(10.0);
            app->graphicsBackend.DrawDebugCube(app->sceneManager.activeCamera, COLOR_RED, l);
        }
    }

    leftTrails.Draw();
    rightTrails.Draw();
    exhaustParticles.Draw();

    if(aircraftWidgetLayer) {
        aircraftWidgetLayer->Draw();
    }
}

void Aircraft::UnloadResources()  {
    std::unique_ptr<Application>& app = Application::GetInstance();

    if(networkId == app->networkManager.localClientId) {
        app->audioBackend.EndSoundAsset(engineSound);
        app->audioBackend.UnloadSoundAsset(engineSound);
    }

    app->graphicsBackend.DeleteSkeletalMesh(skeletalMesh);
    app->graphicsBackend.DeleteShader(shader);

    exhaustParticles.UnloadResources();

    leftTrails.UnloadResources();
    rightTrails.UnloadResources();

    if(aircraftWidgetLayer) {
        aircraftWidgetLayer->UnloadResources();
    }
}

void AircraftExhaustParticleSystem::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    mesh = app->graphicsBackend.CreateQuad();
    mesh.material.albedo = glm::vec3(0.7f);
    mesh.material.alpha = 0.1f;
    shader = &app->graphicsBackend.globalShaders.particles;
}

void AircraftExhaustParticleSystem::Initialize() {
    for(size_t i = 0; i < MAX_PARTICLE_TRANSFORMS; i++) {
        transforms[i] = Transform();
        transforms[i].position = aircraftPosition;
    }
}

void AircraftExhaustParticleSystem::Update() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    FOX2_PROFILE_FUNCTION()

    glm::vec3 toCameraDir = glm::normalize(app->sceneManager.activeCamera.target - app->sceneManager.activeCamera.position);
    for(size_t i = 0; i < MAX_PARTICLE_TRANSFORMS; i++) {
        if(particleLifetimes[i] <= 0.0) {
            transforms[i].position = aircraftPosition;
            particleLifetimes[i] = particleStartLifetime;
            particleRotations[i] = (float)rand() / 10.0f;
        }
        else {
            particleLifetimes[i] -= app->clock.deltaTime;
        }

        transforms[i].rotation = glm::quatLookAt(toCameraDir, GLOBAL_UP) * glm::angleAxis(particleRotations[i], GLOBAL_FORWARD);
        float scaleByLifetime = (1.0 - (abs(particleLifetimes[i] - (particleStartLifetime / 2.0f)) / (particleStartLifetime / 2.0))) * 4.0;
        transforms[i].scale = glm::vec3(scaleByLifetime);
    }
}

void AircraftExhaustParticleSystem::Draw() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.SetBackfaceCulling(false);
    app->graphicsBackend.BeginDrawMeshInstanced(mesh, *shader, app->sceneManager.activeCamera, transforms, MAX_PARTICLE_TRANSFORMS);
    app->graphicsBackend.EndDrawMeshInstanced(mesh, MAX_PARTICLE_TRANSFORMS);
    app->graphicsBackend.SetBackfaceCulling(true);
}

void AircraftExhaustParticleSystem::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.DeleteMesh(mesh);
}

void AircraftTrails::GenerateMesh() {
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
            glm::vec3(-trailWidth / 2.0f, 0.0f, z) + aircraftPosition,      // Position
            {0.0f, 1.0f, 0.0f},                 // Normal (pointing up)
            {0.0f, v}                           // UV
        });

        // Right vertex
        vertices.push_back({
            glm::vec3(trailWidth / 2.0f, 0.0f, z) + aircraftPosition,       // Position
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

void AircraftTrails::RecomputeMesh() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    float pressureScale = MathUtils::Max<float>(MathUtils::Min<float>(gForce - GFORCE_TRAIL_THRESHOLD, 0.0f), 1.0f);

    vertices[0].position = (aircraftRotation * glm::vec3(-trailWidth * pressureScale/ 2.0f, 0.0f, 0.0f)) + aircraftPosition;
    vertices[1].position = (aircraftRotation * glm::vec3(trailWidth * pressureScale / 2.0f, 0.0f, 0.0f)) + aircraftPosition;

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

void AircraftTrails::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    shader = &app->graphicsBackend.globalShaders.trails;
    mesh = app->graphicsBackend.CreateQuad();
    mesh.material.albedo = glm::vec3(1.0f);
    mesh.material.shadowColor = glm::vec3(1.0f);
}

void AircraftTrails::Initialize() {
    GenerateMesh();
}

void AircraftTrails::Update() {
    RecomputeMesh();
}

void AircraftTrails::Draw() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    Transform t = Transform();
    app->graphicsBackend.SetBackfaceCulling(false);
    app->graphicsBackend.BeginDrawMesh(mesh, *shader, app->sceneManager.activeCamera, t, false);
    app->graphicsBackend.EndDrawMesh(mesh);
    app->graphicsBackend.SetBackfaceCulling(true);
}

void AircraftTrails::UnloadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->graphicsBackend.DeleteMesh(mesh);
}
