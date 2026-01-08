#pragma once

#include "../graphics/types.hpp"
#include "entity.hpp"
#include <string>
#include <memory>
#include <future>
#include <iostream>

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
    AircraftResourceDescriptionBoneMappings boneMappings;
};

struct AircraftResourceSettings {
    float flapsMaxAngle;
    float brakeMaxAngle;
    float tailMaxAngle;
    float rudderMaxAngle;
    float maxSpeed;
    float cameraRideHeight;
    float cameraLagDistance;
    float controlSurfaceTweenStep;
};

struct AircraftResource {
    AircraftResourceDescription description;
    AircraftResourceSettings settings;
};

struct AircraftControls {
    float throttle = 0.0;
};

struct AircraftPhysicsBody {
    float forwardVelocity = 0.0;
};

class Aircraft : public Entity {
    const std::string resourcePath;
    AircraftResource resource;
    AircraftControls controls;
    AircraftPhysicsBody physicsBody;

    float targetRoll = 0.0f;

    glm::vec3 targetRotation;

    void ApplyControlSurfaces();

    public:
    Shader shader;
    SkeletalMesh skeletalMesh;
    Camera camera;
    Transform transform;

    glm::vec3 RotatePointAroundPoint(
        const glm::vec3& pointToRotate,
        const glm::vec3& center,
        float angleRadians,
        const glm::vec3& axis)
    {
        // 1. Translate the point to the origin (relative to the center)
        glm::vec3 translatedPoint = pointToRotate - center;

        // 2. Create the rotation quaternion
        glm::quat rotationQuat = glm::angleAxis(angleRadians, axis);

        // 3. Apply the rotation using quaternion multiplication
        glm::vec3 rotatedPoint = rotationQuat * translatedPoint;

        // 4. Translate the point back to its original position (relative to the center)
        glm::vec3 finalPoint = rotatedPoint + center;

        return finalPoint;
    }

    Aircraft(const std::string& aircraftResourcePath) : resourcePath(aircraftResourcePath) {};

    void LoadResources() override;
    void Initialize() override {}
    void Update() override;
    void Draw() override;
    void UnloadResources() override;
};
