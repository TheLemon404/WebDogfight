#pragma once

#include "scene.hpp"
#include "aircraft.hpp"
#include "terrain.hpp"
#include "widget.hpp"

class TestScene {
    public:
    static Scene Create() {
        Scene testScene = Scene();
        testScene.entities.push_back(std::make_shared<Aircraft>("FA-XX", "resources/aircraft/FA-XX.json"));
        testScene.entities.push_back(std::make_shared<Terrain>("terrain"));
        std::shared_ptr<CircleWidget> circle = std::make_shared<CircleWidget>("circleWidget");
        circle->position.x = -1.0;
        testScene.widgets.push_back(circle);
        return testScene;
    }
};
