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

        std::shared_ptr<AircraftWidgetLayer> layer = std::make_shared<AircraftWidgetLayer>();
        testScene.widgetLayers.push_back(layer);
        return testScene;
    }
};
