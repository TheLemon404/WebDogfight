#pragma once

#include "explosion.hpp"
#include "scene.hpp"
#include "aircraft.hpp"
#include "terrain.hpp"
#include "water.hpp"
#include "clouds.hpp"
#include "explosion.hpp"
#include "widget.hpp"
#include "../graphics/window.hpp"
#include "../networking/network_manager.hpp"
#include "../io/input.hpp"

class MenuWidgetLayer : public WidgetLayer {
    std::shared_ptr<InputWidget> nameInput = nullptr;
    std::shared_ptr<TextRectWidget> connectionStatus = nullptr;
    unsigned int connectionStatusId;

    void CreateWidgets() override;
    void UpdateLayer() override;
};

class LobbyWidgetLayer : public WidgetLayer {
    std::shared_ptr<InputWidget> codeInput = nullptr;

    void CreateWidgets() override;
    void UpdateLayer() override {};
};

class SettingsWidgetLayer : public WidgetLayer {
    void CreateWidgets() override;
    void UpdateLayer() override;
};

class TestScene {
    public:
    static Scene Create(const std::string& mapResourcePath) {
        Scene testScene = Scene();
        std::cout << "Attemping to read Scene JSON file at: " << mapResourcePath << std::endl;
        std::string resourceFileText = Files::ReadResourceString(mapResourcePath);
        json JSON = json::parse(resourceFileText);

        testScene.entities.push_back(std::make_shared<Terrain>("terrain", JSON["terrain"]));
        testScene.entities.push_back(std::make_shared<Water>("water", JSON["water"]));
        testScene.entities.push_back(std::make_shared<CloudsVolume>("clouds", JSON["clouds"]));
        testScene.entities.push_back(std::make_shared<ExplosionSystemEntity>("explosionSystem"));

        std::shared_ptr<MenuWidgetLayer> menuLayer = std::make_shared<MenuWidgetLayer>();
        testScene.widgetLayers.push_back(menuLayer);

        std::shared_ptr<LobbyWidgetLayer> lobbyLayer = std::make_shared<LobbyWidgetLayer>();
        lobbyLayer->invisible = true;
        testScene.widgetLayers.push_back(lobbyLayer);

        std::shared_ptr<SettingsWidgetLayer> settingsLayer = std::make_shared<SettingsWidgetLayer>();
        settingsLayer->invisible = true;
        testScene.widgetLayers.push_back(settingsLayer);
        return testScene;
    }
};
