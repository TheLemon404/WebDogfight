#include "scene.hpp"
#include "aircraft.hpp"
#include "entity.hpp"
#include "scene_manager.hpp"
#include "../utils/instrumentor.hpp"
#include "widget.hpp"
#include <memory>

void Scene::RuntimeSpawn(std::shared_ptr<Entity> entity) {
    spawnStack.push(entity);
}

void Scene::SpawnAndDespawnNetworkEntities(GameState& lastNetworkGameState, GameState& currentNetworkGameState) {
    for(auto& entry : lastNetworkGameState.clientStates) {
        if(!currentNetworkGameState.clientStates.contains(entry.first)) {
            //Since we only have <16 player lobbies, list iteration should be fast enough for now
            for(int i = 0; i < entities.size(); i++) {
                std::shared_ptr<Aircraft> aircraft = std::dynamic_pointer_cast<Aircraft>(entities[i]);
                if(aircraft && aircraft->networkId == entry.first) {
                    entities.erase(entities.begin() + i);
                }
            }
        }
    }

    for(auto& entry : currentNetworkGameState.clientStates) {
        bool wasInGame = lastNetworkGameState.clientStates.contains(entry.first) && lastNetworkGameState.clientStates[entry.first].inGame;
        if(!wasInGame && entry.second.inGame) {
            std::cout << "last state: " << lastNetworkGameState.clientStates[entry.first].inGame << std::endl;
            std::cout << "current state: " << entry.second.inGame << std::endl;
            std::cout << "spawning aircraft for client " << entry.first << std::endl;
            std::shared_ptr<Aircraft> newAircraft = std::make_shared<Aircraft>("FA-XX", "resources/aircraft/FA-XX.json", entry.first);
            newAircraft->transform.position = entry.second.position;
            newAircraft->transform.rotation = entry.second.rotation;
            RuntimeSpawn(newAircraft);
        }
    }
}

void Scene::LoadResourcesAsync() {
    isLoadingResources = true;
    switch(resourceLoadingState) {
        case BEGIN:
            {
                environment.skybox = std::make_shared<Skybox>();
                resourceLoadingState = LOADING_ENTITIES;
            }
            break;
        case LOADING_ENTITIES:
            {
                entities[resourceLoadingIndex]->LoadResources();
                resourceLoadingIndex++;
                if(resourceLoadingIndex == entities.size()) {
                    resourceLoadingIndex = 0;
                    resourceLoadingState = CREATING_UI;
                }
            }
            break;
        case CREATING_UI:
            {
                widgetLayers[resourceLoadingIndex]->CreateWidgets();
                resourceLoadingIndex++;
                if(resourceLoadingIndex == widgetLayers.size()) {
                    resourceLoadingIndex = 0;
                    resourceLoadingState = LOADING_UI;
                }
            }
            break;
        case LOADING_UI:
            {
                widgetLayers[resourceLoadingIndex]->LoadResources();
                resourceLoadingIndex++;
                if(resourceLoadingIndex == widgetLayers.size()) {
                    resourceLoadingIndex = 0;
                    resourceLoadingState = END;
                }
            }
            break;
        case END:
            {
                isLoadingResources = false;
                if(onResourcesLoadedCallback) onResourcesLoadedCallback();
            }
            break;
    }
}

void Scene::LoadResources() {
    for(std::shared_ptr<Entity>& entity : entities) {
        entity->LoadResources();
    }

    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->CreateWidgets();
        widgetLayer->LoadResources();
    }
}

void Scene::Initialize()  {
    for(std::shared_ptr<Entity>& entity : entities) {
        entity->Initialize();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->Initialize();
    }
}

void Scene::Update()  {
    while(!spawnStack.empty()) {
        std::shared_ptr<Entity> entity = spawnStack.top();
        entity->LoadResources();
        entity->Initialize();
        entities.push_back(entity);
        spawnStack.pop();
    }
    for(std::shared_ptr<Entity>& entity : entities) {
        entity->Update();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->UpdateLayer();
        widgetLayer->Update();
    }
}

void Scene::Draw() {
    if(environment.skybox) {
        FOX2_PROFILE_FUNCTION()
        GraphicsBackend::SetDepthMask(false);
        GraphicsBackend::SetBackfaceCulling(false);
        GraphicsBackend::DrawSkybox(*environment.skybox, SceneManager::activeCamera);
        GraphicsBackend::SetBackfaceCulling(true);
        GraphicsBackend::SetDepthMask(true);
    }

    for(std::shared_ptr<Entity>& entity : entities) {
        FOX2_PROFILE_SCOPE(entity->name.c_str())
        entity->Draw();
    }

    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        FOX2_PROFILE_SCOPE("widget layer")
        widgetLayer->Draw();
    }
}

void Scene::UnloadResources()  {
    for(std::shared_ptr<Entity>& entity : entities) {
        entity->UnloadResources();
    }
    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->UnloadResources();
    }
}
