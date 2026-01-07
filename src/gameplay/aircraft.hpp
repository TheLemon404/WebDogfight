#pragma once

#include "../graphics/types.hpp"
#include "../graphics/loader.hpp"
#include "../graphics/backend.hpp"
#include "../io/input.hpp"
#include "../io/time.hpp"
#include "GLFW/glfw3.h"
#include "entity.hpp"
#include "scene_manager.hpp"
#include "../graphics/window.hpp"

#include <memory>
#include <future>
#include <iostream>

class Aircraft : public Entity {
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

    void LoadResources() override {
        shader = GraphicsBackend::CreateShader("resources/shaders/skeletal.glsl");
        skeletalMesh = Loader::LoadSkeletalMeshFromGLTF("resources/meshes/demo_jet.gltf");
        camera = Camera();
        camera.position = glm::vec3(10.0f, 10.0f, 10.0f);
        camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void Initialize() override {
    }

    void Update() override {
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
            skeletalMesh.skeleton.bones[1].RotateLocal(glm::vec3(0.0, 1.0, 0.0), 1.0f);
            skeletalMesh.skeleton.bones[2].RotateLocal(glm::vec3(1.0, 0.0, 0.0), 1.0f);
        }

        if(InputManager::IsKeyPressed(GLFW_KEY_D)) {
            skeletalMesh.skeleton.bones[3].RotateLocal(glm::vec3(0.0, 1.0, 0.0), 1.0f);
            skeletalMesh.skeleton.bones[0].RotateLocal(glm::vec3(1.0, 0.0, 0.0), 1.0f);
        }
    }

    void Draw() override {
        GraphicsBackend::BeginDrawSkeletalMesh(skeletalMesh, shader, camera, transform);
        GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunDirection, "uSunDirection");
        GraphicsBackend::UploadShaderUniformVec3(shader, SceneManager::currentScene->environment.sunColor, "uSunColor");
        GraphicsBackend::EndDrawSkeletalMesh(skeletalMesh);
    }

    void UnloadResources() override {
        GraphicsBackend::DeleteSkeletalMesh(skeletalMesh);
        GraphicsBackend::DeleteShader(shader);
    }
};
