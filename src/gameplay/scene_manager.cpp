#include "scene_manager.hpp"
#include "scene.hpp"
#include "test_scene.hpp"
#include <memory>

void SceneManager::ChangeToGameScene() {
    asyncLoadingScene = std::make_shared<Scene>(TestScene::Create());
    asyncLoadingScene->onResourcesLoadedCallback = [] {
        isFinishedLoading = true;
    };
    asyncLoadingScene->LoadResourcesAsync();
}

void SceneManager::Update() {
    if (isChangingToGameScene) {
        isChangingToGameScene = false;
        ChangeToGameScene();
    }
    else if(asyncLoadingScene && asyncLoadingScene->isLoadingResources) {
        asyncLoadingScene->LoadResourcesAsync();
    }
    else if(isFinishedLoading) {
        if(!asyncLoadingScene) return;
        currentScene->UnloadResources();
        currentScene = asyncLoadingScene;
        asyncLoadingScene = nullptr;
        currentScene->Initialize();
        isFinishedLoading = false;
    }
}
