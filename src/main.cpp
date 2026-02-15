#include "gameplay/menu_scene.hpp"
#include "gameplay/test_scene.hpp"
#include "gameplay/scene_manager.hpp"
#include "graphics/loader.hpp"
#include "io/input.hpp"
#include "io/time.hpp"
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

void main_loop() {
    FOX2_PROFILE_FUNCTION()

    {
        FOX2_PROFILE_SCOPE("Input Polling")
        WindowManager::primaryWindow->Poll();

        //--- IMPORTANT --- remove this line from production. Users should not be able to see debug graphics
        if(InputManager::IsKeyJustPressed(GLFW_KEY_P)) GraphicsBackend::debugMode = !GraphicsBackend::debugMode;
    }
    {
        FOX2_PROFILE_SCOPE("Graphics State Reset")
        GraphicsBackend::ResetState(WindowManager::primaryWindow->width, WindowManager::primaryWindow->height);
        GraphicsBackend::SetDepthTest(true);
    }
    {
        FOX2_PROFILE_SCOPE("Scene Update")
        SceneManager::currentScene->Update();
    }
    {
        FOX2_PROFILE_SCOPE("Scene Draw")
        SceneManager::currentScene->Draw();
    }
    {
        FOX2_PROFILE_SCOPE("Swap buffers and Tick")
        WindowManager::primaryWindow->SwapBuffers();
        InputManager::ResetInputState();
        Time::Tick();
    }

    SceneManager::Update();
}

int main() {
    FOX2_PROFILE_BEGIN_SESSION("Fox2", "startup.json")
    WindowManager::primaryWindow = std::make_shared<Window>();
    WindowManager::primaryWindow->title = "Fox2";

    SceneManager::currentScene = std::make_shared<Scene>(MenuScene::Create());
    WindowManager::primaryWindow->Open();

    AudioBackend::Initialize();

    GraphicsBackend::LoadResources();

    SceneManager::currentScene->LoadResources();
    SceneManager::currentScene->Initialize();

    GraphicsBackend::SetBackfaceCulling(true);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while(!WindowManager::primaryWindow->ShouldClose()) {
        main_loop();
    }
#endif

    AudioBackend::Shutdown();
    SceneManager::currentScene->UnloadResources();
    GraphicsBackend::UnloadResources();
    WindowManager::primaryWindow->Close();
    FOX2_PROFILE_END_SESSION()

    return 0;
}
