#pragma once

#include "scene.hpp"
#include <memory>

class SceneManager {
    public:
    inline static std::shared_ptr<Scene> currentScene;
};
