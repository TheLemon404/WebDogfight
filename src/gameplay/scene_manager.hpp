#pragma once

#include "scene.hpp"
#include <memory>

class SceneManager {
    public:
    inline static Camera activeCamera = Camera();
    inline static std::shared_ptr<Scene> currentScene;

    static void ChangeToGameScene();
};
