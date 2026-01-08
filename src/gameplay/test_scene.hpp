#pragma once

#include "scene.hpp"
#include "aircraft.hpp"

class TestScene {
    public:
    static Scene Create() {
        Scene testScene = Scene();
        testScene.entities.push_back(std::make_unique<Aircraft>("resources/aircraft/FA-XX.json"));
        return testScene;
    }
};
