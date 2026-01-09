#pragma once

#include "entity.hpp"
#include "environment.hpp"
#include "ui.hpp"
#include <string>
#include <vector>
#include <memory>

class Scene {
    public:
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Widget>> widgets;
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
        for(std::shared_ptr<Widget> widget : widgets) {
            if(widget->name == name) return widget;
        }

        return nullptr;
    }

    std::shared_ptr<Widget> GetWidgetById(const unsigned int id) {
        for(std::shared_ptr<Widget> widget : widgets) {
            if(widget->id == id) return widget;
        }

        return nullptr;
    }

    void LoadResources() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->LoadResources();
        }
        for(std::shared_ptr<Widget>& widget : widgets) {
            widget->LoadResources();
        }
    }
    void Initialize() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->Initialize();
        }
        for(std::shared_ptr<Widget>& widget : widgets) {
            widget->Initialize();
        }
    }
    void Update() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->Update();
        }
        for(std::shared_ptr<Widget>& widget : widgets) {
            widget->Update();
        }
    }
    void Draw() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->Draw();
        }
        for(std::shared_ptr<Widget>& widget : widgets) {
            widget->Draw();
        }
    }
    void UnloadResources() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->UnloadResources();
        }
        for(std::shared_ptr<Widget>& widget : widgets) {
            widget->UnloadResources();
        }
    }
};
