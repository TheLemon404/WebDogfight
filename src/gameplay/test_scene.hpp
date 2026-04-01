#pragma once

#include "scene.hpp"
#include "aircraft.hpp"
#include "terrain.hpp"
#include "water.hpp"
#include "clouds.hpp"
#include "widget.hpp"
#include "../graphics/window.hpp"
#include "../networking/network_manager.hpp"
#include "../io/input.hpp"

class MenuWidgetLayer : public WidgetLayer {
    std::shared_ptr<InputWidget> nameInput = nullptr;
    std::shared_ptr<InputWidget> codeInput = nullptr;
    std::shared_ptr<TextRectWidget> connectionStatus = nullptr;
    unsigned int connectionStatusId;

    void CreateWidgets() override;
    void UpdateLayer() override;
};

class SettingsWidgetLayer : public WidgetLayer {
    void CreateWidgets() override;
    void UpdateLayer() override;
};

class TestScene {
    public:
    static Scene Create() {
        Scene testScene = Scene();
        testScene.entities.push_back(std::make_shared<Terrain>("terrain", "resources/terrains/default.json"));
        testScene.entities.push_back(std::make_shared<Water>("water"));

        std::shared_ptr<CloudsVolume> cloud = std::make_shared<CloudsVolume>("clouds");
        cloud->transform.position.y = 15000.0f;
        cloud->transform.scale = glm::vec3(50000.0f, 5000.0f, 50000.0f);
        testScene.entities.push_back(cloud);

        std::shared_ptr<MenuWidgetLayer> menuLayer = std::make_shared<MenuWidgetLayer>();
        testScene.widgetLayers.push_back(menuLayer);

        std::shared_ptr<SettingsWidgetLayer> settingsLayer = std::make_shared<SettingsWidgetLayer>();
        settingsLayer->SetDisabled(true);
        testScene.widgetLayers.push_back(settingsLayer);
        return testScene;
    }
};
