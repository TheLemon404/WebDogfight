#pragma once

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

class Application {
    static std::unique_ptr<Application> instance;
    static std::mutex applicationInstanceMutex;

    public:
    Application() {};
    Application(Application& other) = delete;
    void operator=(const Application& other) = delete;

    static std::unique_ptr<Application>& GetInstance();

    GraphicsBackend graphicsBackend;
    SceneManager sceneManager;
    AudioBackend audioBackend;
    NetworkManager networkManager;
    WindowManager windowManager;
    Clock clock;
};
