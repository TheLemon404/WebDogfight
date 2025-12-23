#pragma once

#include "aircraft.hpp"
#include <vector>

class Scene {
    std::vector<Aircraft> aircrafts;

public:
    void LoadAssets();
    void Initialize();
    void Update();
    void Draw();
    void UnloadAssets();
};
