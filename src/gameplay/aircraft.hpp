#pragma once

#include "../graphics/types.hpp"
#include "../graphics/backend.hpp"

#include <memory>
#include <future>
#include <iostream>

class Aircraft {
    public:
    Mesh testCube = Mesh(0,0,0,0,0);

    void LoadAssets() {
        testCube = GraphicsBackend::CreateCube();
    }
    void Initialize() {
    }
    void Update() {
    }
    void Draw() {
    }
    void UnloadAssets() {
    }
};
