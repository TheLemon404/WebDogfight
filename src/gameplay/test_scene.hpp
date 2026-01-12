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

        std::shared_ptr<WidgetLayer> layer = std::make_shared<WidgetLayer>();
        std::shared_ptr<CircleWidget> aim = std::make_shared<CircleWidget>("circleWidget");
        layer->widgets.push_back(aim);
        testScene.widgetLayers.push_back(layer);
        return testScene;
    }
};
