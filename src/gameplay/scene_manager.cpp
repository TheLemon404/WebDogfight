#include "scene_manager.hpp"
#include "scene.hpp"
#include "test_scene.hpp"
#include <memory>

void SceneManager::CheckSceneChange() {
    if(asyncSceneChangeState) {
        if(asyncSceneChangeState->asyncLoadingScene->isLoadingResources) {
            asyncSceneChangeState->asyncLoadingScene->LoadResourcesAsync();
        }
        else if(asyncSceneChangeState->isFinishedLoading) {
            currentScene->UnloadResources();
            currentScene = asyncSceneChangeState->asyncLoadingScene;
            asyncSceneChangeState = nullptr;
            currentScene->Initialize();
        }
    }

    switch(sceneChangeState) {
        case NONE:
            break;
        case GAME_SCENE:
            asyncSceneChangeState = std::make_unique<AsyncSceneChangeState>(
                std::make_shared<Scene>(TestScene::Create()),
                false
            );
            asyncSceneChangeState->asyncLoadingScene->onResourcesLoadedCallback = [this] {
                asyncSceneChangeState->isFinishedLoading = true;
            };
            asyncSceneChangeState->asyncLoadingScene->LoadResourcesAsync();
            sceneChangeState = NONE;
            break;
        case MENU_SCENE:
            //TODO -- impliment menu scene swapping
            break;
    }
}
