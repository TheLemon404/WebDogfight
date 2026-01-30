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

    template <typename T>
    std::vector<std::shared_ptr<T>> GetEntitiesByType() {
        std::vector<std::shared_ptr<T>> result;
        for(std::shared_ptr<Entity> entity : entities) {
            std::shared_ptr<T> cast = std::dynamic_pointer_cast<T>(entity);
            if(cast) {
                result.push_back(cast);
            }
        }
        return result;
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

    void LoadResources();
    void Initialize();
    void Update();
    void Draw();
    void UnloadResources();
};
