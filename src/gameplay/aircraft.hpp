#pragma once

#include "../graphics/types.hpp"
#include "entity.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "widget.hpp"
#include <string>
#include <memory>
#include <future>
#include <iostream>

#define BODY_PRESSURE_NUM_PARTICLES 5

struct AircraftResourceDescriptionBoneMappings {
    int root;
    int brake;
    int wingL;
    int wingR;
    int tailL;
    int tailR;
    int rudderL;
    int rudderR;
};

struct AircraftResourceDescription {
    std::string name;
    std::string shaderResourcePath;
    std::string meshResourcePath;
    AircraftResourceDescriptionBoneMappings boneMappings;
};

struct AircraftResourceSettings {
    float flapsMaxAngle;
    float brakeMaxAngle;
    float tailMaxAngle;
    float rudderMaxAngle;
    float throttleIncreaseRate;
    float throttleCruise;
    float maxSpeed;
    float cameraRideHeight;
    float cameraDistance;
    float cameraZoomDistance;
    float controlSurfaceTweenStep;
    float rollMagnifier;
    float rollRate;
};

struct AircraftResource {
    AircraftResourceDescription description;
    AircraftResourceSettings settings;
};

struct AircraftControls {
    float throttle = 0.5;
};

struct AircraftPhysicsBody {
    float forwardVelocity = 0.0;
};

class AircraftExhaustParticleSystem : public ParticleSystem {
    float particleLifetimes[MAX_PARTICLE_TRANSFORMS];
    float particleStartLifetime = 1.0;

    public:
    glm::vec3 aircraftPosition;

    void LoadResources() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void UnloadResources() override;

    AircraftExhaustParticleSystem() {
        for(size_t i = 1; i < MAX_PARTICLE_TRANSFORMS; i++){
            particleLifetimes[i] = particleStartLifetime * ((float)i / (MAX_PARTICLE_TRANSFORMS - 1.0));
        }
    }
};

class Aircraft : public Entity {
    const std::string resourcePath;
    AircraftResource resource;
    AircraftControls controls;
    AircraftPhysicsBody physicsBody;

    std::shared_ptr<Widget> aimWidget;
    std::shared_ptr<Widget> mouseWidget;

    glm::vec2 smoothedMouseDelta = glm::vec2(0.0);

    float targetBrakeAngle = 0.0f;

    glm::vec2 cameraRotationInputValue = glm::vec2(0.0);

    glm::quat targetRotation;

    void ApplyControlSurfaces();

    float rollValue = 0.0f;
    float uiDiff = 0.0f;

    const glm::quat downQuaternion = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    AircraftExhaustParticleSystem exhaustParticles;

    public:
    float appliedForce = 0.0;

    Shader shader;
    SkeletalMesh skeletalMesh;
    Transform transform;

    glm::quat unrolledRotation = glm::identity<glm::quat>();

    Aircraft(const std::string& name, const std::string& aircraftResourcePath) : Entity(name), resourcePath(aircraftResourcePath) {};

    void LoadResources() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void UnloadResources() override;
};

class AircraftWidgetLayer : public WidgetLayer {
    std::shared_ptr<Aircraft> aircraft;
    std::shared_ptr<CircleWidget> aim;
    std::shared_ptr<RectWidget> mouse;
    glm::vec2 UIAlignmentWithRotation(glm::quat rotation);
    void CreateWidgets() override;
    void UpdateLayer() override;
};
