#include "scene_manager.hpp"
#include "test_scene.hpp"

void SceneManager::ChangeToGameScene() {
    currentScene->UnloadResources();

    currentScene = std::make_shared<Scene>(TestScene::Create());
    currentScene->LoadResources();
    currentScene->Initialize();

    isChangingToGameScene = false;
}

void SceneManager::FutureChangeToGameScene() {
    isChangingToGameScene = true;
}

void SceneManager::Update() {
    if (isChangingToGameScene) {
        ChangeToGameScene();
    }
}
