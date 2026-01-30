#include "gameplay/menu_scene.hpp"
#include "gameplay/test_scene.hpp"
#include "gameplay/scene_manager.hpp"
#include "graphics/loader.hpp"
#include "io/input.hpp"
#include "io/time.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "graphics/backend.hpp"
#include "graphics/window.hpp"
#include <memory>

#include "audio/backend.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#ifdef __EMSCRIPTEN__
EM_JS(int, html_get_width, (), {
    return window.innerWidth;
})

EM_JS(int, html_get_height, (), {
    return window.innerHeight;
})
#endif

void main_loop() {
    WindowManager::primaryWindow->Poll();

    //--- IMPORTANT --- remove this line from production. Users should not be able to see debug graphics
    if(InputManager::IsKeyJustPressed(GLFW_KEY_P)) GraphicsBackend::debugMode = !GraphicsBackend::debugMode;

    GraphicsBackend::ResetState(WindowManager::primaryWindow->width, WindowManager::primaryWindow->height);
    GraphicsBackend::SetDepthTest(true);

    SceneManager::currentScene->Update();
    SceneManager::currentScene->Draw();

    WindowManager::primaryWindow->SwapBuffers();
    InputManager::ResetInputState();
    Time::Tick();
}

int main() {
    WindowManager::primaryWindow = std::make_shared<Window>();
#ifdef __EMSCRIPTEN__
    WindowManager::primaryWindow->width = html_get_width();
    WindowManager::primaryWindow->height = html_get_height();
    WindowManager::aspect = static_cast<float>(WindowManager::primaryWindow->width)/WindowManager::primaryWindow->height;
#else
    WindowManager::primaryWindow->width = 1600;
    WindowManager::primaryWindow->height = 1000;
    WindowManager::aspect = static_cast<float>(1600)/1000;
#endif
    WindowManager::primaryWindow->title = "Fox2";

    WindowManager::aspect = static_cast<float>(WindowManager::primaryWindow->width) / WindowManager::primaryWindow->height;

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

    return 0;
}
