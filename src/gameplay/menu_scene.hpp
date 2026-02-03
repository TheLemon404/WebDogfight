#pragma once

#include "scene.hpp"
#include "aircraft.hpp"
#include "terrain.hpp"
#include "widget.hpp"

class MenuWidgetLayer : public WidgetLayer {
    std::shared_ptr<InputWidget> codeInput;

    void CreateWidgets() override;
    void UpdateLayer() override {};
};

class MenuScene {
    public:
    static Scene Create() {
        Scene menuScene = Scene();
        std::shared_ptr<MenuWidgetLayer> layer = std::make_shared<MenuWidgetLayer>();
        menuScene.widgetLayers.push_back(layer);
        return menuScene;
    }
};
