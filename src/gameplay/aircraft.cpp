#include "aircraft.hpp"
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
#include <math.h>
#include <nlohmann/json.hpp>

#define BRAKE_ANGLE_LERP_TIME 1.0

#define YAW_ROTATION 15
#define ROLL_ROTATION 25
#define PITCH_ROTATION 25

#define GRAVITY 10000.0f
#define DRAG_COEFFICIENT 50.0f
#define GFORCE_COEFFICIENT 150.0f
#define GFORCE_BODY_THRESHOLD 7
#define GFORCE_TRAIL_THRESHOLD 9

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

    exhaustParticles = AircraftExhaustParticleSystem();
    exhaustParticles.LoadResources();

    leftTrails = AircraftTrails();
    leftTrails.LoadResources();

    rightTrails = AircraftTrails();
    rightTrails.LoadResources();

    AudioBackend::LoadSound("resources/audio/engine.wav", engineSound);
}

void Aircraft::Initialize() {
    aimWidget = SceneManager::currentScene->GetWidgetByName("aimWidget");
    mouseWidget = SceneManager::currentScene->GetWidgetByName("mouseWidget");

    skeletalMesh.material.albedo = glm::vec3(0.7f);

    exhaustParticles.Initialize();

    leftTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipL);
    rightTrails.aircraftPosition = transform.position + (transform.rotation * resource.settings.wingTipR);
    leftTrails.Initialize();
    rightTrails.Initialize();

    AudioBackend::StartSoundAsset(engineSound, true, 0.3f);
}

void Aircraft::ApplyControlSurfaces(float roll) {
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
        targetBrakeAngle = MathUtils::Lerp<float>(targetBrakeAngle, resource.settings.brakeMaxAngle, Time::deltaTime * BRAKE_ANGLE_LERP_TIME);
    }
    else {
        targetBrakeAngle = MathUtils::Lerp<float>(targetBrakeAngle, 0, Time::deltaTime * BRAKE_ANGLE_LERP_TIME);
    }

    skeletalMesh.skeleton.bones[resource.description.boneMappings.brake].SetLocalRotation(glm::vec3(1.0, 0.0, 0.0), targetBrakeAngle);

    skeletalMesh.skeleton.bones[resource.description.boneMappings.burner].scale.y = pow(controls.throttle, 15);

    float pressureScale = MathUtils::Max<float>(MathUtils::Min<float>(gForce - GFORCE_BODY_THRESHOLD, 0.0f), 1.0f);
    skeletalMesh.skeleton.bones[resource.description.boneMappings.pressureVorticesL].scale = glm::vec3(pressureScale);
    skeletalMesh.skeleton.bones[resource.description.boneMappings.pressureVorticesR].scale = glm::vec3(pressureScale);
}

void Aircraft::Update() {
    FOX2_PROFILE_FUNCTION();
    Camera& camera = SceneManager::activeCamera;

    glm::vec3 cameraForward;
    {
        FOX2_PROFILE_SCOPE("Camera Movement")
        //camera controls
        //this ugly one-liner makes for smooth camera rotation
        cameraRotationInputValue += InputManager::mouseDelta / 500.0;
        camera.aspect = static_cast<float>(WindowManager::primaryWindow->width) / WindowManager::primaryWindow->height;
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
            uiDiff = MathUtils::Lerp<float>(uiDiff, aimWidget->position.x - mouseWidget->position.x, Time::deltaTime * 10.0f);
            targetRotation = glm::quatLookAt(-cameraForward, GLOBAL_UP);
        }
        if(InputManager::IsKeyPressed(GLFW_KEY_Q)) {
            rollInput -= resource.settings.rollRate * Time::deltaTime;
            restingRollRotation = 0.0f;
        }
        else if(InputManager::IsKeyPressed(GLFW_KEY_E)) {
            rollInput += resource.settings.rollRate * Time::deltaTime;
            restingRollRotation = 2.0f * PI;
        }
        else {
            rollInput = fmodf(rollInput, 2.0 * PI);
            rollInput = MathUtils::Lerp<float>(rollInput, restingRollRotation, Time::deltaTime * 2.0f);
        }
        rollAngle = MathUtils::Clamp<float>(-uiDiff * resource.settings.rollMagnifier, glm::radians(-90.0f), glm::radians(90.0f));
        extraRotation = glm::angleAxis(rollAngle + rollInput, GLOBAL_FORWARD);
        unrolledRotation = glm::slerp(unrolledRotation, targetRotation, (float)Time::deltaTime);
        unrotatedForward = glm::normalize(glm::rotate(unrolledRotation, GLOBAL_FORWARD));

        glm::vec3 velocityChange = velocity - lastVelocity;
        float lateralAcceleration = glm::length(velocityChange - glm::dot(velocityChange, unrotatedForward) * unrotatedForward);

        gForce = MathUtils::Lerp<float>(gForce, lateralAcceleration / (GFORCE_COEFFICIENT * Time::deltaTime), Time::deltaTime * 5.0f);

        lastVelocity = velocity;

        lastRotation = unrolledRotation;
    }
    {
        FOX2_PROFILE_SCOPE("Throttle Controls and Audio")
        if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
            controls.throttle += resource.settings.throttleIncreaseRate * Time::deltaTime;
        }
        else if(InputManager::IsKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
            controls.throttle -= resource.settings.throttleIncreaseRate * Time::deltaTime;
        }
        controls.throttle = MathUtils::Clamp<float>(controls.throttle, 0.0f, 1.0f);

        AudioBackend::SoundAssetSetPitch(engineSound, controls.throttle);
    }
    {
        FOX2_PROFILE_SCOPE("Stalling and Thrust Logic")
        transform.rotation = glm::normalize(unrolledRotation * extraRotation);
        glm::vec3 thrust = unrotatedForward * controls.throttle * resource.settings.maxThrust;
        glm::vec3 brake = (-thrust / 2.0f) * (targetBrakeAngle / resource.settings.brakeMaxAngle);
        glm::vec3 gravity = -GLOBAL_UP * GRAVITY;
        glm::vec3 lift = -gravity * terminalLiftFactor;

        glm::vec3 acceleration = thrust + gravity + lift + brake - (velocity * DRAG_COEFFICIENT);
        velocity += acceleration * (float)Time::deltaTime;

        speed = glm::length(velocity);

        lastPosition = transform.position;
        transform.position += velocity * (float)Time::deltaTime;
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
}

void Aircraft::Draw()  {
    FOX2_PROFILE_SCOPE("Aircraft Draw")

    GraphicsBackend::BeginDrawSkeletalMesh(skeletalMesh, shader, SceneManager::activeCamera, transform);
    GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunDirection, "uSunDirection");
    GraphicsBackend::EndDrawSkeletalMesh(skeletalMesh);

    if(GraphicsBackend::debugMode){
        Transform t = Transform();
        t.position = transform.position;
        t.rotation = transform.rotation;
        t.scale = glm::vec3(10.0);
        GraphicsBackend::DrawDebugCube(SceneManager::activeCamera, t);
    }

    leftTrails.Draw();
    rightTrails.Draw();
    exhaustParticles.Draw();
}

void Aircraft::UnloadResources()  {
    AudioBackend::EndSoundAsset(engineSound);

    GraphicsBackend::DeleteSkeletalMesh(skeletalMesh);
    GraphicsBackend::DeleteShader(shader);

    exhaustParticles.UnloadResources();

    leftTrails.UnloadResources();
    rightTrails.UnloadResources();
}

glm::vec2 AircraftWidgetLayer::UIAlignmentWithRotation(glm::quat rotation) {
    glm::vec3 aircraftForwardVector = glm::normalize(glm::rotate(rotation, GLOBAL_FORWARD));
    glm::vec3 aircraftUpVector = glm::normalize(glm::rotate(rotation, GLOBAL_UP));
    glm::vec3 aircraftLeftVector = glm::normalize(glm::rotate(rotation, GLOBAL_LEFT));

    glm::vec3 cameraForward = glm::normalize(SceneManager::activeCamera.target - SceneManager::activeCamera.position);
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
    std::shared_ptr<TextRectWidget> rect = std::make_shared<TextRectWidget>("rect", GraphicsBackend::globalFonts.defaultFont);
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
    stats = std::make_shared<TextRectWidget>("stats", GraphicsBackend::globalFonts.defaultFont);
    stats->moveWithAspectRatio = true;
    stats->scale = glm::vec2(0.4, 0.13);
    stats->position = glm::vec2(0.6, -0.8);
    stats->color.value = glm::vec4(0.3, 0.3, 0.3, 0.5);
    stats->borderColor.value = glm::vec4(1.0, 1.0, 1.0, 0.5);
    widgets.push_back(stats);

    aircraft = std::static_pointer_cast<Aircraft>(SceneManager::currentScene->GetEntityByName("FA-XX"));
}

void AircraftWidgetLayer::UpdateLayer() {
    FOX2_PROFILE_FUNCTION()
    glm::vec2 targetDelta = glm::vec2(InputManager::mouseDelta.x / 1000.0f,
        #ifdef __EMSCRIPTEN__
        InputManager::mouseDelta.y / 1000.0
        #else
        -InputManager::mouseDelta.y / 1000.0
        #endif
    );

    mouse->position += targetDelta * Time::deltaTime * 70.0f;
    mouse->position *= 0.95f;
    mouse->position = glm::clamp(mouse->position, glm::vec2(-4.0f), glm::vec2(4.0f));

    aim->position = UIAlignmentWithRotation(aircraft->unrolledRotation);
    aim->position.x /= WindowManager::aspect;
    glm::vec3 aircraftForwardVector = glm::normalize(glm::rotate(aircraft->transform.rotation, GLOBAL_FORWARD));
    glm::vec3 cameraForward = glm::normalize(SceneManager::activeCamera.target - SceneManager::activeCamera.position);
    float dot = glm::dot(cameraForward, aircraftForwardVector);

    aim->color.value.a = dot;

    stats->SetText("FPS: " + std::to_string(1/Time::deltaTime) + "\n"
        "Throttle: " + std::to_string(aircraft->controls.throttle) + "\n"
        "Speed: " + std::to_string(glm::length(aircraft->velocity)) + "\n"
        "G-Force: " + std::to_string(glm::length(aircraft->gForce)) + "\n");
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
    FOX2_PROFILE_FUNCTION()

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

void AircraftTrails::GenerateMesh() {
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

    GraphicsBackend::UpdateMeshVertices(mesh, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void AircraftTrails::RecomputeMesh() {
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
        vertexLifetime -= Time::deltaTime;
    }

    GraphicsBackend::UpdateMeshVertices(mesh, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void AircraftTrails::LoadResources() {
    shader = &GraphicsBackend::globalShaders.trails;
    mesh = GraphicsBackend::CreateQuad();
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
    Transform t = Transform();
    GraphicsBackend::SetBackfaceCulling(false);
    GraphicsBackend::BeginDrawMesh(mesh, *shader, SceneManager::activeCamera, t, false);
    GraphicsBackend::EndDrawMesh(mesh);
    GraphicsBackend::SetBackfaceCulling(true);
}

void AircraftTrails::UnloadResources() {
    GraphicsBackend::DeleteMesh(mesh);
}
