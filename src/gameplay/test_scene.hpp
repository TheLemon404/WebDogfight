#pragma once

#include "scene.hpp"
#include "aircraft.hpp"
#include "terrain.hpp"

class TestScene {
    public:
    static Scene Create() {
        Scene testScene = Scene();
        testScene.entities.push_back(std::make_shared<Aircraft>("FA-XX", "resources/aircraft/FA-XX.json"));
        testScene.entities.push_back(std::make_shared<Terrain>("terrain"));
        return testScene;
    }
};
