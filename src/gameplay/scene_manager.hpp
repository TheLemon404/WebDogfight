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
    std::unique_ptr<AsyncSceneChangeState> asyncSceneChangeState = nullptr;

    SceneChangeState sceneChangeState = NONE;

    Camera activeCamera = Camera();
    std::shared_ptr<Scene> currentScene = nullptr;

    void CheckSceneChange();
};
