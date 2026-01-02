#pragma once

#include "entity.hpp"
#include "environment.hpp"
#include <string>
#include <vector>
#include <memory>

class Scene {
    public:
    std::vector<std::unique_ptr<Entity>> entities;
    Environment environment;

    void LoadResources() {
        for(std::unique_ptr<Entity>& entity : entities) {
            entity->LoadResources();
        }
    }
    void Initialize() {
        for(std::unique_ptr<Entity>& entity : entities) {
            entity->Initialize();
        }
    }
    void Update() {
        for(std::unique_ptr<Entity>& entity : entities) {
            entity->Update();
        }
    }
    void Draw() {
        for(std::unique_ptr<Entity>& entity : entities) {
            entity->Draw();
        }
    }
    void UnloadResources() {
        for(std::unique_ptr<Entity>& entity : entities) {
            entity->UnloadResources();
        }
    }
};
