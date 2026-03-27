#include "gameplay/test_scene.hpp"
#include "gameplay/scene_manager.hpp"
#include "graphics/loader.hpp"
#include "io/input.hpp"
#include "io/time.hpp"
#include "networking/network_manager.hpp"
#include "utils/instrumentor.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "graphics/backend.hpp"
#include "graphics/window.hpp"
#include <memory>
#include "audio/backend.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "application.hpp"

void main_loop() {
    FOX2_PROFILE_FUNCTION()
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->sceneManager.CheckSceneChange();

    {
        FOX2_PROFILE_SCOPE("Input Polling")
        app->windowManager.primaryWindow->Poll();

        //--- IMPORTANT --- remove this line from production. Users should not be able to see debug graphics
        if(InputManager::IsKeyJustPressed(GLFW_KEY_P)) app->graphicsBackend.debugMode = !app->graphicsBackend.debugMode;
    }
    {
        FOX2_PROFILE_SCOPE("Graphics State Reset")
        app->graphicsBackend.ResetState(app->windowManager.primaryWindow->width, app->windowManager.primaryWindow->height);
        app->graphicsBackend.SetDepthTest(true);
    }
    {
        FOX2_PROFILE_SCOPE("Scene Update")
        app->sceneManager.currentScene->Update();
    }
    {
        FOX2_PROFILE_SCOPE("Scene Draw")
        app->sceneManager.currentScene->Draw();
    }
    {
        FOX2_PROFILE_SCOPE("Swap buffers and Tick")
        app->networkManager.Tick();
        app->windowManager.primaryWindow->SwapBuffers();
        InputManager::ResetInputState();
        app->clock.Tick();
    }
}

int main() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    app->windowManager.primaryWindow = std::make_shared<Window>();
    app->windowManager.primaryWindow->title = "Fox2";

    app->sceneManager.currentScene = std::make_shared<Scene>(TestScene::Create());
    app->windowManager.primaryWindow->Open();

    app->audioBackend.Initialize();

    app->graphicsBackend.LoadResources();

    app->networkManager.Initialize();
    app->networkManager.ConnectToServer();

    app->sceneManager.currentScene->LoadResources();
    app->sceneManager.currentScene->Initialize();

    app->graphicsBackend.SetBackfaceCulling(true);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while(!app->windowManager.primaryWindow->ShouldClose()) {
        main_loop();
    }
#endif

    app->sceneManager.currentScene->UnloadResources();
    app->audioBackend.Shutdown();
    app->networkManager.Shutdown();
    app->graphicsBackend.UnloadResources();
    app->windowManager.primaryWindow->Close();

    return 0;
}
