#pragma once

#include "scene.hpp"
#include <memory>

enum SceneChangeState {
    NONE,
    GAME_SCENE,
    MENU_SCENE,
};

struct AsyncSceneChangeState {
    std::shared_ptr<Scene> asyncLoadingScene = nullptr;
    bool isFinishedLoading = false;
};

class SceneManager {
    public:
    inline static std::unique_ptr<AsyncSceneChangeState> asyncSceneChangeState = nullptr;

    inline static SceneChangeState sceneChangeState = NONE;

    inline static Camera activeCamera = Camera();
    inline static std::shared_ptr<Scene> currentScene = nullptr;

    static void CheckSceneChange();
};
