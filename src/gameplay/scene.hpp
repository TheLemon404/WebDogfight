#pragma once

#include "entity.hpp"
#include "environment.hpp"
#include <string>
#include <vector>
#include <memory>

class Scene {
    public:
    std::vector<std::shared_ptr<Entity>> entities;
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

    void LoadResources() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->LoadResources();
        }
    }
    void Initialize() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->Initialize();
        }
    }
    void Update() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->Update();
        }
    }
    void Draw() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->Draw();
        }
    }
    void UnloadResources() {
        for(std::shared_ptr<Entity>& entity : entities) {
            entity->UnloadResources();
        }
    }
};
