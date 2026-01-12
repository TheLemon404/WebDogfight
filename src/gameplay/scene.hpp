#pragma once

#include "entity.hpp"
#include "environment.hpp"
#include "widget.hpp"
#include <string>
#include <vector>
#include <memory>

class Scene {
    public:
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<WidgetLayer>> widgetLayers;
    Environment environment;

    std::shared_ptr<Entity> GetEntityByName(const std::string& name) {
        for(std::shared_ptr<Entity> entity : entities) {
            if(entity->name == name) return entity;
        }

        return nullptr;
    }

    std::shared_ptr<Entity> GetEntityById(const unsigned int id) {
        for(std::shared_ptr<Entity> entity : entities) {
            if(entity->id == id) return entity;
        }

        return nullptr;
    }

    std::shared_ptr<Widget> GetWidgetByName(const std::string& name) {
        for(std::shared_ptr<WidgetLayer> widgetLayer : widgetLayers) {
            return widgetLayer->GetWidgetByName(name);
        }

        return nullptr;
    }

    std::shared_ptr<Widget> GetWidgetById(const unsigned int id) {
        for(std::shared_ptr<WidgetLayer> widgetLayer : widgetLayers) {
            return widgetLayer->GetWidgetById(id);
        }

        return nullptr;
    }

    void LoadResources() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->LoadResources();
        }
        for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
            widgetLayer->CreateWidgets();
            widgetLayer->LoadResources();
        }
    }
    void Initialize() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->Initialize();
        }
        for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
            widgetLayer->Initialize();
        }
    }
    void Update() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->Update();
        }
        for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
            widgetLayer->UpdateLayer();
            widgetLayer->Update();
        }
    }
    void Draw() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->Draw();
        }
        for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
            widgetLayer->Draw();
        }
    }
    void UnloadResources() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->UnloadResources();
        }
        for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
            widgetLayer->UnloadResources();
        }
    }
};
