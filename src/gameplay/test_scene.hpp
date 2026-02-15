#pragma once

#include "scene.hpp"
#include "aircraft.hpp"
#include "terrain.hpp"
#include "water.hpp"
#include "clouds.hpp"
#include "widget.hpp"
#include "../graphics/window.hpp"
#include "../io/input.hpp"

class TestScene {
    public:
    static Scene Create() {
        InputManager::mouseHidden = true;
        glfwSetInputMode(WindowManager::primaryWindow->window, GLFW_CURSOR, InputManager::mouseHidden ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

        Scene testScene = Scene();
        testScene.entities.push_back(std::make_shared<Terrain>("terrain", "resources/terrains/default.json"));
        testScene.entities.push_back(std::make_shared<Water>("water"));
        std::shared_ptr<CloudsVolume> cloud = std::make_shared<CloudsVolume>("clouds");
        cloud->transform.position.y = 15000.0f;
        cloud->transform.scale = glm::vec3(50000.0f, 5000.0f, 50000.0f);
        testScene.entities.push_back(cloud);
        testScene.entities.push_back(std::make_shared<Aircraft>("FA-XX", "resources/aircraft/FA-XX.json"));

        std::shared_ptr<AircraftWidgetLayer> layer = std::make_shared<AircraftWidgetLayer>();
        testScene.widgetLayers.push_back(layer);
        return testScene;
    }
};
