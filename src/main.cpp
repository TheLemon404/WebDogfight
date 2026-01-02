#include "gameplay/test_scene.hpp"
#include "gameplay/scene_manager.hpp"
#include "io/time.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "graphics/backend.hpp"
#include "graphics/window.hpp"
#include <memory>

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
#else
    WindowManager::primaryWindow->width = 800;
    WindowManager::primaryWindow->height = 500;
#endif
    WindowManager::primaryWindow->title = "Fox2";

    SceneManager::currentScene = std::make_shared<Scene>(TestScene::Create());
    WindowManager::primaryWindow->Open();

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

    SceneManager::currentScene->UnloadResources();
    WindowManager::primaryWindow->Close();

    return 0;
}
