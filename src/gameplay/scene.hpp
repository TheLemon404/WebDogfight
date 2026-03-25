#pragma once

#include "../networking/network_game.hpp"
#include "entity.hpp"
#include "environment.hpp"
#include "widget.hpp"
#include <string>
#include <vector>
#include <queue>
#include <memory>

enum SceneResourceLoadingState {
    BEGIN,
    LOADING_ENTITIES,
    CREATING_UI,
    LOADING_UI,
    END
};

class Scene {
    std::queue<std::shared_ptr<Entity>> spawnQueue;
    std::queue<std::shared_ptr<Entity>> despawnQueue;

    public:
    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<WidgetLayer>> widgetLayers;
    Environment environment;
    SceneResourceLoadingState resourceLoadingState = SceneResourceLoadingState::BEGIN;

    bool isLoadingResources = false;
    std::function<void()> onResourcesLoadedCallback;
    int resourceLoadingIndex = 0;

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
            if(auto widget = widgetLayer->GetWidgetByName(name)) {
                return widget;
            }
        }

        return nullptr;
    }

    std::shared_ptr<Widget> GetWidgetById(const unsigned int id) {
        for(std::shared_ptr<WidgetLayer> widgetLayer : widgetLayers) {
            if(auto widget = widgetLayer->GetWidgetById(id)) {
                return widget;
            }
        }

        return nullptr;
    }

    //IMPORTANT -- This method is poorly written. MAKE SURE TO NOT USE MORE THAN 1 OF A WIDGET LAYER TYPE IN A SCENE
    template <typename T>
    std::shared_ptr<T> GetWidgetLayerByType() {
        for(std::shared_ptr<WidgetLayer> widgetLayer : widgetLayers) {
            std::shared_ptr<T> cast = std::dynamic_pointer_cast<T>(widgetLayer);
            if(cast) {
                return cast;
            }
        }
        return nullptr;
    }

    void RuntimeSpawn(std::shared_ptr<Entity> entity);
    void RuntimeDespawn(std::shared_ptr<Entity> entity);

    void SpawnAndDespawnNetworkEntities(GameState& lastNetworkGameState, GameState& currentNetworkGameState);

    void LoadResourcesAsync();
    void LoadResources();
    void Initialize();
    void Update();
    void Draw();
    void UnloadResources();
};
