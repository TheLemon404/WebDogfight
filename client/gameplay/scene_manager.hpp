#pragma once

#include "scene.hpp"
#include <memory>

class SceneManager {
    static void ChangeToGameScene();
    inline static bool isChangingToGameScene = false;

    public:
    inline static Camera activeCamera = Camera();
    inline static std::shared_ptr<Scene> currentScene;

    static void FutureChangeToGameScene();

    static void Update();
};
