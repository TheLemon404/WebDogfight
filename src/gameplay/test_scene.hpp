#pragma once

#include "scene.hpp"
#include "aircraft.hpp"
#include "terrain.hpp"
#include "ui.hpp"

class TestScene {
    public:
    static Scene Create() {
        Scene testScene = Scene();
        testScene.entities.push_back(std::make_shared<Aircraft>("FA-XX", "resources/aircraft/FA-XX.json"));
        testScene.entities.push_back(std::make_shared<Terrain>("terrain"));
        testScene.widgets.push_back(std::make_shared<Widget>("testWidget", WidgetType::RING));
        return testScene;
    }
};
