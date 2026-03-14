#pragma once

#include "scene.hpp"
#include <memory>

class SceneManager {
    static void ChangeToGameScene();

    public:
    inline static std::shared_ptr<Scene> asyncLoadingScene = nullptr;

    inline static bool isChangingToGameScene = false;
    inline static bool isFinishedLoading = false;
    inline static void SwapSceneAfterAsyncLoad();

    inline static Camera activeCamera = Camera();
    inline static std::shared_ptr<Scene> currentScene = nullptr;

    static void Update();
};
