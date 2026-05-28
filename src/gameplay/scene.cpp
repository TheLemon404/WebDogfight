#include "scene.hpp"
#include "aircraft.hpp"
#include "missile.hpp"
#include "entity.hpp"
#include "scene_manager.hpp"
#include "../utils/instrumentor.hpp"
#include "widget.hpp"
#include <memory>
#include "../application.hpp"

void Scene::RuntimeSpawn(std::shared_ptr<Entity> entity) {
    spawnQueue.push(entity);
}

void Scene::RuntimeDespawn(std::shared_ptr<Entity> entity) {
    entity->pendingDespawn = true;
    despawnQueue.push(entity);
}

void Scene::SpawnAndDespawnNetworkEntities(GameState& lastNetworkGameState, GameState& currentNetworkGameState) {
    for(auto& entry : lastNetworkGameState.clientStates) {
        bool isInGame = currentNetworkGameState.clientStates.contains(entry.first) && currentNetworkGameState.clientStates[entry.first].inGame;
        if(!isInGame) {
            //Since we only have <16 player lobbies, list iteration should be fast enough for now
            for(int i = 0; i < entities.size(); i++) {
                std::shared_ptr<Aircraft> aircraft = std::dynamic_pointer_cast<Aircraft>(entities[i]);
                if(aircraft && aircraft->networkId == entry.first && !aircraft->pendingDespawn) {
                    RuntimeDespawn(aircraft);
                }
            }
        }
    }

    for(auto& entry : currentNetworkGameState.clientStates) {
        bool wasInGame = lastNetworkGameState.clientStates.contains(entry.first) && lastNetworkGameState.clientStates[entry.first].inGame;
        if(!wasInGame && entry.second.inGame) {
            std::shared_ptr<Aircraft> newAircraft = std::make_shared<Aircraft>("FA-XX", "resources/aircraft/FA-XX.json", entry.first);
            newAircraft->transform.position = entry.second.position;
            newAircraft->transform.rotation = entry.second.rotation;
            RuntimeSpawn(newAircraft);
        }
    }

    for(auto& entry : lastNetworkGameState.missileMap) {
        bool isInGame = currentNetworkGameState.missileMap.contains(entry.first);
        std::cout << currentNetworkGameState.missileMap.size() << std::endl;
        if(!isInGame) {
            for(int i = 0; i < entities.size(); i++) {
                std::shared_ptr<Missile> missile = std::dynamic_pointer_cast<Missile>(entities[i]);
                if(missile && missile->networkId == entry.first && !missile->pendingDespawn) {
                    RuntimeDespawn(missile);
                }
            }
        }
    }

    for(auto& entry : currentNetworkGameState.missileMap) {
        bool wasInGame = lastNetworkGameState.missileMap.contains(entry.first);
        if(!wasInGame) {
            std::shared_ptr<Missile> newMissile = std::make_shared<Missile>("FA-XX", entry.first);
            newMissile->transform.position = entry.second.position;
            newMissile->transform.rotation = entry.second.rotation;
            newMissile->velocity = entry.second.velocity;
            RuntimeSpawn(newMissile);
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
    environment.skybox = std::make_shared<Skybox>();

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
    std::unique_ptr<Application>& app = Application::GetInstance();

    if(app->networkManager.hasPendingStateChange) {
        SpawnAndDespawnNetworkEntities(app->networkManager.lastNetworkGameState, app->networkManager.networkGameState);
        app->networkManager.hasPendingStateChange = false;
    }

    bool sortFlag = false;
    while(!spawnQueue.empty()) {
        std::shared_ptr<Entity> entity = spawnQueue.front();
        entity->LoadResources();
        entity->Initialize();
        entities.push_back(entity);
        spawnQueue.pop();
        sortFlag = true;
    }

    if(sortFlag) {
        std::sort(entities.begin(), entities.end(), [](std::shared_ptr<Entity>& a, std::shared_ptr<Entity>& b) {
            return a->drawPriority < b->drawPriority;
        });

    }

    bool lockedFlag = false;
    for(std::shared_ptr<Entity> entity : entities) {
        entity->Update();

        std::shared_ptr<Aircraft> aircraft = std::dynamic_pointer_cast<Aircraft>(entity);
        if(aircraft != nullptr && aircraft->lockedAircraftNetworkId != 0 && aircraft->lockedAircraftNetworkId == app->networkManager.localClientId) {
            lockedFlag = true;
        }
    }

    if(!app->audioBackend.globalSounds.lockAlert.started && lockedFlag) {
        app->audioBackend.StartSoundAsset(app->audioBackend.globalSounds.lockAlert, true, 1.0f);
    }
    else if (app->audioBackend.globalSounds.lockAlert.started && !lockedFlag) {
        app->audioBackend.EndSoundAsset(app->audioBackend.globalSounds.lockAlert);
    }

    for(std::shared_ptr<WidgetLayer>& widgetLayer : widgetLayers) {
        widgetLayer->UpdateLayer();
        widgetLayer->Update();
    }
    while(!despawnQueue.empty()) {
        std::shared_ptr<Entity> entity = despawnQueue.front();
        entity->UnloadResources();
        entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
        despawnQueue.pop();
    }
}

void Scene::DrawScene() {
    std::unique_ptr<Application>& app = Application::GetInstance();

    if(environment.skybox) {
        FOX2_PROFILE_FUNCTION()
        app->graphicsBackend.SetDepthMask(false);
        app->graphicsBackend.SetBackfaceCulling(false);
        app->graphicsBackend.DrawSkybox(*environment.skybox, app->sceneManager.activeCamera);
        app->graphicsBackend.SetBackfaceCulling(true);
        app->graphicsBackend.SetDepthMask(true);
    }

    for(std::shared_ptr<Entity>& entity : entities) {
        FOX2_PROFILE_SCOPE(entity->name.c_str())
        entity->Draw();
    }
}

void Scene::DrawUI() {
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
