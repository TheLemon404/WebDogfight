#include "aircraft.hpp"

#include "../networking/network_manager.hpp"
#include "../graphics/loader.hpp"
#include "../graphics/backend.hpp"
#include "../io/input.hpp"
#include "../io/time.hpp"
#include "GLFW/glfw3.h"
#include "explosion.hpp"
#include "glm/common.hpp"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "test_scene.hpp"
#include "widget.hpp"
#include <cmath>
#include <cstddef>
#include <memory>
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

#define SHOT_DOWN_EXPLOSION_SIZE 75.0f
#define EXPLODE_EXPLOSION_SIZE 120.0f

#define BRAKE_ANGLE_LERP_TIME 1.0

#define YAW_ROTATION 15
#define ROLL_ROTATION 25
#define PITCH_ROTATION 25

#define GRAVITY 17000.0f
#define DRAG_COEFFICIENT 50.0f
#define GFORCE_COEFFICIENT 0.005f
#define GFORCE_BODY_THRESHOLD 7
#define GFORCE_TRAIL_THRESHOLD 9

#define FONT_CHAR_WIDTH_PIXELS 0.0175f


using json = nlohmann::json;

void CompassWidget::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    quad = &app->graphicsBackend.globalMeshes.quad;
    shader = &app->graphicsBackend.globalShaders.compass;

    textShader = &app->graphicsBackend.globalShaders.font;
    textMesh = app->graphicsBackend.CreateQuad();

    RecomputeTextMesh();
}

void CompassWidget::Draw() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    float cameraRotation = app->sceneManager.activeCamera.GetYaw();
    float rotationDegrees = (cameraRotation + PI) * (180.0f / PI);
    SetText("                                " + std::to_string(static_cast<int>(std::round(rotationDegrees))) + " deg");

    app->graphicsBackend.SetDepthTest(false);
    if(showPanelRect){
        app->graphicsBackend.BeginDrawMesh2D(*quad, *shader, position, scale, rotation, z_distance,stretchWithAspectRatio, moveWithAspectRatio);
        app->graphicsBackend.UploadShaderUniformVec4(*shader, color.value, "uColor");
        app->graphicsBackend.UploadShaderUniformInt(*shader, border, "uBorder");
        app->graphicsBackend.UploadShaderUniformInt(*shader, cornerBorder, "uCornerBorder");
        app->graphicsBackend.UploadShaderUniformInt(*shader, cornerLength, "uCornerLength");
        app->graphicsBackend.UploadShaderUniformVec4(*shader, borderColor.value, "uBorderColor");
        app->graphicsBackend.UploadShaderUniformVec4(*shader, cornerColor.value, "uCornerColor");
        glm::ivec2 widgetResolution = glm::ivec2(app->windowManager.primaryWindow->width * scale.x / (stretchWithAspectRatio ? 1.0f : app->windowManager.primaryWindow->aspect), app->windowManager.primaryWindow->height * scale.y);
        app->graphicsBackend.UploadShaderUniformIVec2(*shader, widgetResolution, "uWidgetResolution");
        app->graphicsBackend.UploadShaderUniformFloat(*shader, cameraRotation, "uCameraRotation");
        app->graphicsBackend.EndDrawMesh2D(*quad);
    }

    app->graphicsBackend.BeginDrawMesh2D(textMesh, *textShader, position, scale, rotation, z_distance, stretchWithAspectRatio, moveWithAspectRatio);
    app->graphicsBackend.UploadShaderUniformInt(*textShader, 0, "uFontTexture");
    app->graphicsBackend.UploadShaderUniformVec4(*textShader, fontColor.value, "uColor");
    app->graphicsBackend.UseTextureIDSlot(font.atlasTextureID, 0);
    app->graphicsBackend.EndDrawMesh2D(textMesh);

    app->graphicsBackend.SetDepthTest(true);
}

void RadarWidget::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    quad = &app->graphicsBackend.globalMeshes.quad;
    shader = &app->graphicsBackend.globalShaders.radar;

    playerWorldPositions.resize(MAX_PLAYERS_PER_LOBBY);
}

void RadarWidget::Draw() {
    FOX2_PROFILE_FUNCTION()
    std::unique_ptr<Application>& app = Application::GetInstance();

    std::vector<std::shared_ptr<Aircraft>> aircrafts = app->sceneManager.currentScene->GetEntitiesByType<Aircraft>();

    app->graphicsBackend.BeginDrawMesh2D(*quad, *shader, position, scale, rotation, z_distance, stretchWithAspectRatio, moveWithAspectRatio);
    int numAircrafts = aircrafts.size();
    glm::vec2 localClientPosition = glm::vec2(0.0f);
    float localClientRotation = 0.0f;

    if(numAircrafts > 0) {
        for (size_t i = 0; i < aircrafts.size(); i++) {
            playerWorldPositions[i] = {
                aircrafts[i]->transform.position.x,
                aircrafts[i]->transform.position.z
            };

            if(aircrafts[i]->networkId == app->networkManager.localClientId || !app->networkManager.connected) {
                localClientPosition = {
                    aircrafts[i]->transform.position.x,
                    aircrafts[i]->transform.position.z
                };
            }

            localClientRotation = app->sceneManager.activeCamera.GetYaw();

            app->graphicsBackend.UploadShaderUniformVec2(*shader, playerWorldPositions[i], "uPlayerWorldPositions[" + std::to_string(i) + "]");
        }
    }

    app->graphicsBackend.UploadShaderUniformInt(*shader, numAircrafts, "uPlayerCount");
    app->graphicsBackend.UploadShaderUniformVec2(*shader, localClientPosition, "uLocalClientPosition");
    app->graphicsBackend.UploadShaderUniformFloat(*shader, localClientRotation, "uLocalClientRotation");
    app->graphicsBackend.UploadShaderUniformVec4(*shader, color.value, "uColor");
    app->graphicsBackend.UploadShaderUniformInt(*shader, border, "uBorder");
    app->graphicsBackend.UploadShaderUniformInt(*shader, cornerBorder, "uCornerBorder");
    app->graphicsBackend.UploadShaderUniformInt(*shader, cornerLength, "uCornerLength");
    app->graphicsBackend.UploadShaderUniformVec4(*shader, borderColor.value, "uBorderColor");
    app->graphicsBackend.UploadShaderUniformVec4(*shader, cornerColor.value, "uCornerColor");
    glm::ivec2 widgetResolution = glm::ivec2(app->windowManager.primaryWindow->width * scale.x / (stretchWithAspectRatio ? 1.0f : app->windowManager.primaryWindow->aspect), app->windowManager.primaryWindow->height * scale.y);
    app->graphicsBackend.UploadShaderUniformIVec2(*shader, widgetResolution, "uWidgetResolution");
    app->graphicsBackend.UploadShaderUniformVec2(*shader, glm::vec2(TERRAIN_SIZE), "uTerrainSize");
    app->graphicsBackend.UploadShaderUniformInt(*shader, 0, "uTerrainHeightmap");
    app->graphicsBackend.UseTextureSlot(app->sceneManager.currentScene->GetEntityByName<Terrain>("terrain")->GetHeightMap(), 0);
    app->graphicsBackend.EndDrawMesh2D(*quad);
    app->graphicsBackend.ResetTextureSlots();
}

glm::vec2 AircraftWidgetLayer::UIAlignmentWithWorldPosition(glm::vec3 worldPosition) {
    std::unique_ptr<Application>& app = Application::GetInstance();
    glm::vec4 clipPosition = app->sceneManager.activeCamera.GetProjectionMatrix() * app->sceneManager.activeCamera.GetViewMatrix() * glm::vec4(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f);
    return glm::vec2(clipPosition.x / clipPosition.w, clipPosition.y / clipPosition.w);
}

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
    std::unique_ptr<Application>& app = Application::GetInstance();

    //aircraft aiming ui
    aim = CreateWidget<CircleWidget>("aimWidget");
    aim->color.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    aim->moveWithAspectRatio = true;

    mouse = CreateWidget<RectWidget>("mouseWidget");
    mouse->rotation = 45.0;
    mouse->scale = glm::vec2(0.02);
    mouse->color.value.a = 0.0;
    mouse->borderColor.value = glm::vec4(0.0);
    mouse->cornerLength = 7;
    mouse->cornerColor.value = glm::vec4(0.3, 1.0, 0.4, 1.0);

    lockWidget = CreateWidget<RectWidget>("lockWidget");
    lockWidget->moveWithAspectRatio = true;
    lockWidget->scale = glm::vec2(0.02);
    lockWidget->position = glm::vec2(2.0f, 0.0);
    lockWidget->color.value.a = 0.0;
    lockWidget->borderColor.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    lockWidget->cornerLength = 7;
    lockWidget->z_distance = -0.95f;
    lockWidget->cornerColor.value = glm::vec4(0.3, 1.0, 0.4, 1.0);

    lockNameWidget = CreateWidget<TextRectWidget>("lockNameWidget", app->graphicsBackend.globalFonts.defaultFont);
    lockNameWidget->moveWithAspectRatio = true;
    lockNameWidget->scale = glm::vec2(0.1f, 0.045f);
    lockNameWidget->position = glm::vec2(2.0f, 0.0);
    lockNameWidget->centerText = true;
    lockNameWidget->color.value.a = 0.0;
    lockNameWidget->borderColor.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    lockNameWidget->fontColor.value = glm::vec4(0.3, 1.0, 0.4, 1.0);
    lockNameWidget->cornerLength = 7;
    lockNameWidget->z_distance = -0.95f;
    lockNameWidget->cornerColor.value = glm::vec4(0.3, 1.0, 0.4, 1.0);

    std::shared_ptr<TextRectWidget> lobbyInfoRect = CreateWidget<TextRectWidget>("lobbyInfoRect", app->graphicsBackend.globalFonts.defaultFont);
    lobbyInfoRect->SetText("Lobby Id: " + std::to_string(app->networkManager.GetLobbyId()) + "\n");
    lobbyInfoRect->position = glm::vec2(0.8, 0.9);
    lobbyInfoRect->moveWithAspectRatio = true;
    lobbyInfoRect->scale = glm::vec2(0.28, 0.0425);
    lobbyInfoRect->color.value = glm::vec4(0.3, 0.3, 0.3, 0.3);
    lobbyInfoRect->borderColor.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    lobbyInfoRect->cornerColor.value = glm::vec4(0.3, 0.3, 0.3, 0.5);

    std::shared_ptr<TextRectWidget> settings = CreateWidget<TextRectWidget>("rect", app->graphicsBackend.globalFonts.defaultFont);
    settings->SetText("Left Alt: Settings");
    settings->position = glm::vec2(-0.8, 0.9);
    settings->moveWithAspectRatio = true;
    settings->scale = glm::vec2(0.275, 0.0425);
    settings->color.value = glm::vec4(0.3, 0.3, 0.3, 0.3);
    settings->borderColor.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    settings->cornerColor.value = glm::vec4(0.3, 0.3, 0.3, 0.5);

    //aircraft stats ui
    stats = CreateWidget<TextRectWidget>("stats",app->graphicsBackend.globalFonts.defaultFont);
    stats->moveWithAspectRatio = true;
    stats->scale = glm::vec2(0.4, 0.16);
    stats->position = glm::vec2(0.7, -0.75);
    stats->color.value = glm::vec4(0.3, 0.3, 0.3, 0.3);
    stats->borderColor.value = glm::vec4(0.3, 0.3, 0.3, 0.5);

    radar = CreateWidget<RadarWidget>("radar");
    radar->moveWithAspectRatio = true;
    radar->stretchWithAspectRatio = false;
    radar->scale = glm::vec2(0.3, 0.3);
    radar->position = glm::vec2(-0.8, -0.7);
    radar->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    radar->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);

    compass = CreateWidget<CompassWidget>("compass", app->graphicsBackend.globalFonts.defaultFont);
    compass->moveWithAspectRatio = true;
    compass->stretchWithAspectRatio = false;
    compass->scale = glm::vec2(1.0, 0.0425);
    compass->position = glm::vec2(0.0, 0.9);
    compass->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    compass->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    compass->font.fontScale = 1.0f;
    compass->SetText("                                0 deg");
}

void AircraftWidgetLayer::UpdateLayer() {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

    if(InputManager::IsKeyJustPressed(GLFW_KEY_LEFT_ALT)) {
        InputManager::mouseHidden = !InputManager::mouseHidden;
        glfwSetInputMode(app->windowManager.primaryWindow->window, GLFW_CURSOR, InputManager::mouseHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

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

Aircraft::~Aircraft() {
    std::unique_ptr<Application>& app = Application::GetInstance();
}

void Aircraft::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

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

    {
        FOX2_PROFILE_SCOPE("Aircraft Shader Load")
        shader = &app->graphicsBackend.globalShaders.aircraft;
    }
    {
        FOX2_PROFILE_SCOPE("Aircraft Mesh Load")
        skeletalMesh = Loader::LoadSkeletalMeshFromGLTF(resource.description.meshResourcePath.c_str());
    }

    transform.position.y = 12000.0f;

    {
        FOX2_PROFILE_SCOPE("exhaust and smoke particles")
        smokeParticles.emitting = false;
        smokeParticles.startAlbedo = glm::vec3(1.0f, 0.5f, 0.4f);
        smokeParticles.alpha = 0.7f;
        smokeParticles.scale = 10.0f;
        smokeParticles.scaleType = BIG_SMALL;
        smokeParticles.disableBackfaceCulling = false;

        exhaustParticles.LoadResources();
        smokeParticles.LoadResources();
    }
    {
        FOX2_PROFILE_SCOPE("trails")
        leftTrails.LoadResources();
        rightTrails.LoadResources();
    }

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
    smokeParticles.Initialize();

    leftTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipL);
    rightTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipR);
    leftTrails.Initialize();
    rightTrails.Initialize();

    if(networkId == app->networkManager.localClientId) {
        app->audioBackend.StartSoundAsset(engineSound, true, 0.3f);
    }

    skeletalMesh.skeleton.UpdateGlobalBoneTransforms();

    if(aircraftWidgetLayer) {
        aimWidget = static_pointer_cast<RectWidget>(aircraftWidgetLayer->GetWidgetByName("aimWidget"));
        mouseWidget = static_pointer_cast<CircleWidget>(aircraftWidgetLayer->GetWidgetByName("mouseWidget"));
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
    if(pendingDespawn) return;

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
            const float PITCH_LIMIT = (PI / 2.0f) - 0.01f;
            cameraRotationInputValue += targetDelta * app->clock.deltaTime * 100.0f;
            cameraRotationInputValue.y = std::clamp(cameraRotationInputValue.y, -PITCH_LIMIT, PITCH_LIMIT);
            camera.aspect = app->windowManager.primaryWindow->aspect;
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
            if(!InputManager::IsKeyPressed(GLFW_KEY_TAB) && InputManager::mouseHidden && !shotDown){
                uiDiff = MathUtils::Lerp<float>(uiDiff, aimWidget->position.x - mouseWidget->position.x, app->clock.deltaTime * 10.0f);
                targetRotation = glm::quatLookAt(-cameraForward, GLOBAL_UP);
            }
            else if(shotDown) {
                targetRotation = glm::quatLookAt(glm::normalize(lastPosition - transform.position), GLOBAL_UP);
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

            float maxDelta = resource.settings.maxTurnRate * (float)app->clock.deltaTime;
            float angle = glm::angle(unrolledRotation * glm::inverse(targetRotation));
            float maxDeltaAngle = MathUtils::Clamp<float>(maxDelta / angle, 0.0f, 1.0f);

            unrolledRotation = glm::slerp(unrolledRotation, targetRotation, (float)app->clock.deltaTime * maxDeltaAngle);
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

            if(shotDown) {
                controls.throttle = 0.001f;
            }

            controls.throttle = MathUtils::Clamp<float>(controls.throttle, 0.0f, 1.0f);

            //DEBUGGING FOR SHOOT DOWN
            if(InputManager::IsKeyJustPressed(GLFW_KEY_I)) {
                ShootDown();
            }

            app->audioBackend.SoundAssetSetPitch(engineSound, controls.throttle);
        }
        {
            FOX2_PROFILE_SCOPE("Stalling and Thrust Logic")
            transform.rotation = glm::normalize(unrolledRotation * extraRotation);

            if(!shotDown) {
                thrust = unrotatedForward * controls.throttle * resource.settings.maxThrust;
            }
            else {
                thrust = MathUtils::Lerp<glm::vec3>(thrust, glm::vec3(0.0f, -GRAVITY, 0.0f), app->clock.deltaTime * 0.1f);
            }

            glm::vec3 brake = (-thrust / 2.0f) * (targetBrakeAngle / resource.settings.brakeMaxAngle);
            glm::vec3 gravity = -GLOBAL_UP * GRAVITY;
            glm::vec3 lift = -gravity * terminalLiftFactor * (1.0f - glm::abs(glm::dot(aircraftForward, GLOBAL_UP)));

            glm::vec3 acceleration = thrust + gravity + lift + brake;
            velocity += acceleration * (float)app->clock.deltaTime;
            velocity *= expf(-DRAG_COEFFICIENT * (float)app->clock.deltaTime);

            speed = glm::length(velocity);

            lastPosition = transform.position;
            transform.position += velocity * (float)app->clock.deltaTime;
        }
        {
            FOX2_PROFILE_SCOPE("More Camera Controls")
            camera.target = transform.position + GLOBAL_UP * resource.settings.cameraRideHeight;
            float cameraZoom = InputManager::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2) ? resource.settings.cameraZoomDistance : resource.settings.cameraDistance;
            if(shotDown) {
                cameraZoom = resource.settings.cameraDistance * 4.0f;
            }
            camera.position = camera.target + GLOBAL_FORWARD * cameraZoom;
            glm::vec3 horizontalAxis = MathUtils::RotatePointAroundPoint(camera.position, camera.target, cameraRotationInputValue.y, -GLOBAL_LEFT);
            camera.position = MathUtils::RotatePointAroundPoint(horizontalAxis, camera.target, -cameraRotationInputValue.x, GLOBAL_UP);
        }
        {
            FOX2_PROFILE_SCOPE("Aircraft Target Locking")
            if(lockedAircraft == nullptr) {
                std::shared_ptr<RectWidget> lockWidget = aircraftWidgetLayer->lockWidget;
                std::shared_ptr<TextRectWidget> lockNameWidget = aircraftWidgetLayer->lockNameWidget;
                if(lockWidget != nullptr && lockNameWidget != nullptr) {
                    lockWidget->position = glm::vec2(2.0f, 0.0);
                    lockNameWidget->position = glm::vec2(2.0f, 0.075f);
                }

                for(std::shared_ptr<Aircraft> prospectiveTarget : app->sceneManager.currentScene->GetEntitiesByType<Aircraft>()) {
                    if(prospectiveTarget->id == id) {
                        continue;
                    }

                    glm::vec3 toVector = glm::normalize(prospectiveTarget->transform.position - transform.position);
                    float angle = glm::acos(glm::dot(aircraftForward, toVector));
                    if(angle <= PI/5) {
                        lockedAircraft = prospectiveTarget;
                        const std::string clientName = app->networkManager.networkGameState.clientStates[lockedAircraft->networkId].name;
                        lockNameWidget->scale.x = clientName.length() * FONT_CHAR_WIDTH_PIXELS;
                        lockNameWidget->SetText(clientName);
                        break;
                    }
                }
            }
            else {
                std::shared_ptr<RectWidget> lockWidget = aircraftWidgetLayer->lockWidget;
                std::shared_ptr<TextRectWidget> lockNameWidget = aircraftWidgetLayer->lockNameWidget;
                if(lockWidget != nullptr && lockNameWidget != nullptr) {
                    lockWidget->position = glm::clamp(aircraftWidgetLayer->UIAlignmentWithWorldPosition(lockedAircraft->transform.position), glm::vec2(-0.9f), glm::vec2(0.9f));
                    lockNameWidget->position = glm::clamp(aircraftWidgetLayer->UIAlignmentWithWorldPosition(lockedAircraft->transform.position) + glm::vec2(0.0f, 0.075f), glm::vec2(-0.9f), glm::vec2(0.9f));
                }

                glm::vec3 toVector = glm::normalize(lockedAircraft->transform.position - transform.position);
                float angle = glm::acos(glm::dot(aircraftForward, toVector));
                if(angle > PI/2) {
                    lockedAircraft = nullptr;
                }
            }
        }
        {
            FOX2_PROFILE_SCOPE("Animations")
            ApplyControlSurfaces(rollAngle);
            skeletalMesh.skeleton.UpdateGlobalBoneTransforms();
        }

        app->networkManager.networkGameState.clientStates[networkId].position = transform.position;
        app->networkManager.networkGameState.clientStates[networkId].rotation = transform.rotation;
        app->networkManager.networkGameState.clientStates[networkId].velocity = velocity;
        app->networkManager.networkGameState.clientStates[networkId].shotDown = shotDown;
        app->networkManager.networkGameState.clientStates[networkId].exploded = exploded;
    }
    else {
        if(app->networkManager.networkGameState.clientStates.contains(networkId)) {
            ClientState& clientState = app->networkManager.networkGameState.clientStates[networkId];

            float dt = app->clock.currentTime - app->networkManager.networkGameState.lastUpdateTimeStamp;
            glm::vec3 predictedPosition = clientState.position + clientState.velocity * dt;

            shotDown = clientState.shotDown;
            exploded = clientState.exploded;

            if(shotDown && !app->networkManager.lastNetworkGameState.clientStates[networkId].shotDown) {
                smokeParticles.emitting = true;
                exhaustParticles.emitting = false;
                app->sceneManager.currentScene->GetEntityByName<ExplosionSystemEntity>("explosionSystem")->SpawnExplosion(transform.position, SHOT_DOWN_EXPLOSION_SIZE, 0.5f);
            }
            else if(exploded && !app->networkManager.lastNetworkGameState.clientStates[networkId].exploded) {
                app->sceneManager.currentScene->GetEntityByName<ExplosionSystemEntity>("explosionSystem")->SpawnExplosion(transform.position, EXPLODE_EXPLOSION_SIZE, 0.5f);
            }


            transform.position = MathUtils::Lerp<glm::vec3>(transform.position, predictedPosition, (float)app->clock.deltaTime * app->networkManager.interpolationFactor);
            transform.rotation = glm::slerp(transform.rotation, clientState.rotation, (float)app->clock.deltaTime);
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
        smokeParticles.aircraftPosition = transform.position;
        smokeParticles.Update();

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

void Aircraft::Explode() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->sceneManager.currentScene->GetEntityByName<ExplosionSystemEntity>("explosionSystem")->SpawnExplosion(transform.position, EXPLODE_EXPLOSION_SIZE, 0.5f);

    if(networkId == app->networkManager.localClientId) {
        exploded = true;
        app->sceneManager.currentScene->RuntimeDespawn(shared_from_this());
        app->networkManager.networkGameState.clientStates[networkId].inGame = false;

        Timer timer;
        timer.endTime = 4.0f;
        timer.callback = [this, &app]() {
            std::shared_ptr<MenuWidgetLayer> menuWidgetLayer = app->sceneManager.currentScene->GetWidgetLayerByType<MenuWidgetLayer>();
            if(menuWidgetLayer) {
                InputManager::mouseHidden = false;
                glfwSetInputMode(app->windowManager.primaryWindow->window, GLFW_CURSOR, InputManager::mouseHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                menuWidgetLayer->invisible = false;
            }
        };

        app->clock.timers.push_back(timer);
    }
}

void Aircraft::ShootDown() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->sceneManager.currentScene->GetEntityByName<ExplosionSystemEntity>("explosionSystem")->SpawnExplosion(transform.position, SHOT_DOWN_EXPLOSION_SIZE, 0.5f);

    if(networkId == app->networkManager.localClientId) {
        smokeParticles.emitting = true;
        exhaustParticles.emitting = false;
        aircraftWidgetLayer->aim->radius = 0.0f;
        aircraftWidgetLayer->mouse->cornerBorder = 0;
        shotDown = true;

        Timer timer;
        timer.endTime = 15.0f;
        timer.callback = [this]() {
            Explode();
        };

        app->clock.timers.push_back(timer);
    }
}

void Aircraft::Draw()  {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

    //Note: here we only animate the local client's aircraft. Other clients' aircraft are static.
    app->graphicsBackend.BeginDrawSkeletalMesh(skeletalMesh, *shader, app->sceneManager.activeCamera, transform);
    app->graphicsBackend.UploadShaderUniformVec3(*shader, app->sceneManager.currentScene->environment.sunDirection, "uSunDirection");
    app->graphicsBackend.UploadShaderUniformVec3(*shader, app->sceneManager.activeCamera.position, "uCameraPosition");
    app->graphicsBackend.UploadShaderUniformInt(*shader, 0, "uAlbedoTexture");
    app->graphicsBackend.UseTextureSlot(skeletalMesh.textureMap["albedo"], 0);
    app->graphicsBackend.UploadShaderUniformInt(*shader, 1, "uEmmissionTexture");
    app->graphicsBackend.UseTextureSlot(skeletalMesh.textureMap["emmission"], 1);
    app->graphicsBackend.EndDrawSkeletalMesh(skeletalMesh);
    app->graphicsBackend.ResetTextureSlots();

    if(app->graphicsBackend.debugMode){
        Transform t = Transform();
        t.position = transform.position;
        t.rotation = transform.rotation;
        t.scale = glm::vec3(10.0);
        app->graphicsBackend.DrawDebugCube(app->sceneManager.activeCamera, COLOR_BLUE, t);
    }

    leftTrails.Draw();
    rightTrails.Draw();
    exhaustParticles.Draw();
    smokeParticles.Draw();

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

    exhaustParticles.UnloadResources();
    smokeParticles.UnloadResources();

    leftTrails.UnloadResources();
    rightTrails.UnloadResources();

    if(aircraftWidgetLayer) {
        aircraftWidgetLayer->UnloadResources();
    }
}


void AircraftSmokeParticleSystem::LoadResources() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    mesh = app->graphicsBackend.CreateQuad();
    mesh.material.albedo = startAlbedo;
    mesh.material.alpha = alpha;
    shader = &app->graphicsBackend.globalShaders.particles;
}

void AircraftSmokeParticleSystem::Initialize() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    glm::mat4 transformationMatrices[MAX_PARTICLE_TRANSFORMS];
    for(size_t i = 0; i < MAX_PARTICLE_TRANSFORMS; i++) {
        transforms[i] = Transform();
        transforms[i].position = aircraftPosition;
        transformationMatrices[i] = transforms[i].GetMatrix();
    }

    app->graphicsBackend.UploadInstancedMeshTransforms(mesh, transformationMatrices, MAX_PARTICLE_TRANSFORMS);
}

void AircraftSmokeParticleSystem::Update() {
    FOX2_PROFILE_FUNCTION()

    std::unique_ptr<Application>& app = Application::GetInstance();

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
        switch(scaleType) {
            case SMALL_BIG_SMALL:
                transforms[i].scale = glm::vec3(scale * (1.0 - (abs(particleLifetimes[i] - (particleStartLifetime / 2.0f)) / (particleStartLifetime / 2.0))));
                break;
            case SMALL_BIG:
                transforms[i].scale = glm::vec3(scale * ((abs(particleLifetimes[i] - (particleStartLifetime)) / particleStartLifetime)));
                break;
            case BIG_SMALL:
                transforms[i].scale = glm::vec3(scale * (1.0 - (abs(particleLifetimes[i] - (particleStartLifetime)) / particleStartLifetime)));
                break;
            case BIG:
                transforms[i].scale = glm::vec3(scale);
                break;
                break;
        }
    }

    glm::mat4 transformationMatrices[MAX_PARTICLE_TRANSFORMS];
    for(size_t i = 0; i < MAX_PARTICLE_TRANSFORMS; i++) {
        transformationMatrices[i] = transforms[i].GetMatrix();
    }

    app->graphicsBackend.UpdateInstancedMeshTransforms(mesh, transformationMatrices, MAX_PARTICLE_TRANSFORMS);
}

void AircraftSmokeParticleSystem::Draw() {
    FOX2_PROFILE_FUNCTION()

    if(!emitting) return;

    std::unique_ptr<Application>& app = Application::GetInstance();

    if(disableBackfaceCulling) {
        app->graphicsBackend.SetBackfaceCulling(false);
    }
    app->graphicsBackend.BeginDrawMeshInstanced(mesh, *shader, app->sceneManager.activeCamera, transforms, MAX_PARTICLE_TRANSFORMS);

    for(size_t i = 0; i < MAX_PARTICLE_TRANSFORMS; i++) {
        app->graphicsBackend.UploadShaderUniformVec3(*shader, glm::mix(startAlbedo, endAlbedo, (float)pow(1.0 - particleLifetimes[i] / particleStartLifetime, 0.2f)), "uAlbedos[" + std::to_string(i) + "]");
    }

    app->graphicsBackend.EndDrawMeshInstanced(mesh, MAX_PARTICLE_TRANSFORMS);
    if(disableBackfaceCulling) {
        app->graphicsBackend.SetBackfaceCulling(true);
    }
}

void AircraftSmokeParticleSystem::UnloadResources() {
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
    FOX2_PROFILE_FUNCTION()

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
